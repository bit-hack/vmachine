//	VMachine
//	Switch tasks
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::SwitchTasks(	SwitchTasksType type, Word selector, Dword descriptorLow,
							Dword descriptorHigh)
{
	//If this task switch was initiated by a jump or iret,
	//clear the busy flag in the old TSS descriptor
	if(type == SWITCH_TASKS_JUMP || type == SWITCH_TASKS_IRET)
	{
		Dword oldDescriptorHigh;

		if(!ReadDwordLinear(registers->r_gdtr.base +
								(registers->r_tr.selector & SELECTOR_INDEX_MASK) + 4,
									oldDescriptorHigh, true))
		{
			throw Ex("CPU Error: Task Switch");
		}
	
		if(!WriteDwordLinear(	registers->r_gdtr.base +
									(registers->r_tr.selector & SELECTOR_INDEX_MASK) + 4,
										oldDescriptorHigh & ~TSS_DESCRIPTOR_FLAGS_BUSY_BIT,
											true))
		{
			throw Ex("CPU Error: Task Switch");
		}
	}

	//If the task switch was initiated by an iret,
	//clear the NT flag in a temporary copy of eflags
	Dword tempEflags =	(registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK) | 
						registers->r_systemFlags;

	if(type == SWITCH_TASKS_IRET)
		tempEflags &= ~EFLAGS_NT_FLAG;

	//Save the processor state into the old TSS
	if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_TSS_16_BIT_BUSY)
	{
		if(	!WriteWordLinear(registers->r_tr.base + 14, registers->r_ip, true) ||
			!WriteWordLinear(registers->r_tr.base + 16, static_cast<Word>(tempEflags), true) ||
			!WriteWordLinear(registers->r_tr.base + 18, registers->r_ax, true) ||
			!WriteWordLinear(registers->r_tr.base + 20, registers->r_cx, true) ||
			!WriteWordLinear(registers->r_tr.base + 22, registers->r_dx, true) ||
			!WriteWordLinear(registers->r_tr.base + 24, registers->r_bx, true) ||
			!WriteWordLinear(registers->r_tr.base + 26, registers->r_sp, true) ||
			!WriteWordLinear(registers->r_tr.base + 28, registers->r_bp, true) ||
			!WriteWordLinear(registers->r_tr.base + 30, registers->r_si, true) ||
			!WriteWordLinear(registers->r_tr.base + 32, registers->r_di, true) ||
			!WriteWordLinear(registers->r_tr.base + 34, registers->r_es.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 36, registers->r_cs.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 38, registers->r_ss.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 40, registers->r_ds.selector, true))
		{
			throw Ex("CPU Error: Task Switch");
		}
	}
	else if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
														DESCRIPTOR_FLAGS_TYPE_TSS_32_BIT_BUSY)
	{
		if(	!WriteDwordLinear(registers->r_tr.base + 32, registers->r_eip, true) ||
			!WriteDwordLinear(registers->r_tr.base + 36, tempEflags, true) ||
			!WriteDwordLinear(registers->r_tr.base + 40, registers->r_eax, true) ||
			!WriteDwordLinear(registers->r_tr.base + 44, registers->r_ecx, true) ||
			!WriteDwordLinear(registers->r_tr.base + 48, registers->r_edx, true) ||
			!WriteDwordLinear(registers->r_tr.base + 52, registers->r_ebx, true) ||
			!WriteDwordLinear(registers->r_tr.base + 56, registers->r_esp, true) ||
			!WriteDwordLinear(registers->r_tr.base + 60, registers->r_ebp, true) ||
			!WriteDwordLinear(registers->r_tr.base + 64, registers->r_esi, true) ||
			!WriteDwordLinear(registers->r_tr.base + 68, registers->r_edi, true) ||
			!WriteWordLinear(registers->r_tr.base + 72, registers->r_es.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 76, registers->r_cs.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 80, registers->r_ss.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 84, registers->r_ds.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 88, registers->r_fs.selector, true) ||
			!WriteWordLinear(registers->r_tr.base + 92, registers->r_gs.selector, true))
		{
			throw Ex("CPU Error: Task Switch");
		}
	}
	else
		throw Ex("CPU Error: Unexpected task register type in SwitchTasks [Write]");

	//If this task switch was initiated by a call, jump or interrupt,
	//set the busy flag in the new TSS descriptor
	if(type == SWITCH_TASKS_CALL || type == SWITCH_TASKS_JUMP || type == SWITCH_TASKS_INT)
	{
		descriptorHigh |= TSS_DESCRIPTOR_FLAGS_BUSY_BIT;

		if(!WriteDwordLinear(registers->r_gdtr.base +
								(selector & SELECTOR_INDEX_MASK) + 4,
									descriptorHigh, true))
		{
			throw Ex("CPU Error: Task Switch");
		}
	}
	
	//Load the task register with the new TSS
	Word oldTrSelector = registers->r_tr.selector;

    registers->r_tr.selector = selector;
	registers->r_tr.base = GetDescriptorBase(descriptorLow, descriptorHigh);
	registers->r_tr.limit = GetDescriptorLimit(descriptorLow, descriptorHigh);
	registers->r_tr.descriptorFlags = GetDescriptorFlags(descriptorHigh);

	//Load the processor state from the new TSS
	Dword newCr3;
	Word newCs, newDs, newEs, newFs, newGs, newSs, newLdtr;
	bool newTss32Bit = false;

	if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_TSS_16_BIT_BUSY)
	{
		Word newIp, newFlags;

		if(	!ReadWordLinear(registers->r_tr.base + 14, newIp, true) ||
			!ReadWordLinear(registers->r_tr.base + 16, newFlags, true) ||
			!ReadWordLinear(registers->r_tr.base + 18, registers->r_ax, true) ||
			!ReadWordLinear(registers->r_tr.base + 20, registers->r_cx, true) ||
			!ReadWordLinear(registers->r_tr.base + 22, registers->r_dx, true) ||
			!ReadWordLinear(registers->r_tr.base + 24, registers->r_bx, true) ||
			!ReadWordLinear(registers->r_tr.base + 26, registers->r_sp, true) ||
			!ReadWordLinear(registers->r_tr.base + 28, registers->r_bp, true) ||
			!ReadWordLinear(registers->r_tr.base + 30, registers->r_si, true) ||
			!ReadWordLinear(registers->r_tr.base + 32, registers->r_di, true) ||
			!ReadWordLinear(registers->r_tr.base + 34, newEs, true) ||
			!ReadWordLinear(registers->r_tr.base + 36, newCs, true) ||
			!ReadWordLinear(registers->r_tr.base + 38, newSs, true) ||
			!ReadWordLinear(registers->r_tr.base + 40, newDs, true) ||
			!ReadWordLinear(registers->r_tr.base + 42, newLdtr, true))
		{
			throw Ex("CPU Error: Task Switch");
		}

		registers->r_eip = newIp;

		registers->r_eflags &= 0xffff0000 | ~EFLAGS_STATUS_FLAGS_MASK;
		registers->r_eflags |= newFlags & EFLAGS_STATUS_FLAGS_MASK;
		registers->r_systemFlags &= 0xffff0000;
		registers->r_systemFlags |= newFlags & EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;
	}
	else if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
													DESCRIPTOR_FLAGS_TYPE_TSS_32_BIT_BUSY)
	{
		newTss32Bit = true;

		Dword newEflags;

		if(	!ReadDwordLinear(registers->r_tr.base + 28, newCr3, true) ||
			!ReadDwordLinear(registers->r_tr.base + 32, registers->r_eip, true) ||
			!ReadDwordLinear(registers->r_tr.base + 36, newEflags, true) ||
			!ReadDwordLinear(registers->r_tr.base + 40, registers->r_eax, true) ||
			!ReadDwordLinear(registers->r_tr.base + 44, registers->r_ecx, true) ||
			!ReadDwordLinear(registers->r_tr.base + 48, registers->r_edx, true) ||
			!ReadDwordLinear(registers->r_tr.base + 52, registers->r_ebx, true) ||
			!ReadDwordLinear(registers->r_tr.base + 56, registers->r_esp, true) ||
			!ReadDwordLinear(registers->r_tr.base + 60, registers->r_ebp, true) ||
			!ReadDwordLinear(registers->r_tr.base + 64, registers->r_esi, true) ||
			!ReadDwordLinear(registers->r_tr.base + 68, registers->r_edi, true) ||
			!ReadWordLinear(registers->r_tr.base + 72, newEs, true) ||
			!ReadWordLinear(registers->r_tr.base + 76, newCs, true) ||
			!ReadWordLinear(registers->r_tr.base + 80, newSs, true) ||
			!ReadWordLinear(registers->r_tr.base + 84, newDs, true) ||
			!ReadWordLinear(registers->r_tr.base + 88, newFs, true) ||
			!ReadWordLinear(registers->r_tr.base + 92, newGs, true) ||
			!ReadWordLinear(registers->r_tr.base + 96, newLdtr, true))
		{
			throw Ex("CPU Error: Task Switch");
		}

		registers->r_eflags = newEflags & EFLAGS_STATUS_FLAGS_MASK;
		registers->r_systemFlags = newEflags & EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;
	}
	else
		throw Ex("CPU Error: Unexpected task register type in SwitchTasks [Read]");

	//If this task switch was initiated by a call or interrupt,
	//save the old TSS selector into the new TSS and set the NT flag in eflags
	if(type == SWITCH_TASKS_CALL || type == SWITCH_TASKS_INT)
	{
		if(!WriteWordLinear(registers->r_tr.base, oldTrSelector, true))
			throw Ex("CPU Error: Task Switch");

		registers->r_systemFlags |= EFLAGS_NT_FLAG;
	}

	//Set the new cr3 value
	if(newTss32Bit)
		SetCR3Value(newCr3);

	//Set the new ldtr value
	registers->r_ldtr.selector = newLdtr;

	Dword ldtrDescriptorLow, ldtrDescriptorHigh;

	if(	!ReadDwordLinear(registers->r_gdtr.base + (newLdtr & SELECTOR_INDEX_MASK), ldtrDescriptorLow, true) ||
		!ReadDwordLinear(registers->r_gdtr.base + (newLdtr & SELECTOR_INDEX_MASK) + 4, ldtrDescriptorHigh, true))
	{
		throw Ex("CPU Error: Task Switch");
	}

	registers->r_ldtr.selector = newLdtr;
	registers->r_ldtr.base = GetDescriptorBase(ldtrDescriptorLow, ldtrDescriptorHigh);
	registers->r_ldtr.limit = GetDescriptorLimit(ldtrDescriptorLow, ldtrDescriptorHigh);
	registers->r_ldtr.descriptorFlags = GetDescriptorFlags(ldtrDescriptorHigh);

	//Set the new segment register values
	if(	!SetCSValue(newCs) ||
		!SetDataSegmentRegisterValue(&r_ds, newDs) ||
		!SetDataSegmentRegisterValue(&r_es, newEs) ||
		!SetDataSegmentRegisterValue(&r_ss, newSs))
	{
		throw Ex("CPU Error: Task Switch");
	}

	if(newTss32Bit)
	{
		if(	!SetDataSegmentRegisterValue(&r_fs, newFs) ||
			!SetDataSegmentRegisterValue(&r_gs, newGs))
		{
			throw Ex("CPU Error: Task Switch");
		}
	}
}
