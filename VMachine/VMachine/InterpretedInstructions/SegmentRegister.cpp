//	VMachine
//	Segment register instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"

void VMachine::POP_SS_O16(PrefixBitset prefixes)
{
	Word newSs;

	if(!PopWord(newSs))
		return;

	SetSSValue(newSs);
}

void VMachine::POP_SS_O32(PrefixBitset prefixes)
{
	Dword newSs;

	if(!PopDword(newSs))
		return;

	SetSSValue(static_cast<Word>(newSs));
}

void VMachine::MOV_Sw_Mw(Dword opcode, PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	SegmentRegister * dataSegReg;
	SegmentRegister * defaultSegReg;
	Dword offset;

	if(!DecodeModRMByteSegRegMemoryWord(registers->r_eip, opcode, dataSegReg, defaultSegReg, offset))
		return;

	//Get the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//Read the data
	Word selector;
	if(!ReadWord(*segReg, offset, selector))
		return;

	//This interpretive implementation is only used to set SS
	assert(dataSegReg == &r_ss);
	
	SetSSValue(selector);
}

void VMachine::MOV_Sw_Rw(PrefixBitset prefixes)
{
	//Decode the ModR/M byte
	SegmentRegister * dataSegReg;
	Word * reg;

	if(!DecodeModRMByteSegRegRegWord(registers->r_eip, dataSegReg, reg))
		return;

	//This interpretive implementation is only used to set SS
	assert(dataSegReg == &r_ss);
	
	SetSSValue(*reg);
}

void VMachine::LSS_Gw_Mp(Dword opcode, PrefixBitset prefixes)
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

	//Read the data
	Word regValue;
	if(!ReadWord(*segReg, offset, regValue))
		return;

	Word selector;
	if(!ReadWord(*segReg, offset + 2, selector))
		return;

	SetSSValue(selector);
	*reg1 = regValue;
}

void VMachine::LSS_Gd_Mp(Dword opcode, PrefixBitset prefixes)
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

	//Read the data
	Dword regValue;
	if(!ReadDword(*segReg, offset, regValue))
		return;

	Word selector;
	if(!ReadWord(*segReg, offset + 4, selector))
		return;

	SetSSValue(selector);
	*reg1 = regValue;
}
