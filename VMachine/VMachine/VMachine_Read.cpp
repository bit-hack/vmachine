//	VMachine
//	Read memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "VideoCard/VideoCard.h"

Byte VMachine::ReadBytePhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000)
		return memory[address];

	else if(address < 0xc0000)
		return videoCard->ReadByte(0xa0000, address - 0xa0000);

	else if(address < registers->memorySize)
		return memory[address];
	
	else
		return 0xff;
}

Word VMachine::ReadWordPhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000 - 1)
		return *reinterpret_cast<Word *>(&memory[address]);

	else if(address >= 0xa0000 && address < 0xc0000 - 1)
		return videoCard->ReadWord(0xa0000, address - 0xa0000);

	else if(address >= 0xc0000 && address < registers->memorySize - 1)
		return *reinterpret_cast<Word *>(&memory[address]);

	else
		return 0xffff;
}

Dword VMachine::ReadDwordPhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000 - 3)
		return *reinterpret_cast<Dword *>(&memory[address]);

	else if(address >= 0xa0000 && address < 0xc0000 - 3)
		return videoCard->ReadDword(0xa0000, address - 0xa0000);

	else if(address >= 0xc0000 && address < registers->memorySize - 3)
		return *reinterpret_cast<Dword *>(&memory[address]);

	else
		return 0xffffffff;
}

bool VMachine::TranslateAddressForRead(bool userMode, Dword & address)
{
	Dword pde = ReadDwordPhysical(
		(registers->r_cr3 & 0xfffff000) | ((address & 0xffc00000) >> 20));

	if((pde & PDE_PRESENT_BIT) == 0 || (userMode && (pde & PDE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode = PAGE_FAULT_ERROR_CODE_READ;
		registers->errorCode |= ((pde & PDE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->errorCode |= (userMode)
			? PAGE_FAULT_ERROR_CODE_USER_MODE
			: PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		registers->r_cr2 = address;
		
		return false;
	}

	Dword pte = ReadDwordPhysical(
		(pde & 0xfffff000) | ((address & 0x003ff000) >> 10));

	if((pte & PTE_PRESENT_BIT) == 0 || (userMode && (pte & PTE_USER_BIT) == 0))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode = PAGE_FAULT_ERROR_CODE_READ;
		registers->errorCode |= ((pte & PTE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		registers->errorCode |= (userMode)
			? PAGE_FAULT_ERROR_CODE_USER_MODE
			: PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		registers->r_cr2 = address;
		
		return false;
	}

	WriteDwordPhysical(	(registers->r_cr3 & 0xfffff000) | ((address & 0xffc00000) >> 20),
						pde | PDE_ACCESSED_BIT);
	
	WriteDwordPhysical(	(pde & 0xfffff000) | ((address & 0x003ff000) >> 10),
						pte | PTE_ACCESSED_BIT);

	address = (pte & 0xfffff000) | (address & 0x00000fff);

	return true;
}

bool VMachine::ReadByteLinear(Dword address, Byte & data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//Perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForRead(false, address))
			return false;
	}

	data = ReadBytePhysical(address);

	return true;
}

bool VMachine::ReadWordLinear(Dword address, Word & data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the read crosses a page boundary, split it into 2 byte reads
		if((address & 0xfff) == 0xfff)
		{
			Byte lowByte, highByte;

			if(!ReadByteLinear(address, lowByte) || !ReadByteLinear(address + 1, highByte))
				return false;

			data = (static_cast<Word>(highByte) << 8) | lowByte;

			return true;
		}

        //Otherwise, perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForRead(false, address))
			return false;
	}
			
	data = ReadWordPhysical(address);

	return true;
}

bool VMachine::ReadDwordLinear(Dword address, Dword & data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the read crosses a page boundary, split it into 2 word reads
		if((address & 0xfff) == 0xffd || (address & 0xffe) == 0xffe)
		{
			Word lowWord, highWord;

			if(!ReadWordLinear(address, lowWord) || !ReadWordLinear(address + 2, highWord))
				return false;

			data = (static_cast<Dword>(highWord) << 16) | lowWord;

			return true;
		}

		//Otherwise, perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForRead(false, address))
			return false;
	}

	data = ReadDwordPhysical(address);

	return true;
}

bool VMachine::ReadByte(const SegmentRegister & segReg, Dword offset, Byte & data)
{
	Dword linAddr;

	if(!segReg.VerifyByteRead(offset, linAddr))
		return false;

	return ReadByteLinear(linAddr, data);
}

bool VMachine::ReadWord(const SegmentRegister & segReg, Dword offset, Word & data)
{
	Dword linAddr;

	if(!segReg.VerifyWordRead(offset, linAddr))
		return false;

	return ReadWordLinear(linAddr, data);
}

bool VMachine::ReadDword(const SegmentRegister & segReg, Dword offset, Dword & data)
{
	Dword linAddr;

	if(!segReg.VerifyDwordRead(offset, linAddr))
		return false;

	return ReadDwordLinear(linAddr, data);
}

Byte VMachine::ReadCodeBytePhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000 || (address >= 0xc0000 && address < registers->memorySize))
		return memory[address];

	throw Ex("CPU Error: Execution out of bounds");
}

