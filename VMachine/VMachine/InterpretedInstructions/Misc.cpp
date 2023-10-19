//	VMachine
//	Misc instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::POPF_O16(PrefixBitset prefixes)
{
	Dword systemFlagsRetainMask = 0xffff0000;
	Dword systemFlagsModifyMask = EFLAGS_SYSTEM_CONTROL_FLAGS_MASK & 0x0000ffff;

	//If CPL > IOPL, IF is retained
	if(	((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
			>> DESCRIPTOR_FLAGS_DPL_SHIFT) >
				((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT))
	{
		systemFlagsRetainMask |= EFLAGS_I_FLAG;
		systemFlagsModifyMask &= ~EFLAGS_I_FLAG;
	}

	if((registers->r_systemFlags & EFLAGS_VM_FLAG) == 0)
	{
		//Not V8086 mode
		if(((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
				>> DESCRIPTOR_FLAGS_DPL_SHIFT) == 0)
		{
			//CPL == 0: All 16-bit flags may be modified
		}
		else
		{
			//CPL > 0: All 16-bit flags except IOPL may be modified
			systemFlagsRetainMask |= EFLAGS_IOPL_FLAG;
			systemFlagsModifyMask &= ~EFLAGS_IOPL_FLAG;
		}
	}
	else
	{
		//V8086 mode
		if((registers->r_systemFlags & EFLAGS_IOPL_FLAG) == EFLAGS_IOPL_FLAG)
		{
			//IOPL == 3: All 16-bit flags except IOPL may be modified
			systemFlagsRetainMask |= EFLAGS_IOPL_FLAG;
			systemFlagsModifyMask &= ~EFLAGS_IOPL_FLAG;
		}
		else
		{
			//IOPL < 3: #GP(0)
			registers->exception = EXCEPTION_GENERAL_PROTECTION;
			registers->errorCode = 0;
			return;
		}
	}

	Word newFlags;
	if(!PopWord(newFlags))
		return;

	registers->r_eflags &= 0xffff0000 | ~EFLAGS_STATUS_FLAGS_MASK;
	registers->r_eflags |= newFlags & EFLAGS_STATUS_FLAGS_MASK;
	registers->r_systemFlags &= systemFlagsRetainMask;
	registers->r_systemFlags |= (newFlags & systemFlagsModifyMask) | 0x02;
}

void VMachine::POPF_O32(PrefixBitset prefixes)
{
	Dword systemFlagsRetainMask = 0x00000000;
	Dword systemFlagsModifyMask = EFLAGS_SYSTEM_CONTROL_FLAGS_MASK;

	//If CPL > IOPL, IF is retained
	if(	((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
			>> DESCRIPTOR_FLAGS_DPL_SHIFT) >
				((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT))
	{
		systemFlagsRetainMask |= EFLAGS_I_FLAG;
		systemFlagsModifyMask &= ~EFLAGS_I_FLAG;
	}

	if((registers->r_systemFlags & EFLAGS_VM_FLAG) == 0)
	{
		//Not V8086 mode
		if(((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
				>> DESCRIPTOR_FLAGS_DPL_SHIFT) == 0)
		{
			//CPL == 0: All flags except VM may be modified
			systemFlagsRetainMask |= EFLAGS_VM_FLAG;
			systemFlagsModifyMask &= ~EFLAGS_VM_FLAG;
		}
		else
		{
			//CPL > 0: All flags except VM and IOPL may be modified
			systemFlagsRetainMask |= (EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
			systemFlagsModifyMask &= ~(EFLAGS_VM_FLAG | EFLAGS_IOPL_FLAG);
		}
	}
	else
	{
		//V8086 mode
		if((registers->r_systemFlags & EFLAGS_IOPL_FLAG) == EFLAGS_IOPL_FLAG)
		{
			//IOPL == 3: All flags except VM, RF and IOPL may be modified
			systemFlagsRetainMask |= (EFLAGS_VM_FLAG | EFLAGS_RF_FLAG | EFLAGS_IOPL_FLAG);
			systemFlagsModifyMask &= ~(EFLAGS_VM_FLAG | EFLAGS_RF_FLAG | EFLAGS_IOPL_FLAG);
		}
		else
		{
			//IOPL < 3: #GP(0)
			registers->exception = EXCEPTION_GENERAL_PROTECTION;
			registers->errorCode = 0;
			return;
		}
	}

	Dword newEflags;
	if(!PopDword(newEflags))
		return;

	registers->r_eflags &= ~EFLAGS_STATUS_FLAGS_MASK;
	registers->r_eflags |= newEflags & EFLAGS_STATUS_FLAGS_MASK;
	registers->r_systemFlags &= systemFlagsRetainMask;
	registers->r_systemFlags |= (newEflags & systemFlagsModifyMask) | 0x02;
}
