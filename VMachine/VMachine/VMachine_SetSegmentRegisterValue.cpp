//	VMachine
//	Set the value of a segment register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

bool VMachine::SetCSValue(Word selector)
{
	if((registers->r_cr0 & CR0_PE_FLAG) == 0)
	{
		//Real mode
		r_cs.data.selector = selector;
		r_cs.data.base = (Dword)selector << 4;
	}
	else if((registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//V8086 mode
		r_cs.data.selector = selector;
		r_cs.data.base = (Dword)selector << 4;
		r_cs.data.limit = 0xffff;
		r_cs.data.descriptorFlags = 0x0000f300;
	}
	else
	{
		//Protected mode

		//Is the selector NULL?
		if(selector < 3)
		{
			//If so, zero all components of the segment register data
			r_cs.data.selector = 0;
			r_cs.data.base = 0;
			r_cs.data.limit = 0;
			r_cs.data.descriptorFlags = 0;
		}
		else
		{
			//Otherwise, get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((selector & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the segment descriptor
			Dword descriptorLow, descriptorHigh;

			if(	!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK), descriptorLow, true) ||
                !ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorHigh, true))
			{
				return false;
			}
			
			//Save the values
			r_cs.data.selector = selector;
			r_cs.data.base = GetDescriptorBase(descriptorLow, descriptorHigh);
			r_cs.data.limit = GetDescriptorLimit(descriptorLow, descriptorHigh);
			r_cs.data.descriptorFlags = GetDescriptorFlags(descriptorHigh);
		}
	}

	return true;
}

bool VMachine::SetDataSegmentRegisterValue(SegmentRegister * segReg, Word selector)
{
	if((registers->r_cr0 & CR0_PE_FLAG) == 0)
	{
		//Real mode
		segReg->data.selector = selector;
		segReg->data.base = (Dword)selector << 4;
	}
	else if((registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//V8086 mode
		segReg->data.selector = selector;
		segReg->data.base = (Dword)selector << 4;
		segReg->data.limit = 0xffff;
		segReg->data.descriptorFlags = 0x0000f300;
	}
	else
	{
		//Protected mode

		//Is the selector NULL?
		if(selector < 3)
		{
			//If so, zero all components of the segment register data
			segReg->data.selector = 0;
			segReg->data.base = 0;
			segReg->data.limit = 0;
			segReg->data.descriptorFlags = 0;
		}
		else
		{
			//Otherwise, get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((selector & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the segment descriptor
			Dword descriptorLow, descriptorHigh;

			if(	!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
                !ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
			{
				return false;
			}

			//If RPL > DPL and CPL > DPL, #GP(selector)
			if(	(Dword)((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) >
					((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) &&
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) >
							((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >>
								DESCRIPTOR_FLAGS_DPL_SHIFT))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = selector;

				return false;
			}

			//If the descriptor is not present, #NP(selector)
			if((descriptorHigh & DESCRIPTOR_FLAGS_PRESENT_BIT) == 0)
			{
				registers->exception = EXCEPTION_SEGMENT_NOT_PRESENT;
				registers->errorCode = selector;

				return false;
			}
			
			//Save the values
			segReg->data.selector = selector;
			segReg->data.base = GetDescriptorBase(descriptorLow, descriptorHigh);
			segReg->data.limit = GetDescriptorLimit(descriptorLow, descriptorHigh);
			segReg->data.descriptorFlags = GetDescriptorFlags(descriptorHigh);
		}
	}

	return true;
}

bool VMachine::SetSSValue(Word selector)
{
	if((registers->r_cr0 & CR0_PE_FLAG) == 0)
	{
		//Real mode
		r_ss.data.selector = selector;
		r_ss.data.base = (Dword)selector << 4;
	}
	else if((registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//V8086 mode
		r_ss.data.selector = selector;
		r_ss.data.base = (Dword)selector << 4;
		r_ss.data.limit = 0xffff;
		r_ss.data.descriptorFlags = 0x0000f300;
	}
	else
	{
		//Protected mode

		//Is the selector NULL?
		if(selector < 3)
		{
			//If so, zero all components of the segment register data
			r_ss.data.selector = 0;
			r_ss.data.base = 0;
			r_ss.data.limit = 0;
			r_ss.data.descriptorFlags = 0;
		}
		else
		{
			//Otherwise, get the base of the descriptor table
			Dword tableBase = registers->r_gdtr.base;

			if((selector & SELECTOR_TI_BIT) != 0)
				tableBase = registers->r_ldtr.base;

			//Read the segment descriptor
			Dword descriptorLow, descriptorHigh;

			if(	!ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK), descriptorLow) ||
                !ReadDwordLinear(tableBase + (selector & SELECTOR_INDEX_MASK) + 4, descriptorHigh))
			{
				return false;
			}

			//If RPL != CPL or DPL != CPL, #GP(selector)
			if(	(Dword)((selector & SELECTOR_RPL_MASK) >> SELECTOR_RPL_SHIFT) !=
					((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
						DESCRIPTOR_FLAGS_DPL_SHIFT) ||
				((descriptorHigh & DESCRIPTOR_FLAGS_DPL_MASK) >> DESCRIPTOR_FLAGS_DPL_SHIFT) !=
						((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) >>
							DESCRIPTOR_FLAGS_DPL_SHIFT))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = selector;

				return false;
			}

			//If the descriptor is not present, #SS(selector)
			if((descriptorHigh & DESCRIPTOR_FLAGS_PRESENT_BIT) == 0)
			{
				registers->exception = EXCEPTION_STACK_FAULT;
				registers->errorCode = selector;

				return false;
			}
			
			//Save the values
			r_ss.data.selector = selector;
			r_ss.data.base = GetDescriptorBase(descriptorLow, descriptorHigh);
			r_ss.data.limit = GetDescriptorLimit(descriptorLow, descriptorHigh);
			r_ss.data.descriptorFlags = GetDescriptorFlags(descriptorHigh);
		}
	}

	return true;
}
