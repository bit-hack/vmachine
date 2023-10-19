//	VMachine
//	Int instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::DoInt(Byte intNum, IntType intType, Dword errorCode)
{
	if((registers->r_cr0 & CR0_PE_FLAG) == 0)
	{
		//Real mode
		//Save the return flags and address
		if(!PushWord(static_cast<Word>(registers->r_systemFlags | (registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK))))
			return;

		//TODO: Handle exceptions
		if(intType == INT_TYPE_EXCEPTION)
		{
			if(	!PushWord(registers->r_cs.selector) ||
				!PushWord(static_cast<Word>(instrStartEip)))
			{
				return;
			}
		}
		else
		{
			if(!PushWord(registers->r_cs.selector) || !PushWord(registers->r_ip))
				return;
		}
	
		//Clear the I and T flags
		registers->r_systemFlags &= ~(EFLAGS_I_FLAG | EFLAGS_T_FLAG);
	
		//Get the destination
		Word newCs, newIp;
		if(!ReadWordLinear(intNum * 4, newIp) || !ReadWordLinear(intNum * 4 + 2, newCs))
			return;

		//Set the new cs:eip
		if(!SetCSValue(newCs))
			return;

		registers->r_eip = newIp;
	}
	else
	{
		//Protected mode
		//Read the gate
		Dword gateLow, gateHigh;

		if(	!ReadDwordLinear(registers->r_idtr.base + intNum * 8, gateLow, true) ||
			!ReadDwordLinear(registers->r_idtr.base + intNum * 8 + 4, gateHigh, true))
		{
			return;
		}

		//If this is a software interrupt, and the gate DPL < CPL, #GP((intNum * 8) | IDT)
		if(	intType == INT_TYPE_SOFTWARE &&
			((gateHigh & DESCRIPTOR_FLAGS_DPL_MASK) >> DESCRIPTOR_FLAGS_DPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
					DESCRIPTOR_FLAGS_DPL_SHIFT))
		{
			registers->exception = EXCEPTION_GENERAL_PROTECTION;
			registers->errorCode = (intNum * 8) | ERROR_CODE_IDT_BIT;
			return;
		}

		//If gate not present, #NP((intNum * 8) | IDT | [EXT])
		if((gateHigh & DESCRIPTOR_FLAGS_PRESENT_BIT) == 0)
		{
			registers->exception = EXCEPTION_SEGMENT_NOT_PRESENT;
			registers->errorCode = (intNum * 8) | ERROR_CODE_IDT_BIT;
			if(intType == INT_TYPE_HARDWARE)
				errorCode |= ERROR_CODE_EXT_BIT;
			return;
		}

		//If this is an interrupt or trap gate
		if(	(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_INT_GATE_16_BIT ||
			(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_TRAP_GATE_16_BIT ||
			(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_INT_GATE_32_BIT ||
			(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_TRAP_GATE_32_BIT)
		{
			bool gate32Bit =	(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_INT_GATE_32_BIT ||
								(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_TRAP_GATE_32_BIT;

			//Calculate the new cs:eip
			Word newCs = static_cast<Word>(gateLow >> 16);
			Dword newEip = (gateLow & 0x0000ffff);

			if(gate32Bit)
			{
				newEip |= gateHigh & 0xffff0000;
			}

			//Get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((newCs & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the segment descriptor
			Dword descriptorLow, descriptorHigh;

			if(	!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK), descriptorLow, true) ||
				!ReadDwordLinear(tableBase + (newCs & SELECTOR_INDEX_MASK) + 4, descriptorHigh, true))
			{
				return;
			}

			//Save the current eflags value
			Dword tempEflags = registers->r_systemFlags | 
									(registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK);

			//Clear the VM, RF, NT and T flags
			registers->r_systemFlags &=
				~(EFLAGS_VM_FLAG | EFLAGS_RF_FLAG | EFLAGS_NT_FLAG | EFLAGS_T_FLAG);

			//If this segment is of a higher (numerically lower) privilege level than the
			//current, or this is an interrupt from V8086 mode,
			//a stack change is required
			if(	((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) <
				(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)) ||
				(tempEflags & EFLAGS_VM_FLAG) != 0)
			{
                Word newSs;
				Dword newEsp;

				//Read the new ss:esp from the TSS
				if(!ReadSsEspFromTss((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
										DESCRIPTOR_FLAGS_DPL_SHIFT, newSs, newEsp))
				{
					return;
				}

				Word oldSs = registers->r_ss.selector;
				Dword oldEsp =
					(intType == INT_TYPE_EXCEPTION) ? instrStartEsp : registers->r_esp;

				//Set the new ss:esp values
				if(!SetDataSegmentRegisterValue(&r_ss, newSs))
					return;

				registers->r_esp = newEsp;

				//If this is an interrupt from V8086 mode
				if((tempEflags & EFLAGS_VM_FLAG) != 0)
				{
					//Push the data segment register selectors
					if(gate32Bit)
					{
						if(	!PushDword(registers->r_gs.selector) ||
							!PushDword(registers->r_fs.selector) ||
							!PushDword(registers->r_ds.selector) ||
							!PushDword(registers->r_es.selector))
						{
							return;
						}
					}
					else
					{
						if(	!PushWord(registers->r_gs.selector) ||
							!PushWord(registers->r_fs.selector) ||
							!PushWord(registers->r_ds.selector) ||
							!PushWord(registers->r_es.selector))
						{
							return;
						}
					}

					//Nullify the data segment registers
                    registers->r_ds.selector = 0;
					registers->r_ds.base = registers->r_ds.limit = 0;
					registers->r_ds.descriptorFlags = 0;
					registers->r_es.selector = 0;
					registers->r_es.base = registers->r_es.limit = 0;
					registers->r_es.descriptorFlags = 0;
					registers->r_fs.selector = 0;
					registers->r_fs.base = registers->r_fs.limit = 0;
					registers->r_fs.descriptorFlags = 0;
					registers->r_gs.selector = 0;
					registers->r_gs.base = registers->r_gs.limit = 0;
					registers->r_gs.descriptorFlags = 0;
				}

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
			}

			//Save the return flags and address
			if(gate32Bit)
			{
				if(!PushDword(tempEflags))
					return;

				if(intType == INT_TYPE_EXCEPTION)
				{
					if(!PushDword(registers->r_cs.selector) || !PushDword(instrStartEip))
						return;
				}
				else
				{
					if(!PushDword(registers->r_cs.selector) || !PushDword(registers->r_eip))
						return;
				}

				//Push an error code if applicable
				if(	intType == INT_TYPE_EXCEPTION &&
					(intNum == 8 || intNum == 10 || intNum == 11 || intNum == 12 ||
					intNum == 13 || intNum == 14 || intNum == 17))
				{
					if(!PushDword(errorCode))
						return;
				}
			}
			else
			{
				if(!PushWord(static_cast<Word>(tempEflags)))
				{
					return;
				}

				if(intType == INT_TYPE_EXCEPTION)
				{
					if(	!PushWord(registers->r_cs.selector) ||
						!PushWord(static_cast<Word>(instrStartEip)))
					{
						return;
					}
				}
				else
				{
					if(!PushWord(registers->r_cs.selector) || !PushWord(registers->r_ip))
						return;
				}

				//Push an error code if applicable
				if(	intType == INT_TYPE_EXCEPTION &&
					(intNum == 8 || intNum == 10 || intNum == 11 || intNum == 12 ||
					intNum == 13 || intNum == 14 || intNum == 17))
				{
					if(!PushWord(static_cast<Word>(errorCode)))
						return;
				}
			}

			//Set the new cs:eip values
			if(!SetCSValue(newCs))
				return;
	
			registers->r_eip = newEip;
			
			//If this is an interrupt gate, clear the I flag
			if(	(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_INT_GATE_16_BIT ||
				(gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_INT_GATE_32_BIT)
			{
				registers->r_systemFlags &= ~EFLAGS_I_FLAG;
			}
		}

		//Otherwise, if this is a task gate
		else if((gateHigh & DESCRIPTOR_FLAGS_TYPE_MASK) ==
											DESCRIPTOR_FLAGS_TYPE_TASK_GATE)
		{
			//Get the TSS selector from the gate
			Word tssSelector = static_cast<Word>(gateLow >> 16);

			//Get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((tssSelector & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the TSS descriptor
			Dword descriptorLow, descriptorHigh;

			if(	!ReadDwordLinear(tableBase + (tssSelector & SELECTOR_INDEX_MASK), descriptorLow, true) ||
				!ReadDwordLinear(tableBase + (tssSelector & SELECTOR_INDEX_MASK) + 4, descriptorHigh, true))
			{
				return;
			}

			//Switch to the new task
			SwitchTasks(SWITCH_TASKS_INT, tssSelector, descriptorLow, descriptorHigh);

			//Push an error code if applicable
			if(	intType == INT_TYPE_EXCEPTION &&
				(intNum == 8 || intNum == 10 || intNum == 11 || intNum == 12 ||
				intNum == 13 || intNum == 14 || intNum == 17))
			{
				if(!PushDword(errorCode))
					return;
			}
		}
	}
}

void VMachine::INT_Ib(PrefixBitset prefixes)
{
	//Get the interrupt number
    Byte intNum;
	
	if(!ReadByte(r_cs, registers->r_eip++, intNum))
		return;

	DoInt(intNum, INT_TYPE_SOFTWARE, 0);
}
