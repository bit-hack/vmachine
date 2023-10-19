//	VMachine
//	Far call instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::DoFarCall(Dword opcode, Word newCs, Dword newEip)
{
	if((registers->r_cr0 & CR0_PE_FLAG) == 0 || (registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real mode / V8086 mode

		//Push the return address
		if((opcode & OPCODE_O32) == 0)
		{
			if(!PushWord(registers->r_cs.selector) || !PushWord(registers->r_ip))
				return;
		}
		else
		{
			if(!PushDword(registers->r_cs.selector) || !PushDword(registers->r_eip))
				return;
		}

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

			//Push the return address
			if((opcode & OPCODE_O32) == 0)
			{
				if(!PushWord(registers->r_cs.selector) || !PushWord(registers->r_ip))
					return;
			}
			else
			{
				if(!PushDword(registers->r_cs.selector) || !PushDword(registers->r_eip))
					return;
			}

			//Set the new cs:eip value
			if(!SetCSValue(newCs))
				return;

			registers->r_eip = newEip;
		}

		//If this is a call gate
		else if((descriptorHigh & DESCRIPTOR_FLAGS_TYPE_MASK) == 
												DESCRIPTOR_FLAGS_TYPE_CALL_GATE_16_BIT ||
				(descriptorHigh & DESCRIPTOR_FLAGS_TYPE_MASK) == 
												DESCRIPTOR_FLAGS_TYPE_CALL_GATE_32_BIT)
		{
			bool gate32Bit =	(descriptorHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_CALL_GATE_32_BIT;

			//Calculate the new cs:eip
			newCs = static_cast<Word>(descriptorLow >> 16);
			newEip = descriptorLow & 0x0000ffff;

			if(gate32Bit)
			{
				newEip |= descriptorHigh & 0xffff0000;
			}

			//Get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((newCs & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the cs segment descriptor
			Dword csDescriptorLow, csDescriptorHigh;

			if(	!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK), csDescriptorLow) ||
				!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK) + 4, csDescriptorHigh))
			{
				return;
			}

			//If this segment is of a higher (numerically lower) privilege level than the current,
			//a stack change is required
			if(	(csDescriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) <
				(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK))
			{
				//Read the parameters from the old stack
				Dword numParameters = (descriptorHigh &
										CALL_GATE_DESCRIPTOR_HIGH_PARAMETER_COUNT_MASK) >>
											CALL_GATE_DESCRIPTOR_HIGH_PARAMETER_COUNT_SHIFT;
				Dword parameters[32];

				for(Dword i = 0; i < numParameters; ++i)
				{
					if(gate32Bit)
					{
						if(!ReadStackDword((numParameters - 1) * 4 - i * 4, parameters[i]))
						{
							return;
						}
					}
					else
					{
						Word tempWord;

						if(!ReadStackWord((numParameters - 1) * 2 - i * 2, tempWord))
						{
							return;
						}

						parameters[i] = tempWord;
					}
				}

                //Read the new ss:esp from the TSS
				Word newSs;
				Dword newEsp;

				if(!ReadSsEspFromTss((csDescriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
												DESCRIPTOR_FLAGS_DPL_SHIFT, newSs, newEsp))
				{
					return;
				}

				Word oldSs = registers->r_ss.selector;
				Dword oldEsp = registers->r_esp;

				//Set the new ss:esp values
				if(!SetDataSegmentRegisterValue(&r_ss, newSs))
					return;

				registers->r_esp = newEsp;

                //Push the old ss:esp
				if(gate32Bit)
				{
					if(!PushDword(oldSs) || !PushDword(oldEsp))
						return;
				}
				else
				{
					if(!PushWord(oldSs) || !PushWord(static_cast<Word>(oldEsp)))
						return;					
				}

				//Push the parameters
				for(Dword i = 0; i < numParameters; ++i)
				{
					if(gate32Bit)
					{
						if(!PushDword(parameters[i]))
							return;
					}
					else
					{
						if(!PushWord(static_cast<Word>(parameters[i])))
							return;
					}
				}
			}

			//Push the return address
			if(gate32Bit)
			{
				if(!PushDword(registers->r_cs.selector) || !PushDword(registers->r_eip))
					return;
			}
			else
			{
				if(!PushWord(registers->r_cs.selector) || !PushWord(registers->r_ip))
					return;
			}
			
			//Set the new cs:eip values
			if(!SetCSValue(newCs))
				return;

			registers->r_eip = newEip;
		}
	}
}

void VMachine::CALL_FAR_Ap(Dword opcode, PrefixBitset prefixes)
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
			
		registers->r_eip += 4;

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

	DoFarCall(opcode, newCs, newEip);
}

void VMachine::CALL_FAR_Mp(Dword opcode, PrefixBitset prefixes)
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

	DoFarCall(opcode, newCs, newEip);
}
