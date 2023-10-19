//	VMachine
//	Far return instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::DoFarReturn(Dword opcode, bool iret, Word stackPointerOffset)
{
	assert(!(iret && stackPointerOffset != 0));

	//Get the destination
	Word newCs;
	Dword newEip, newEflags;

	if((opcode & OPCODE_O32) == 0)
	{
		Word newIp, newFlags;

		if(!PopWord(newIp) || !PopWord(newCs))
			return;

		newEip = newIp;

		if(iret)
		{
			if(!PopWord(newFlags))
				return;
				
			newEflags = (registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK) | registers->r_systemFlags;
			newEflags &= 0xffff0000;
			newEflags |= newFlags;
		}
	}
	else
	{
		Dword tempCs;

		if(!PopDword(newEip) || !PopDword(tempCs))
			return;

		newCs = static_cast<Word>(tempCs);

		if(iret)
		{
			if(!PopDword(newEflags))
				return;
		}
	}

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real mode / V8086 mode

		//If a V8086 mode IRET with IOPL != 3, #GP(0)
		if(	iret && (registers->r_systemFlags & EFLAGS_VM_FLAG) != 0 &&
			(registers->r_systemFlags & EFLAGS_IOPL_FLAG) != EFLAGS_IOPL_FLAG)
		{
			registers->exception = EXCEPTION_GENERAL_PROTECTION;
			registers->errorCode = 0;

			return;
		}

		//Set the new cs:eip and eflags values
		if(!SetCSValue(newCs))
			return;
	
		registers->r_eip = newEip;

		if(iret)
		{
			Dword oldSystemFlags = registers->r_systemFlags;

			registers->r_eflags = newEflags & EFLAGS_STATUS_FLAGS_MASK;
			registers->r_systemFlags = newEflags & EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;

			//If a V8086 mode IRET, VM and IOPL not modified
			if((oldSystemFlags & EFLAGS_VM_FLAG) != 0)
			{
				registers->r_systemFlags &= ~(EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
				registers->r_systemFlags |= oldSystemFlags & (EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
			}
		}
	}
	else if(iret && (newEflags & EFLAGS_VM_FLAG) != 0)
	{
		//IRET to V8086 mode

		//Set the new eflags value, hence switching to V8086 mode
		registers->r_eflags = newEflags & EFLAGS_STATUS_FLAGS_MASK;
		registers->r_systemFlags = newEflags & EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;

		//Set the new cs:eip values
		if(!SetCSValue(newCs))
			return;
	
		registers->r_eip = newEip;

		//Get the new ss:esp values
		Word newSs;
		Dword newEsp;

		//TODO: Does operand size affect whether Words or Dwords are popped?
		if((opcode & OPCODE_O32) == 0)
		{
			Word newSp;

			if(!PopWord(newSp) || !PopWord(newSs))
				return;
			
			newEsp = newSp;
		}
		else
		{
			Dword tempSs;

			if(!PopDword(newEsp) || !PopDword(tempSs))
				return;

			newSs = static_cast<Word>(tempSs);
		}

		//Set the values of the data segment registers
		Dword newEs, newDs, newFs, newGs;

		//TODO: Again, does operand size affect whether Words or Dwords are popped?
		if(!PopDword(newEs) || !PopDword(newDs) || !PopDword(newFs) || !PopDword(newGs))
			return;

		if(	!SetDataSegmentRegisterValue(&r_ds, static_cast<Word>(newDs)) ||
			!SetDataSegmentRegisterValue(&r_es, static_cast<Word>(newEs)) ||
			!SetDataSegmentRegisterValue(&r_fs, static_cast<Word>(newFs)) ||
			!SetDataSegmentRegisterValue(&r_gs, static_cast<Word>(newGs)))
		{
			return;
		}

		//Switch to the new stack
		if(!SetDataSegmentRegisterValue(&r_ss, newSs))
			return;

		registers->r_esp = newEsp;
	}
	else
	{
		//Protected mode
		Byte initialCpl = static_cast<Byte>(
			(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
				>> DESCRIPTOR_FLAGS_DPL_SHIFT);

		//Get the base of the descriptor table
		Dword tableBase = registers->r_gdtr.base;

		if((newCs & SELECTOR_TI_BIT) != 0)
			tableBase = registers->r_ldtr.base;

		//Read the segment descriptor
		Dword descriptorLow, descriptorHigh;

		if(	!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK), descriptorLow) ||
			!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
		{
			return;
		}

		//If RPL < CPL, #GP(selector)
		if(	static_cast<Dword>((newCs & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
					DESCRIPTOR_FLAGS_DPL_SHIFT))
		{
			registers->exception = EXCEPTION_GENERAL_PROTECTION;
			registers->errorCode = newCs;
			return;
		}

		//If the descriptor is not present, #NP(selector)
		if((descriptorHigh & DESCRIPTOR_FLAGS_PRESENT_BIT) == 0)
		{
			registers->exception = EXCEPTION_SEGMENT_NOT_PRESENT;
			registers->errorCode = newCs;
			return;
		}

		//If this segment is of a lower (numerically higher) privilege level than the current,
		//a stack change is required
		if(	(descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >
			(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK))
		{
			//Add the offset to the old stack pointer
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
				registers->r_esp += stackPointerOffset;
			else
				registers->r_sp += stackPointerOffset;

			Word newSs;
			Dword newEsp;

			if((opcode & OPCODE_O32) == 0)
			{
				Word newSp;

				if(!PopWord(newSp) || !PopWord(newSs))
					return;
				
				newEsp = newSp;
			}
			else
			{
				Dword tempSs;

				if(!PopDword(newEsp) || !PopDword(tempSs))
					return;

				newSs = static_cast<Word>(tempSs);
			}

			//Set the new ss:esp values
			if(!SetDataSegmentRegisterValue(&r_ss, newSs))
				return;

			registers->r_esp = newEsp;
		}

		//Set the new cs:eip and eflags values
		if(!SetCSValue(newCs))
			return;
	
		registers->r_eip = newEip;

		if(iret)
		{
			Dword systemFlagsRetainMask = 0x00000000;
			Dword systemFlagsModifyMask = EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;

			//If CPL > IOPL, IF retained
			if(initialCpl > ((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >>
				EFLAGS_IOPL_SHIFT))
			{
				systemFlagsRetainMask |= EFLAGS_I_FLAG;
				systemFlagsModifyMask &= ~EFLAGS_I_FLAG;
			}

			//If CPL != 0, VM and IOPL retained
			if(initialCpl != 0)
			{
				systemFlagsRetainMask |= (EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
				systemFlagsModifyMask &= ~(EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
			}

			registers->r_eflags = newEflags & EFLAGS_STATUS_FLAGS_MASK;
			
			registers->r_systemFlags &= systemFlagsRetainMask;
			registers->r_systemFlags |= newEflags & systemFlagsModifyMask;
		}
	}

	//Add the offset to the stack pointer
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += stackPointerOffset;
	else
		registers->r_sp += stackPointerOffset;
}

void VMachine::RETF(Dword opcode, PrefixBitset prefixes)
{
	DoFarReturn(opcode, false, 0);
}

void VMachine::RETF_Iw(Dword opcode, PrefixBitset prefixes)
{
	Word offset;

	if(!ReadWord(r_cs, registers->r_eip, offset))
		return;

	DoFarReturn(opcode, false, offset);
}

void VMachine::IRET(Dword opcode, PrefixBitset prefixes)
{
	//Is this a protected mode task IRET?
	if(	(registers->r_cr0 & CR0_PE_FLAG) != 0 &&
		(registers->r_systemFlags & EFLAGS_VM_FLAG) == 0 &&
		(registers->r_systemFlags & EFLAGS_NT_FLAG) != 0)
	{
		//If so, read the previous task TSS selector from the current TSS
		Word previousTssSelector;

		if(!ReadWordLinear(registers->r_tr.base, previousTssSelector))
			return;

		//Get the base of the descriptor table
		Dword tableBase = registers->r_gdtr.base;

		if((previousTssSelector & SELECTOR_TI_BIT) != 0)
			tableBase = registers->r_ldtr.base;

		//Read the previous TSS descriptor
		Dword descriptorLow, descriptorHigh;

		if(	!ReadDwordLinear(tableBase + (previousTssSelector & SELECTOR_INDEX_MASK), descriptorLow) ||
			!ReadDwordLinear(tableBase + (previousTssSelector & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
		{
			return;
		}

		//Switch to the previous task
		SwitchTasks(SWITCH_TASKS_IRET, previousTssSelector, descriptorLow, descriptorHigh);
	}
	else
	{
		DoFarReturn(opcode, true, 0);
	}
}
