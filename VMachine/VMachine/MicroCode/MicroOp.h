//	VMachine
//	Structure for a MicroOp
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

#include "../x86Constants.h"

//MicroOp types
//* - register types & numbers used depend on operation size
//+ - may cause exception
//! - clobbers eax, ecx, edx
enum MicroOpType
{
	TYPE_NOP,			//Nop
	TYPE_GET,			//Get host, guest
	TYPE_PUT,			//Put guest, host
	TYPE_GETSEG,		//GetSeg host, guestSeg
	TYPE_PUTSEG,		//PutSeg guestSeg, host + !
	TYPE_GETEFLAGS,		//GetEflags host
	TYPE_PUTEFLAGS,		//PutEflags host
	TYPE_READ,			//Read host, [host] + !
	TYPE_WRITE,			//Write [host], host + !
	TYPE_LEA16,			//Lea16 host, [host + imm]; Lea16 host, [host + host + imm]
	TYPE_LEA32,			//Lea32 host, [host + imm]; Lea32 host, [host + (host << imm) + imm]
	TYPE_MOV,			//Mov host, host; Mov host, imm
	TYPE_CMOV,			//Cmov host, host
	TYPE_ZEROEX,		//ZeroEx host, host
	TYPE_SIGNEX,		//SignEx host, host
	TYPE_ALU,			//Alu host, host; Alu host, imm
	TYPE_UALU,			//Ualu host
	TYPE_SHIFT,			//Shift host, host; Shift host, imm
	TYPE_SHIFTD,		//Shift host, host, host; Shift host, host, imm
	TYPE_MUL,			//Mul *
	TYPE_DIV,			//Div * +
	TYPE_BIT,			//Bit host, imm; Bit host, host
	TYPE_BSWAP,			//Bswap host
	TYPE_BIT_SCAN,		//BitScan host, host
	TYPE_AS_ADJUST,		//ASAdjust host
	TYPE_MD_ADJUST,		//MDAdjust host, imm
	TYPE_ADDEIP,		//AddEip imm; AddEip host
	TYPE_ANDEIP,		//AndEip imm
	TYPE_SETEIP,		//SetEip imm; SetEip host !
	TYPE_CLD,			//Cld
	TYPE_STD,			//Std
	TYPE_DUPDATE,		//DUpdate host, imm !
	TYPE_REGISTER_FPU,	//RegisterFpu +
	TYPE_MEMORY_FPU,	//MemoryFpu +
	TYPE_WAIT			//Wait +
};

enum OperandType
{
	OPERAND_TYPE_NONE,
	OPERAND_TYPE_IMMEDIATE_8,
	OPERAND_TYPE_IMMEDIATE_16,
	OPERAND_TYPE_IMMEDIATE_32,
	OPERAND_TYPE_GUEST_REG,
	OPERAND_TYPE_GUEST_SEG_REG,
	OPERAND_TYPE_HOST_REG_L8,
	OPERAND_TYPE_HOST_REG_H8,
	OPERAND_TYPE_HOST_REG_16,
	OPERAND_TYPE_HOST_REG_32,
	OPERAND_TYPE_SCRATCH_MEMORY_16,
	OPERAND_TYPE_SCRATCH_MEMORY_32,
	OPERAND_TYPE_SCRATCH_MEMORY_64,
	OPERAND_TYPE_SCRATCH_MEMORY_80
};

typedef Byte FlagSet;

const FlagSet FLAGSET_O_FLAG = 0x20;
const FlagSet FLAGSET_S_FLAG = 0x10;
const FlagSet FLAGSET_Z_FLAG = 0x08;
const FlagSet FLAGSET_A_FLAG = 0x04;
const FlagSet FLAGSET_P_FLAG = 0x02;
const FlagSet FLAGSET_C_FLAG = 0x01;
const FlagSet FLAGSET_STATUS_FLAGS =	FLAGSET_O_FLAG | FLAGSET_S_FLAG | FLAGSET_Z_FLAG |
										FLAGSET_A_FLAG | FLAGSET_P_FLAG | FLAGSET_C_FLAG;

class MicroOp
{
public:

	MicroOp(void)
		: type(TYPE_NOP)
	{}

	MicroOp(MicroOpType type_,
			OperandType operand0Type = OPERAND_TYPE_NONE, Dword operand0 = 0,
			OperandType operand1Type = OPERAND_TYPE_NONE, Dword operand1 = 0,
			OperandType operand2Type = OPERAND_TYPE_NONE, Dword operand2 = 0,
			OperandType operand3Type = OPERAND_TYPE_NONE, Dword operand3 = 0,
			OperandType operand4Type = OPERAND_TYPE_NONE, Dword operand4 = 0)
		:	type(type_), function(FUNC_NONE),
			flagsRead(0), flagsWritten(0)
	{
		operandTypes[0] = operand0Type; operands[0] = operand0;
		operandTypes[1] = operand1Type; operands[1] = operand1;
		operandTypes[2] = operand2Type; operands[2] = operand2;
		operandTypes[3] = operand3Type; operands[3] = operand3;
		operandTypes[4] = operand4Type; operands[4] = operand4;
	}

