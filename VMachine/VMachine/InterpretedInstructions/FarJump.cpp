//	VMachine
//	Far jump instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::DoFarJump(Word newCs, Dword newEip)
{
	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real mode / V8086 mode
		//Set the new cs:eip value
		if(!SetCSValue(newCs))
			return;

		registers->r_eip = newEip;
	}
	else
	{
		//Protected mode
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

		//If this is a code segment
		if(IsCodeSegment(descriptorHigh))
		{
			//If RPL > CPL or DPL != CPL, #GP(selector)
			if(	static_cast<Dword>((newCs & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
					((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) ||
				((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
					DESCRIPTOR_FLAGS_DPL_SHIFT) !=
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

			//Set the new cs:eip value
			if(!SetCSValue(newCs))
				return;

			registers->r_eip = newEip;
		}

		//If this is an available 32-bit TSS
		if((descriptorHigh & DESCRIPTOR_FLAGS_TYPE_MASK) == 
													DESCRIPTOR_FLAGS_TYPE_TSS_32_BIT_AVAIL)
		{
			//Switch to the new task
			SwitchTasks(SWITCH_TASKS_JUMP, newCs, descriptorLow, descriptorHigh);   
		}
	}
}


void VMachine::JMP_FAR_Ap(Dword opcode, PrefixBitset prefixes)
{
	//Get the destination
	Word newCs;
	Dword newEip;

	if((opcode & OPCODE_O32) == 0)
	{
		Word newIp;

		if(	!ReadWord(r_cs, registers->r_eip, newIp) ||
			!ReadWord(r_cs, registers->r_eip + 2, newCs))
		{
			return;
		}
		
		registers->r_eip += 4;	//Some jumps save a return address!

		newEip = newIp;
	}
	else
	{
		if(	!ReadDword(r_cs, registers->r_eip, newEip) ||
			!ReadWord(r_cs, registers->r_eip + 4, newCs))
		{
			return;
		}

		registers->r_eip += 6;
	}

	DoFarJump(newCs, newEip);
}

void VMachine::JMP_FAR_Mp(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryWord(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the destination
	Word newCs;
	Dword newEip;

	if((opcode & OPCODE_O32) == 0)
	{
		Word newIp;

		if(!ReadWord(*segReg, offset, newIp) || !ReadWord(*segReg, offset + 2, newCs))
			return;
			
		newEip = newIp;
	}
	else
	{
		if(!ReadDword(*segReg, offset, newEip) || !ReadWord(*segReg, offset + 4, newCs))
			return;
	}

	DoFarJump(newCs, newEip);
}
