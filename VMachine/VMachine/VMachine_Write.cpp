//	VMachine
//	Write memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "VideoCard/VideoCard.h"

void VMachine::WriteBytePhysical(Dword address, Byte data)
{
	address &= registers->addressMask;

	if(registers->pageClassifications[address >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
		InvalidateCodeTranslations(address, address + 1, data);

	if(address < 0xa0000)
		memory[address] = data;

	else if(address < 0xc0000)
		videoCard->WriteByte(0xa0000, address - 0xa0000, data);

	else if(address >= 0x100000 && address < registers->memorySize)
		memory[address] = data;
}

void VMachine::WriteWordPhysical(Dword address, Word data)
{
	address &= registers->addressMask;

	if(	registers->pageClassifications[address >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS ||
		registers->pageClassifications[(address + 1) >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
	{
		InvalidateCodeTranslations(address, address + 2, data);
	}

	if(address < 0xa0000 - 1)
		*reinterpret_cast<Word *>(&memory[address]) = data;

	else if(address >= 0xa0000 && address < 0xc0000 - 1)
		videoCard->WriteWord(0xa0000, address - 0xa0000, data);

	else if(address >= 0x100000 && address < registers->memorySize - 1)
		*reinterpret_cast<Word *>(&memory[address]) = data;
}

void VMachine::WriteDwordPhysical(Dword address, Dword data)
{
	address &= registers->addressMask;

	if(	registers->pageClassifications[address >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS ||
		registers->pageClassifications[(address + 3) >> 12] == PAGE_CLASS_CONTAINS_TRANSLATIONS)
	{
		InvalidateCodeTranslations(address, address + 4, data);
	}

	if(address < 0xa0000 - 3)
		*reinterpret_cast<Dword *>(&memory[address]) = data;

	else if(address >= 0xa0000 && address < 0xc0000 - 3)
		videoCard->WriteDword(0xa0000, address - 0xa0000, data);

	else if(address >= 0x100000 && address < registers->memorySize - 3)
		*reinterpret_cast<Dword *>(&memory[address]) = data;
}

bool VMachine::TranslateAddressForWrite(bool userMode, Dword & address)
{
	Dword pde = ReadDwordPhysical(
		(registers->r_cr3 & 0xfffff000) | ((address & 0xffc00000) >> 20));

	if(	(pde & PDE_PRESENT_BIT) == 0 ||
		(userMode && ((pde & PDE_WRITABLE_BIT) == 0 || (pde & PDE_USER_BIT) == 0)))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode = PAGE_FAULT_ERROR_CODE_WRITE;
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

	if(	(pte & PTE_PRESENT_BIT) == 0 ||
		(userMode && ((pte & PTE_WRITABLE_BIT) == 0 || (pte & PTE_USER_BIT) == 0)))
	{
		registers->exception = EXCEPTION_PAGE_FAULT;
		registers->errorCode = PAGE_FAULT_ERROR_CODE_WRITE;
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
						pte | PTE_ACCESSED_BIT | PTE_DIRTY_BIT);

	address = (pte & 0xfffff000) | (address & 0x00000fff);

	return true;
}

bool VMachine::WriteByteLinear(Dword address, Byte data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//Perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForWrite(false, address))
			return false;
	}

	WriteBytePhysical(address, data);

	return true;
}

bool VMachine::WriteWordLinear(Dword address, Word data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the write crosses a page boundary, split it into 2 byte writes
		if((address & 0xfff) == 0xfff)
		{
			Byte lowByte = static_cast<Byte>(data);
			Byte highByte = static_cast<Byte>(data >> 8);

			return (WriteByteLinear(address, lowByte) && WriteByteLinear(address + 1, highByte));
		}

		//Otherwise, perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForWrite(false, address))
			return false;
	}

	WriteWordPhysical(address, data);

	return true;
}

bool VMachine::WriteDwordLinear(Dword address, Dword data, bool forceSupervisorMode)
{
	//If paging is enabled
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
	{
		//If the write crosses a page boundary, split it into 2 word writes
		if((address & 0xfff) == 0xffd || (address & 0xffe) == 0xffe)
		{
			Word lowWord = static_cast<Word>(data);
			Word highWord = static_cast<Word>(data >> 16);

			return (WriteWordLinear(address, lowWord) && WriteWordLinear(address + 2, highWord));
		}

		//Otherwise, perform address translation
		bool userMode = !forceSupervisorMode &&
							(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
								DESCRIPTOR_FLAGS_DPL_MASK;

		if(!TranslateAddressForWrite(false, address))
			return false;
	}

	WriteDwordPhysical(address, data);

	return true;
}

bool VMachine::WriteByte(const SegmentRegister & segReg, Dword offset, Byte data)
{
	Dword linAddr;

	if(!segReg.VerifyByteWrite(offset, linAddr))
		return false;

	return WriteByteLinear(linAddr, data);
}

bool VMachine::WriteWord(const SegmentRegister & segReg, Dword offset, Word data)
{
	Dword linAddr;

	if(!segReg.VerifyWordWrite(offset, linAddr))
		return false;

	return WriteWordLinear(linAddr, data);
}

bool VMachine::WriteDword(const SegmentRegister & segReg, Dword offset, Dword data)
{
	Dword linAddr;

	if(!segReg.VerifyDwordWrite(offset, linAddr))
		return false;

	return WriteDwordLinear(linAddr, data);
}