Word VMachine::ReadCodeWordPhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000 - 1 || (address >= 0xc0000 && address < registers->memorySize - 1))
		return *reinterpret_cast<Word *>(&memory[address]);

	throw Ex("CPU Error: Execution out of bounds");
}

Dword VMachine::ReadCodeDwordPhysical(Dword address)
{
	address &= registers->addressMask;

	if(address < 0xa0000 - 3 || (address >= 0xc0000 && address < registers->memorySize - 3))
		return *reinterpret_cast<Dword *>(&memory[address]);

	throw Ex("CPU Error: Execution out of bounds");
}

bool VMachine::TranslateAddressForReadCode(bool userMode, Dword & address)
{
	Dword pde = ReadDwordPhysical(
		(registers->r_cr3 & 0xfffff000) | ((address & 0xffc00000) >> 20));

	if((pde & PDE_PRESENT_BIT) == 0 || (userMode && (pde & PDE_USER_BIT) == 0))
	{
		readCodeException = EXCEPTION_PAGE_FAULT;
		readCodeErrorCode = PAGE_FAULT_ERROR_CODE_READ;
		readCodeErrorCode |= ((pde & PDE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		readCodeErrorCode |= (userMode)
			? PAGE_FAULT_ERROR_CODE_USER_MODE
			: PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		readCodeCr2 = address;
		
		return false;
	}

	Dword pte = ReadDwordPhysical(
		(pde & 0xfffff000) | ((address & 0x003ff000) >> 10));

	if((pte & PTE_PRESENT_BIT) == 0 || (userMode && (pte & PTE_USER_BIT) == 0))
	{
		readCodeException = EXCEPTION_PAGE_FAULT;
		readCodeErrorCode = PAGE_FAULT_ERROR_CODE_READ;
		readCodeErrorCode |= ((pte & PTE_PRESENT_BIT) == 0)
			? PAGE_FAULT_ERROR_CODE_NOT_PRESENT_FAULT
			: PAGE_FAULT_ERROR_CODE_PROTECTION_FAULT;
		readCodeErrorCode |= (userMode)
			? PAGE_FAULT_ERROR_CODE_USER_MODE
			: PAGE_FAULT_ERROR_CODE_SUPERVISOR_MODE;
		readCodeCr2 = address;
		
		return false;
	}

	WriteDwordPhysical(	(registers->r_cr3 & 0xfffff000) | ((address & 0xffc00000) >> 20),
						pde | PDE_ACCESSED_BIT);
	
	WriteDwordPhysical(	(pde & 0xfffff000) | ((address & 0x003ff000) >> 10),
						pte | PTE_ACCESSED_BIT);

	address = (pte & 0xfffff000) | (address & 0x00000fff);

	return true;
}

bool VMachine::ReadCodeByte(Dword offset, Byte & data)
{
	Dword address = registers->r_cs.base + offset;

	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//Perform address translation
		bool userMode = (registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
						DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForReadCode(userMode, address))
			return false;
	}

	data = ReadCodeBytePhysical(address);

	return true;
}

bool VMachine::ReadCodeWord(Dword offset, Word & data)
{
	Dword address = registers->r_cs.base + offset;

	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the read crosses a page boundary, split it into 2 byte reads
		if((address & 0xfff) == 0xfff)
		{
			Byte lowByte, highByte;

			if(!ReadCodeByte(offset, lowByte) || !ReadCodeByte(offset + 1, highByte))
				return false;

			data = (static_cast<Word>(highByte) << 8) | lowByte;

			return true;
		}

		//Otherwise, perform address translation
		bool userMode = (registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
						DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForReadCode(userMode, address))
			return false;
	}

	data = ReadCodeWordPhysical(address);

	return true;
}

bool VMachine::ReadCodeDword(Dword offset, Dword & data)
{
	Dword address = registers->r_cs.base + offset;

	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the read crosses a page boundary, split it into 2 word reads
		if((address & 0xfff) == 0xffd || (address & 0xffe) == 0xffe)
		{
			Word lowWord, highWord;

			if(!ReadCodeWord(offset, lowWord) || !ReadCodeWord(offset + 2, highWord))
				return false;

			data = (static_cast<Dword>(highWord) << 16) | lowWord;

			return true;
		}

		//Otherwise, perform address translation
		bool userMode = (registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
						DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForReadCode(userMode, address))
			return false;
	}

	data = ReadCodeDwordPhysical(address);

	return true;
}
