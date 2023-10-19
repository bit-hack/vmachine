//	VMachine
//	Functions implementing interpreted string instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::INS_Yb_DX_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Byte data = InputByte(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteByte(r_es, registers->r_di, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_di += 1;
	else
		registers->r_di -= 1;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::INS_Yb_DX_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Byte data = InputByte(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteByte(r_es, registers->r_edi, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_edi += 1;
	else
		registers->r_edi -= 1;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::INS_Yw_DX_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Word data = InputWord(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteWord(r_es, registers->r_di, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_di += 2;
	else
		registers->r_di -= 2;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::INS_Yw_DX_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Word data = InputWord(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteWord(r_es, registers->r_edi, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_edi += 2;
	else
		registers->r_edi -= 2;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::INS_Yd_DX_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Dword data = InputDword(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteDword(r_es, registers->r_di, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_di += 4;
	else
		registers->r_di -= 4;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::INS_Yd_DX_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Perform the input
	SwitchToHostFpuContext();
	Dword data = InputDword(registers->r_dx);
	SwitchToVMachineFpuContext();

	//Write the data
	if(!WriteDword(r_es, registers->r_edi, data))
		return;

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_edi += 4;
	else
		registers->r_edi -= 4;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xb_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Byte data;

	if(!ReadByte(*segReg, registers->r_si, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputByte(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_si += 1;
	else
		registers->r_si -= 1;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xb_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Byte data;

	if(!ReadByte(*segReg, registers->r_esi, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputByte(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_esi += 1;
	else
		registers->r_esi -= 1;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xw_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Word data;

	if(!ReadWord(*segReg, registers->r_si, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputWord(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_si += 2;
	else
		registers->r_si -= 2;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xw_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Word data;

	if(!ReadWord(*segReg, registers->r_esi, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputWord(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_esi += 2;
	else
		registers->r_esi -= 2;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xd_A16(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_cx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Dword data;

	if(!ReadDword(*segReg, registers->r_si, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputDword(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_si += 4;
	else
		registers->r_si -= 4;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_cx != 0)
			registers->r_eip = instrStartEip;
	}
}

void VMachine::OUTS_DX_Xd_A32(PrefixBitset prefixes)
{
	//If a repeat prefix is present and the count register is zero, no operation
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 && registers->r_ecx == 0)
		return;

	//Calculate which segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = &r_ds;

	//Read the data
	Dword data;

	if(!ReadDword(*segReg, registers->r_esi, data))
		return;

	//Perform the output
	SwitchToHostFpuContext();
	OutputDword(registers->r_dx, data);
	SwitchToVMachineFpuContext();

	//Update registers
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)
		registers->r_esi += 4;
	else
		registers->r_esi -= 4;

	//Repeat if necessary
	if((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 )
	{
		if(--registers->r_ecx != 0)
			registers->r_eip = instrStartEip;
	}
}