	MicroOp(MicroOpType type_, Dword function_,
			OperandType operand0Type = OPERAND_TYPE_NONE, Dword operand0 = 0,
			OperandType operand1Type = OPERAND_TYPE_NONE, Dword operand1 = 0,
			OperandType operand2Type = OPERAND_TYPE_NONE, Dword operand2 = 0,
			OperandType operand3Type = OPERAND_TYPE_NONE, Dword operand3 = 0,
			OperandType operand4Type = OPERAND_TYPE_NONE, Dword operand4 = 0)
		:	type(type_), function(function_),
			flagsRead(0), flagsWritten(0)
	{
		operandTypes[0] = operand0Type; operands[0] = operand0;
		operandTypes[1] = operand1Type; operands[1] = operand1;
		operandTypes[2] = operand2Type; operands[2] = operand2;
		operandTypes[3] = operand3Type; operands[3] = operand3;
		operandTypes[4] = operand4Type; operands[4] = operand4;
	}

	MicroOp(MicroOpType type_,
			FlagSet flagsRead_, FlagSet flagsWritten_,
			OperandType operand0Type = OPERAND_TYPE_NONE, Dword operand0 = 0,
			OperandType operand1Type = OPERAND_TYPE_NONE, Dword operand1 = 0,
			OperandType operand2Type = OPERAND_TYPE_NONE, Dword operand2 = 0,
			OperandType operand3Type = OPERAND_TYPE_NONE, Dword operand3 = 0,
			OperandType operand4Type = OPERAND_TYPE_NONE, Dword operand4 = 0)
		:	type(type_), function(FUNC_NONE),
			flagsRead(flagsRead_), flagsWritten(flagsWritten_)
	{
		operandTypes[0] = operand0Type; operands[0] = operand0;
		operandTypes[1] = operand1Type; operands[1] = operand1;
		operandTypes[2] = operand2Type; operands[2] = operand2;
		operandTypes[3] = operand3Type; operands[3] = operand3;
		operandTypes[4] = operand4Type; operands[4] = operand4;
	}

	MicroOp(MicroOpType type_, Dword function_,
			FlagSet flagsRead_, FlagSet flagsWritten_,
			OperandType operand0Type = OPERAND_TYPE_NONE, Dword operand0 = 0,
			OperandType operand1Type = OPERAND_TYPE_NONE, Dword operand1 = 0,
			OperandType operand2Type = OPERAND_TYPE_NONE, Dword operand2 = 0,
			OperandType operand3Type = OPERAND_TYPE_NONE, Dword operand3 = 0,
			OperandType operand4Type = OPERAND_TYPE_NONE, Dword operand4 = 0)
		:	type(type_), function(function_),
			flagsRead(flagsRead_), flagsWritten(flagsWritten_)
	{
		operandTypes[0] = operand0Type; operands[0] = operand0;
		operandTypes[1] = operand1Type; operands[1] = operand1;
		operandTypes[2] = operand2Type; operands[2] = operand2;
		operandTypes[3] = operand3Type; operands[3] = operand3;
		operandTypes[4] = operand4Type; operands[4] = operand4;
	}

	//Use default destructor, copy constructor & copy assignment operator

	//Convert to host code
	void ConvertToHostCode(	std::vector <Byte> & hostCode,
							Dword & immDwordOffset,
							std::vector <Dword> & readByteCallOffsets,
							std::vector <Dword> & readWordCallOffsets,
							std::vector <Dword> & readDwordCallOffsets,
							std::vector <Dword> & writeByteCallOffsets,
							std::vector <Dword> & writeWordCallOffsets,
							std::vector <Dword> & writeDwordCallOffsets,
							std::vector <Dword> & setDataSegmentRegisterValueCallOffsets) const;

	void ConvertTypeGetToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypePutToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeGetSegToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypePutSegToHostCode(	std::vector <Byte> & hostCode,
										std::vector <Dword> & setDataSegmentRegisterValueCallOffsets) const;
	void ConvertTypeGetEflagsToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypePutEflagsToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeReadToHostCode(	std::vector <Byte> & hostCode,
									std::vector <Dword> & readByteCallOffsets,
									std::vector <Dword> & readWordCallOffsets,
									std::vector <Dword> & readDwordCallOffsets) const;
	void ConvertTypeWriteToHostCode(std::vector <Byte> & hostCode, Dword & immDwordOffset,
									std::vector <Dword> & writeByteCallOffsets,
									std::vector <Dword> & writeWordCallOffsets,
									std::vector <Dword> & writeDwordCallOffsets) const;
	void ConvertTypeLea16ToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeLea32ToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeMovToHostCode(std::vector <Byte> & hostCode, Dword & immDwordOffset) const;
	void ConvertTypeCmovToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeZeroExToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeSignExToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeAluToHostCode(std::vector <Byte> & hostCode, Dword & immDwordOffset) const;
	void ConvertTypeUaluToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeShiftToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeShiftDToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeMulToHostCode(std::vector <Byte> & hostCode, Dword & immDwordOffset) const;
	void ConvertTypeDivToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeBitToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeBSwapToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeBitScanToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeASAdjustToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeMDAdjustToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeAddEipToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeAndEipToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeSetEipToHostCode(std::vector <Byte> & hostCode) const;
	void ConvertTypeDUpdateToHostCode(std::vector <Byte> & hostCode) const;

	MicroOpType type;
	Dword function;
	
	FlagSet flagsRead, flagsWritten;

	OperandType operandTypes[5];
	Dword operands[5];

	//Exception thrown by MicroOp functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};
