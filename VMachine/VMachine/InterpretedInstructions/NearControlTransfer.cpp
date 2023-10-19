//	VMachine
//	Near control transfer instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::JMP_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Jump to the destination
	registers->r_eip = *reg2;
}

void VMachine::JMP_Mw(Dword opcode, PrefixBitset prefixes)
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

	//Get the new ip
	Word newIp;

	if(!ReadWord(*segReg, offset, newIp))
		return;

	registers->r_eip = newIp;
}

void VMachine::JMP_Rd(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	//Jump to the destination
	registers->r_eip = *reg2;
}

void VMachine::JMP_Md(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryDword(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new eip
	Dword newEip;

	if(!ReadDword(*segReg, offset, newEip))
		return;

	registers->r_eip = newEip;
}

void VMachine::CALL_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Word * reg1;
	Word * reg2;

	if(!DecodeModRMByteRegRegWord(registers->r_eip, reg1, reg2))
		return;

	//Save the return address
	if(!PushWord(registers->r_ip))
		return;

	//Jump to the destination
	registers->r_eip = *reg2;
}

void VMachine::CALL_Mw(Dword opcode, PrefixBitset prefixes)
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

	//Get the new ip
	Word newIp;

	if(!ReadWord(*segReg, offset, newIp))
		return;

	//Save the return address
	if(!PushWord(registers->r_ip))
		return;

	//Jump to the destination
	registers->r_eip = newIp;
}

void VMachine::CALL_Rd(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	Dword * reg2;

	if(!DecodeModRMByteRegRegDword(registers->r_eip, reg1, reg2))
		return;

	//Save the return address
	if(!PushDword(registers->r_eip))
		return;

	//Jump to the destination
	registers->r_eip = *reg2;
}

void VMachine::CALL_Md(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	Dword * reg1;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteRegMemoryDword(registers->r_eip, opcode, reg1, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Get the new eip
	Dword newEip;

	if(!ReadDword(*segReg, offset, newEip))
		return;

	//Save the return address
	if(!PushDword(registers->r_eip))
		return;

	//Jump to the destination
	registers->r_eip = newEip;
}
