//	VMachine
//	Functions implementing interpreted input/output instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::IN_AL_Ib(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If the I/O permission bit for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 1))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_al = InputByte(address);
	
	SwitchToVMachineFpuContext();
}

void VMachine::IN_AL_DX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If the I/O permission bit for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 1))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_al = InputByte(registers->r_dx);
	
	SwitchToVMachineFpuContext();
}

void VMachine::IN_AX_Ib(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 2))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_ax = InputWord(address);
	
	SwitchToVMachineFpuContext();
}

void VMachine::IN_AX_DX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();
	
	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 2))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_ax = InputWord(registers->r_dx);
	
	SwitchToVMachineFpuContext();
}

void VMachine::IN_EAX_Ib(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 4))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_eax = InputDword(address);

	SwitchToVMachineFpuContext();
}

void VMachine::IN_EAX_DX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();
	
	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 4))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	registers->r_eax = InputDword(registers->r_dx);
	
	SwitchToVMachineFpuContext();
}

void VMachine::OUT_Ib_AL(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If the I/O permission bit for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 1))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputByte(address, registers->r_al);
	
	SwitchToVMachineFpuContext();
}

void VMachine::OUT_DX_AL(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If the I/O permission bit for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 1))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputByte(registers->r_dx, registers->r_al);
	
	SwitchToVMachineFpuContext();
}

void VMachine::OUT_Ib_AX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 2))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputWord(address, registers->r_ax);
	
	SwitchToVMachineFpuContext();
}

void VMachine::OUT_DX_AX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();
	
	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 2))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputWord(registers->r_dx, registers->r_ax);

	SwitchToVMachineFpuContext();
}

void VMachine::OUT_Ib_EAX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();

	Byte address;
	if(!ReadByte(r_cs, registers->r_eip++, address))
		return;

	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(address, 4))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputDword(address, registers->r_eax);
	
	SwitchToVMachineFpuContext();
}

void VMachine::OUT_DX_EAX(PrefixBitset prefixes)
{
	SwitchToHostFpuContext();
	
	//If protected mode with IOPL < CPL, or V8086 mode
	if((registers->r_cr0 & CR0_PE_FLAG) != 0)
	{
		if(	((registers->r_systemFlags & EFLAGS_IOPL_FLAG) >> EFLAGS_IOPL_SHIFT) <
				((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK)
					>> DESCRIPTOR_FLAGS_DPL_SHIFT) ||
			(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
		{
			//If any of the I/O permission bits for this I/O port is 1, #GP(0)
			if(!IsIoPermittedByBitmap(registers->r_dx, 4))
			{
				registers->exception = EXCEPTION_GENERAL_PROTECTION;
				registers->errorCode = 0;
				return;
			}
		}
	}

	OutputDword(registers->r_dx, registers->r_eax);
	
	SwitchToVMachineFpuContext();
}
