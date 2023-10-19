//	VMachine
//	Convert to host code
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "MicroOp.h"
#include "../Registers.h"
#include "../EmitFunctions/Emit.h"

namespace
{
	Dword GetL8Reg(Dword reg)
	{
		Dword reg8 = REG_NONE;

		switch(reg)
		{
		case REG_EAX:	reg8 = REG_AL;	break;
		case REG_ECX:	reg8 = REG_CL;	break;
		case REG_EDX:	reg8 = REG_DL;	break;
		case REG_EBX:	reg8 = REG_BL;	break;
		}

		assert(reg8 != REG_NONE);

		return reg8;
	}

	Dword GetH8Reg(Dword reg)
	{
		Dword reg8 = REG_NONE;

		switch(reg)
		{
		case REG_EAX:	reg8 = REG_AH;	break;
		case REG_ECX:	reg8 = REG_CH;	break;
		case REG_EDX:	reg8 = REG_DH;	break;
		case REG_EBX:	reg8 = REG_BH;	break;
		}

		assert(reg8 != REG_NONE);

		return reg8;
	}
}

void MicroOp::ConvertToHostCode(std::vector <Byte> & hostCode,
								Dword & immDwordOffset,
								std::vector <Dword> & readByteCallOffsets,
								std::vector <Dword> & readWordCallOffsets,
								std::vector <Dword> & readDwordCallOffsets,
								std::vector <Dword> & writeByteCallOffsets,
								std::vector <Dword> & writeWordCallOffsets,
								std::vector <Dword> & writeDwordCallOffsets,
								std::vector <Dword> & setDataSegmentRegisterValueCallOffsets) const
{
	switch(type)
	{
	case TYPE_NOP:
		break;
		
	case TYPE_GET:
		ConvertTypeGetToHostCode(hostCode);
		break;

	case TYPE_PUT:
		ConvertTypePutToHostCode(hostCode);
		break;

	case TYPE_GETSEG:
		ConvertTypeGetSegToHostCode(hostCode);
		break;

	case TYPE_PUTSEG:
		ConvertTypePutSegToHostCode(hostCode, setDataSegmentRegisterValueCallOffsets);
		break;

	case TYPE_GETEFLAGS:
		ConvertTypeGetEflagsToHostCode(hostCode);
		break;

	case TYPE_PUTEFLAGS:
		ConvertTypePutEflagsToHostCode(hostCode);
		break;

	case TYPE_READ:
		ConvertTypeReadToHostCode(	hostCode, readByteCallOffsets,
									readWordCallOffsets, readDwordCallOffsets);
		break;

	case TYPE_WRITE:
		ConvertTypeWriteToHostCode(	hostCode, immDwordOffset, writeByteCallOffsets,
									writeWordCallOffsets, writeDwordCallOffsets);
		break;

	case TYPE_LEA16:
		ConvertTypeLea16ToHostCode(hostCode);
		break;

	case TYPE_LEA32:
		ConvertTypeLea32ToHostCode(hostCode);
		break;

	case TYPE_MOV:
		ConvertTypeMovToHostCode(hostCode, immDwordOffset);
		break;

	case TYPE_CMOV:
		ConvertTypeCmovToHostCode(hostCode);
		break;

	case TYPE_ZEROEX:
		ConvertTypeZeroExToHostCode(hostCode);
		break;

	case TYPE_SIGNEX:
		ConvertTypeSignExToHostCode(hostCode);
		break;

	case TYPE_ALU:
		ConvertTypeAluToHostCode(hostCode, immDwordOffset);
		break;

	case TYPE_UALU:
		ConvertTypeUaluToHostCode(hostCode);
		break;

	case TYPE_SHIFT:
		ConvertTypeShiftToHostCode(hostCode);
		break;

	case TYPE_SHIFTD:
		ConvertTypeShiftDToHostCode(hostCode);
		break;

	case TYPE_MUL:
		ConvertTypeMulToHostCode(hostCode, immDwordOffset);
		break;

	case TYPE_DIV:
		ConvertTypeDivToHostCode(hostCode);
		break;

	case TYPE_BIT:
		ConvertTypeBitToHostCode(hostCode);
		break;

	case TYPE_BSWAP:
		ConvertTypeBSwapToHostCode(hostCode);
		break;

	case TYPE_BIT_SCAN:
		ConvertTypeBitScanToHostCode(hostCode);
		break;

	case TYPE_AS_ADJUST:
		ConvertTypeASAdjustToHostCode(hostCode);
		break;

	case TYPE_MD_ADJUST:
		ConvertTypeMDAdjustToHostCode(hostCode);
		break;

	case TYPE_ADDEIP:
		ConvertTypeAddEipToHostCode(hostCode);
		break;

	case TYPE_ANDEIP:
		ConvertTypeAndEipToHostCode(hostCode);
		break;
		
	case TYPE_SETEIP:
		ConvertTypeSetEipToHostCode(hostCode);
		break;

	case TYPE_CLD:
		{
			assert(flagsRead == 0);
			assert(flagsWritten == 0);

			EmitALU_Md_Id(	hostCode, ALU_FUNC_AND, REG_EBP, REG_NONE, 0,
							offsetof(Registers, r_systemFlags), ~EFLAGS_D_FLAG);

			break;
		}

	case TYPE_STD:
		{
			assert(flagsRead == 0);
			assert(flagsWritten == 0);

			EmitALU_Md_Id(	hostCode, ALU_FUNC_OR, REG_EBP, REG_NONE, 0,
							offsetof(Registers, r_systemFlags), EFLAGS_D_FLAG);

			break;
		}

	case TYPE_DUPDATE:
		ConvertTypeDUpdateToHostCode(hostCode);
		break;

	case TYPE_REGISTER_FPU:
		{
			assert(flagsRead == 0);
			assert(flagsWritten == 0);

			EmitRegisterFPU(hostCode, function);

			break;
		}

	case TYPE_MEMORY_FPU:
		{
			assert(flagsRead == 0);
			assert(flagsWritten == 0);

			EmitMemoryFPU(	hostCode, function, REG_EBP, REG_NONE, 0,
							offsetof(Registers, scratchMemory));

			break;
		}

	case TYPE_WAIT:
		{
			assert(flagsRead == 0);
			assert(flagsWritten == 0);

			EmitWAIT(hostCode);

			break;
		}

	default:
		{
			throw Ex("Internal Error: Unimplemented MicroOp");
		}
	}
}

void MicroOp::ConvertTypeGetToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_GET);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operandTypes[1] == OPERAND_TYPE_GUEST_REG);
	
	if(operands[1] == REG_NONE)
		EmitALU_Gd_Rd(hostCode, ALU_FUNC_XOR, operands[0], operands[0]);
	else
	{
		Dword offset = 0xffffffff;

		switch(operands[1])
		{
		case REG_EAX:	offset = offsetof(Registers, r_eax);	break;
		case REG_ECX:	offset = offsetof(Registers, r_ecx);	break;
		case REG_EDX:	offset = offsetof(Registers, r_edx);	break;
		case REG_EBX:	offset = offsetof(Registers, r_ebx);	break;
		case REG_ESP:	offset = offsetof(Registers, r_esp);	break;
		case REG_EBP:	offset = offsetof(Registers, r_ebp);	break;
		case REG_ESI:	offset = offsetof(Registers, r_esi);	break;
		case REG_EDI:	offset = offsetof(Registers, r_edi);	break;

		case REG_CS_BASE:	offset = offsetof(Registers, r_cs) + offsetof(SegmentRegisterData, base);	break;
		case REG_DS_BASE:	offset = offsetof(Registers, r_ds) + offsetof(SegmentRegisterData, base);	break;
		case REG_ES_BASE:	offset = offsetof(Registers, r_es) + offsetof(SegmentRegisterData, base);	break;
		case REG_FS_BASE:	offset = offsetof(Registers, r_fs) + offsetof(SegmentRegisterData, base);	break;
		case REG_GS_BASE:	offset = offsetof(Registers, r_gs) + offsetof(SegmentRegisterData, base);	break;
		case REG_SS_BASE:	offset = offsetof(Registers, r_ss) + offsetof(SegmentRegisterData, base);	break;

		case REG_EIP:	offset = offsetof(Registers, r_eip);	break;

		case REG_CR0:	offset = offsetof(Registers, r_cr0);	break;
		case REG_CR2:	offset = offsetof(Registers, r_cr2);	break;
		case REG_CR3:	offset = offsetof(Registers, r_cr3);	break;
		case REG_CR4:	offset = offsetof(Registers, r_cr4);	break;

		case REG_DR0:	offset = offsetof(Registers, r_dr[0]);	break;
		case REG_DR1:	offset = offsetof(Registers, r_dr[1]);	break;
		case REG_DR2:	offset = offsetof(Registers, r_dr[2]);	break;
		case REG_DR3:	offset = offsetof(Registers, r_dr[3]);	break;
		case REG_DR4:	offset = offsetof(Registers, r_dr[4]);	break;
		case REG_DR5:	offset = offsetof(Registers, r_dr[5]);	break;
		case REG_DR6:	offset = offsetof(Registers, r_dr[6]);	break;
		case REG_DR7:	offset = offsetof(Registers, r_dr[7]);	break;
		};

		assert(offset != 0xffffffff);

		EmitMOV_Gd_Md(hostCode, operands[0], REG_EBP, REG_NONE, 0, offset);
	}
}

void MicroOp::ConvertTypePutToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_PUT);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_GUEST_REG);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);

	Dword offset = 0xffffffff;

	switch(operands[0])
	{
	case REG_EAX:	offset = offsetof(Registers, r_eax);	break;
	case REG_ECX:	offset = offsetof(Registers, r_ecx);	break;
	case REG_EDX:	offset = offsetof(Registers, r_edx);	break;
	case REG_EBX:	offset = offsetof(Registers, r_ebx);	break;
	case REG_ESP:	offset = offsetof(Registers, r_esp);	break;
	case REG_EBP:	offset = offsetof(Registers, r_ebp);	break;
	case REG_ESI:	offset = offsetof(Registers, r_esi);	break;
	case REG_EDI:	offset = offsetof(Registers, r_edi);	break;
	};

	assert(offset != 0xffffffff);

	EmitMOV_Md_Gd(hostCode, REG_EBP, REG_NONE, 0, offset, operands[1]);
}

void MicroOp::ConvertTypeGetSegToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_GETSEG);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_16);
	assert(operandTypes[1] == OPERAND_TYPE_GUEST_SEG_REG);
	
	Dword offset = 0xffffffff;

	switch(operands[1])
	{
	case REG_CS:	offset = offsetof(Registers, r_cs) + offsetof(SegmentRegisterData, selector);	break;
	case REG_DS:	offset = offsetof(Registers, r_ds) + offsetof(SegmentRegisterData, selector);	break;
	case REG_ES:	offset = offsetof(Registers, r_es) + offsetof(SegmentRegisterData, selector);	break;
	case REG_FS:	offset = offsetof(Registers, r_fs) + offsetof(SegmentRegisterData, selector);	break;
	case REG_GS:	offset = offsetof(Registers, r_gs) + offsetof(SegmentRegisterData, selector);	break;
	case REG_SS:	offset = offsetof(Registers, r_ss) + offsetof(SegmentRegisterData, selector);	break;
	};

	assert(offset != 0xffffffff);

	EmitMOV_Gw_Mw(hostCode, operands[0], REG_EBP, REG_NONE, 0, offset);
}

void MicroOp::ConvertTypePutSegToHostCode(std::vector <Byte> & hostCode,
										  std::vector <Dword> & setDataSegmentRegisterValueCallOffsets) const
{
	assert(type == TYPE_PUTSEG);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_GUEST_SEG_REG);
	assert(	operands[0] == REG_DS || operands[0] == REG_ES ||
			operands[0] == REG_FS || operands[0] == REG_GS);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_16);

	EmitPUSH_Rd(hostCode, operands[1]);
	EmitPUSH_Id(hostCode, operands[0]);
	setDataSegmentRegisterValueCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
	EmitCALL(hostCode);	//SetDataSegmentRegisterValue
	EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ESP, 8);
}

void MicroOp::ConvertTypeGetEflagsToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_GETEFLAGS);
	assert(flagsRead == FLAGSET_STATUS_FLAGS);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);

	EmitMOV_Gd_Md(hostCode, operands[0], REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
}

void MicroOp::ConvertTypePutEflagsToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_PUTEFLAGS);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);

	EmitMOV_Md_Gd(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags), operands[0]);
}

void MicroOp::ConvertTypeReadToHostCode(std::vector <Byte> & hostCode,
										std::vector <Dword> & readByteCallOffsets,
										std::vector <Dword> & readWordCallOffsets,
										std::vector <Dword> & readDwordCallOffsets) const
{
	assert(type == TYPE_READ);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);
	
	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 ||
		operandTypes[0] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readByteCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);

		if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8)
			EmitMOV_Gb_Rb(hostCode, GetL8Reg(operands[0]), REG_AL);
		else
			EmitMOV_Gb_Rb(hostCode, GetH8Reg(operands[0]), REG_AL);
	}
    else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Gw_Rw(hostCode, operands[0], REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Gd_Rd(hostCode, operands[0], REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_SCRATCH_MEMORY_16)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Mw_Gw(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0], REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_SCRATCH_MEMORY_32)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0], REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_SCRATCH_MEMORY_64)
	{
		assert(operands[1] != REG_ECX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0], REG_EAX);
		
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 4);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0] + 4, REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_SCRATCH_MEMORY_80)
	{
		assert(operands[1] != REG_ECX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0], REG_EAX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 4);
		readDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0] + 4, REG_EAX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[1]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 8);
		readWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
		EmitMOV_Mw_Gw(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[0] + 8, REG_EAX);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeWriteToHostCode(	std::vector <Byte> & hostCode,
											Dword & immDwordOffset,
											std::vector <Dword> & writeByteCallOffsets,
											std::vector <Dword> & writeWordCallOffsets,
											std::vector <Dword> & writeDwordCallOffsets) const
{
	assert(type == TYPE_WRITE);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	
	if(	operandTypes[1] == OPERAND_TYPE_HOST_REG_L8 ||
		operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
		assert(operands[0] != REG_EDX);

		EmitMOV_Gd_Rd(hostCode, REG_EDX, operands[1]);
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);

		if(operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
			EmitSHIFT_Rd_Ib(hostCode, SHIFT_FUNC_SHR, REG_EDX, 8);
		
		writeByteCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		assert(operands[0] != REG_EDX);

		EmitMOV_Gd_Rd(hostCode, REG_EDX, operands[1]);
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		writeWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_HOST_REG_32)
	{
		assert(operands[0] != REG_EDX);

		EmitMOV_Gd_Rd(hostCode, REG_EDX, operands[1]);
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Rd_Id(hostCode, REG_EDX, operands[1]);
		writeByteCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_16)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Rd_Id(hostCode, REG_EDX, operands[1]);
		writeWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_32)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		immDwordOffset = hostCode.size() + EmitMOV_Rd_Id(hostCode, REG_EDX, operands[1]);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_SCRATCH_MEMORY_16)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1]);
		writeWordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_SCRATCH_MEMORY_32)
	{
		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1]);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_SCRATCH_MEMORY_64)
	{
		assert(operands[0] != REG_ECX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1]);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 4);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1] + 4);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else if(operandTypes[1] == OPERAND_TYPE_SCRATCH_MEMORY_80)
	{
		assert(operands[0] != REG_ECX);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1]);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 4);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1] + 4);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);

		EmitMOV_Gd_Rd(hostCode, REG_ECX, operands[0]);
		EmitALU_Rd_Id(hostCode, ALU_FUNC_ADD, REG_ECX, 8);
		EmitMOV_Gd_Md(	hostCode, REG_EDX, REG_EBP, REG_NONE, 0,
						offsetof(Registers, scratchMemory) + operands[1] + 8);
		writeDwordCallOffsets.push_back(static_cast<Dword>(hostCode.size()));
		EmitCALL(hostCode);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeLea16ToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_LEA16);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_16);

	EmitMOVZX_Gd_Rw(hostCode, operands[0], operands[1]);
	
	if(operandTypes[2] == OPERAND_TYPE_IMMEDIATE_16)
	{
		if(operands[2] != 0)
			EmitALU_Rw_Iw(hostCode, ALU_FUNC_ADD, operands[0], operands[2]);
	}
	else if(operandTypes[2] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[3] == OPERAND_TYPE_IMMEDIATE_16)
	{
		EmitALU_Gw_Rw(hostCode, ALU_FUNC_ADD, operands[0], operands[2]);

		if(operands[3] != 0)
			EmitALU_Rw_Iw(hostCode, ALU_FUNC_ADD, operands[0], operands[3]);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeLea32ToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_LEA32);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);
	assert(operandTypes[3] == OPERAND_TYPE_IMMEDIATE_8);
	assert(operandTypes[4] == OPERAND_TYPE_IMMEDIATE_32);

	Dword indexOperand;

	if(operandTypes[2] == OPERAND_TYPE_HOST_REG_32)
	{
		indexOperand = operands[2];
	}
	else if(operandTypes[2] == OPERAND_TYPE_NONE)
	{
		indexOperand = REG_NONE;
		assert(operands[3] == 0);
	}
	else
		assert(false);

	EmitLEA_Gd_M(hostCode, operands[0], operands[1], indexOperand, operands[3], operands[4]);
}

void MicroOp::ConvertTypeMovToHostCode(std::vector <Byte> & hostCode,
									   Dword & immDwordOffset) const
{
	assert(type == TYPE_MOV);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
		operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitMOV_Rb_Ib(hostCode, GetL8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitMOV_Rb_Ib(hostCode, GetH8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_16)
	{
		EmitMOV_Rw_Iw(hostCode, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_32)
	{
		immDwordOffset = hostCode.size() +
			EmitMOV_Rd_Id(hostCode, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		EmitMOV_Gb_Rb(hostCode, GetL8Reg(operands[0]), GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitMOV_Gb_Rb(hostCode, GetL8Reg(operands[0]), GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		EmitMOV_Gb_Rb(hostCode, GetH8Reg(operands[0]), GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitMOV_Gb_Rb(hostCode, GetH8Reg(operands[0]), GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitMOV_Gw_Rw(hostCode, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitMOV_Gd_Rd(hostCode, operands[0], operands[1]);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeCmovToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_CMOV);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);

	if(function < 0x10)
	{
		assert(flagsRead != 0);
		assert(!ConditionUsesOFlag(function));

		EmitPUSH_Rd(hostCode, REG_EAX);
		EmitMOV_Gb_Mb(hostCode, REG_AH, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		EmitSAHF(hostCode);
		EmitPOP_Rd(hostCode, REG_EAX);

		EmitCMOV_Gd_Rd(hostCode, function, operands[0], operands[1]);
	}
	else if(function == CONDITION_CXZ || function == CONDITION_CXNZ ||
			function == CONDITION_ECXZ || function == CONDITION_ECXNZ)
	{
		assert(flagsRead == 0);

		Dword testImm = (function == CONDITION_CXZ || function == CONDITION_CXNZ)
							? 0x0000ffff : 0xffffffff;
		Dword cmovCond = (function == CONDITION_CXZ || function == CONDITION_ECXZ)
							? CONDITION_Z : CONDITION_NZ;

		EmitALU_Md_Id(	hostCode, ALU_FUNC_TEST, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_ecx), testImm);

		EmitCMOV_Gd_Rd(hostCode, cmovCond, operands[0], operands[1]);
	}
}

void MicroOp::ConvertTypeZeroExToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_ZEROEX);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
        EmitMOVZX_Gw_Rb(hostCode, operands[0], GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
        EmitMOVZX_Gw_Rb(hostCode, operands[0], GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
        EmitMOVZX_Gd_Rb(hostCode, operands[0], GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
        EmitMOVZX_Gd_Rb(hostCode, operands[0], GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitMOVZX_Gd_Rw(hostCode, operands[0], operands[1]);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeSignExToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_SIGNEX);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
        EmitMOVSX_Gw_Rb(hostCode, operands[0], GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
        EmitMOVSX_Gw_Rb(hostCode, operands[0], GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
        EmitMOVSX_Gd_Rb(hostCode, operands[0], GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
        EmitMOVSX_Gd_Rb(hostCode, operands[0], GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitMOVSX_Gd_Rw(hostCode, operands[0], operands[1]);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeAluToHostCode(std::vector <Byte> & hostCode,
									   Dword & immDwordOffset) const
{
	assert(type == TYPE_ALU);
	assert(function != FUNC_NONE);
	if(function == ALU_FUNC_ADC || function == ALU_FUNC_SBB)
		assert(flagsRead == FLAGSET_C_FLAG);
	else
		assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	if(flagsRead == FLAGSET_C_FLAG)
	{
		EmitSHIFT_Md_Ib(hostCode, SHIFT_FUNC_SHR, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eflags), EFLAGS_C_SHIFT + 1);
	}

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
		operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitALU_Rb_Ib(hostCode, function, GetL8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitALU_Rb_Ib(hostCode, function, GetH8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_16)
	{
		EmitALU_Rw_Iw(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_32)
	{
		immDwordOffset = hostCode.size() +
			EmitALU_Rd_Id(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		EmitALU_Gb_Rb(	hostCode, function, GetL8Reg(operands[0]),
						GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitALU_Gb_Rb(	hostCode, function, GetL8Reg(operands[0]),
						GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		EmitALU_Gb_Rb(	hostCode, function, GetH8Reg(operands[0]),
						GetL8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitALU_Gb_Rb(	hostCode, function, GetH8Reg(operands[0]),
						GetH8Reg(operands[1]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitALU_Gw_Rw(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitALU_Gd_Rd(hostCode, function, operands[0], operands[1]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeUaluToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_UALU);
	assert(flagsRead == 0);
	assert(	flagsWritten == 0 ||
			flagsWritten == FLAGSET_STATUS_FLAGS ||
			flagsWritten == (FLAGSET_STATUS_FLAGS & ~FLAGSET_C_FLAG));

	if(flagsWritten == (FLAGSET_STATUS_FLAGS & ~FLAGSET_C_FLAG))
	{
		EmitSHIFT_Md_Ib(hostCode, SHIFT_FUNC_SHR, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eflags), EFLAGS_C_SHIFT + 1);
	}

	if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8)
	{
		EmitUALU_Rb(hostCode, function, GetL8Reg(operands[0]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8)
	{
		EmitUALU_Rb(hostCode, function, GetH8Reg(operands[0]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitUALU_Rw(hostCode, function, operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitUALU_Rd(hostCode, function, operands[0]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeShiftToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_SHIFT);
	assert(	(flagsRead == 0 && flagsWritten == 0) ||
			(flagsRead == FLAGSET_STATUS_FLAGS && flagsWritten == 0) ||
			(flagsRead == FLAGSET_STATUS_FLAGS &&
				flagsWritten == FLAGSET_STATUS_FLAGS));

	if(flagsRead != 0)
	{
		EmitPUSH_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		EmitPOPF(hostCode);
	}

	if(	(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 ||
		operandTypes[0] == OPERAND_TYPE_HOST_REG_H8) &&
		operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		assert(operands[1] == REG_ECX);
		
		if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8)
			EmitSHIFT_Rb_CL(hostCode, function, GetL8Reg(operands[0]));
		else
			EmitSHIFT_Rb_CL(hostCode, function, GetH8Reg(operands[0]));
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_L8 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFT_Rb_Ib(hostCode, function, GetL8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_H8 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFT_Rb_Ib(hostCode, function, GetH8Reg(operands[0]), operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		assert(operands[1] == REG_ECX);

		EmitSHIFT_Rw_CL(hostCode, function, operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFT_Rw_Ib(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_L8)
	{
		assert(operands[1] == REG_ECX);

		EmitSHIFT_Rd_CL(hostCode, function, operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFT_Rd_Ib(hostCode, function, operands[0], operands[1]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeShiftDToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_SHIFTD);
	assert(flagsRead == FLAGSET_STATUS_FLAGS);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	EmitPUSH_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	EmitPOPF(hostCode);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[1] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
	{
		assert(operands[2] == REG_ECX);
		
		EmitSHIFTD_Rw_Gw_CL(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[2] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFTD_Rw_Gw_Ib(hostCode, function, operands[0], operands[1], operands[2]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
	{
		assert(operands[2] == REG_ECX);

		EmitSHIFTD_Rd_Gd_CL(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[2] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitSHIFTD_Rd_Gd_Ib(hostCode, function, operands[0], operands[1], operands[2]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeMulToHostCode(std::vector <Byte> & hostCode,
									   Dword & immDwordOffset) const
{
	assert(type == TYPE_MUL);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	if(function == MUL_FUNC_MUL_16_8_8 || function == MUL_FUNC_IMUL_16_8_8)
	{
		//AX = AL * op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[0] == REG_EAX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_L8);
		assert(operands[1] == REG_EAX);

		if(function == MUL_FUNC_MUL_16_8_8)
		{
			if(operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
				EmitMUL_Rb(hostCode, GetL8Reg(operands[2]));
			else if(operandTypes[2] == OPERAND_TYPE_HOST_REG_H8)
				EmitMUL_Rb(hostCode, GetH8Reg(operands[2]));
			else
				assert(false);
		}
		else if(function == MUL_FUNC_IMUL_16_8_8)
		{
			if(operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
				EmitIMUL_Rb(hostCode, GetL8Reg(operands[2]));
			else if(operandTypes[2] == OPERAND_TYPE_HOST_REG_H8)
				EmitIMUL_Rb(hostCode, GetH8Reg(operands[2]));
			else
				assert(false);
		}

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else if(function == MUL_FUNC_MUL_32_16_16 || function == MUL_FUNC_IMUL_32_16_16)
	{
		//DX:AX = AX * op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[0] == REG_EDX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[1] == REG_EAX);
		assert(operandTypes[2] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[2] == REG_EAX);
		assert(operandTypes[3] == OPERAND_TYPE_HOST_REG_16);

		if(function == MUL_FUNC_MUL_32_16_16)
			EmitMUL_Rw(hostCode, operands[3]);
		else if(function == MUL_FUNC_IMUL_32_16_16)
			EmitIMUL_Rw(hostCode, operands[3]);

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else if(function == MUL_FUNC_MUL_64_32_32 || function == MUL_FUNC_IMUL_64_32_32)
	{
		//EDX:EAX = EAX * op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[0] == REG_EDX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[1] == REG_EAX);
		assert(operandTypes[2] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[2] == REG_EAX);
		assert(operandTypes[3] == OPERAND_TYPE_HOST_REG_32);

		if(function == MUL_FUNC_MUL_64_32_32)
			EmitMUL_Rd(hostCode, operands[3]);
		else if(function == MUL_FUNC_IMUL_64_32_32)
			EmitIMUL_Rd(hostCode, operands[3]);

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeDivToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_DIV);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	if(function == DIV_FUNC_DIV_16_16_8 || function == DIV_FUNC_IDIV_16_16_8)
	{
		//AX = AX / op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[0] == REG_EAX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[1] == REG_EAX);

		if(function == DIV_FUNC_DIV_16_16_8)
		{
			if(operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
				EmitDIV_Rb(hostCode, GetL8Reg(operands[2]));
			else if(operandTypes[2] == OPERAND_TYPE_HOST_REG_H8)
				EmitDIV_Rb(hostCode, GetH8Reg(operands[2]));
			else
				assert(false);
		}
		else if(function == DIV_FUNC_IDIV_16_16_8)
		{
			if(operandTypes[2] == OPERAND_TYPE_HOST_REG_L8)
				EmitIDIV_Rb(hostCode, GetL8Reg(operands[2]));
			else if(operandTypes[2] == OPERAND_TYPE_HOST_REG_H8)
				EmitIDIV_Rb(hostCode, GetH8Reg(operands[2]));
			else
				assert(false);
		}

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else if(function == DIV_FUNC_DIV_32_32_16 || function == DIV_FUNC_IDIV_32_32_16)
	{
		//DX:AX = DX:AX / op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[0] == REG_EDX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[1] == REG_EAX);
		assert(operandTypes[2] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[2] == REG_EDX);
		assert(operandTypes[3] == OPERAND_TYPE_HOST_REG_16);
		assert(operands[3] == REG_EAX);
		assert(operandTypes[4] == OPERAND_TYPE_HOST_REG_16);

		if(function == DIV_FUNC_DIV_32_32_16)
			EmitDIV_Rw(hostCode, operands[4]);
		else if(function == DIV_FUNC_IDIV_32_32_16)
			EmitIDIV_Rw(hostCode, operands[4]);

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else if(function == DIV_FUNC_DIV_64_64_32 || function == DIV_FUNC_IDIV_64_64_32)
	{
		//EDX:EAX = EDX:EAX / op2
		assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[0] == REG_EDX);
		assert(operandTypes[1] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[1] == REG_EAX);
		assert(operandTypes[2] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[2] == REG_EDX);
		assert(operandTypes[3] == OPERAND_TYPE_HOST_REG_32);
		assert(operands[3] == REG_EAX);
		assert(operandTypes[4] == OPERAND_TYPE_HOST_REG_32);

		if(function == DIV_FUNC_DIV_64_64_32)
			EmitDIV_Rd(hostCode, operands[4]);
		else if(function == DIV_FUNC_IDIV_64_64_32)
			EmitIDIV_Rd(hostCode, operands[4]);

		if(flagsWritten != 0)
		{
			EmitPUSHF(hostCode);
			EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
		}
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeBitToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_BIT);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitBIT_Rw_Ib(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8)
	{
		EmitBIT_Rd_Ib(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitBIT_Rw_Gw(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitBIT_Rd_Gd(hostCode, function, operands[0], operands[1]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeBSwapToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_BSWAP);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	
	EmitBSWAP_Rd(hostCode, operands[0]);
}

void MicroOp::ConvertTypeBitScanToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_BIT_SCAN);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);

	if(	operandTypes[0] == OPERAND_TYPE_HOST_REG_16 &&
		operandTypes[1] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitBITSCAN_Gw_Rw(hostCode, function, operands[0], operands[1]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32 &&
			operandTypes[1] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitBITSCAN_Gd_Rd(hostCode, function, operands[0], operands[1]);
	}
	else
		assert(false);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeASAdjustToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_AS_ADJUST);
	assert(flagsRead == FLAGSET_STATUS_FLAGS);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operands[0] == REG_EAX);

	EmitPUSH_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	EmitPOPF(hostCode);

	EmitASAdjust(hostCode, function);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeMDAdjustToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_MD_ADJUST);
	assert(flagsRead == 0);
	assert(flagsWritten == 0 || flagsWritten == FLAGSET_STATUS_FLAGS);
	assert(operandTypes[0] == OPERAND_TYPE_HOST_REG_32);
	assert(operands[0] == REG_EAX);
	assert(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_8);

	EmitMDAdjust(hostCode, function, operands[1]);

	if(flagsWritten != 0)
	{
		EmitPUSHF(hostCode);
		EmitPOP_Md(hostCode, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
	}
}

void MicroOp::ConvertTypeAddEipToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_ADDEIP);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	if(operandTypes[0] == OPERAND_TYPE_IMMEDIATE_32)
	{
		EmitALU_Md_Id(	hostCode, ALU_FUNC_ADD, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eip), operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitALU_Md_Gd(	hostCode, ALU_FUNC_ADD, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eip), operands[0]);
	}
	else
		assert(false);
}

void MicroOp::ConvertTypeAndEipToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_ANDEIP);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);
	assert(operandTypes[0] == OPERAND_TYPE_IMMEDIATE_32);

	EmitALU_Md_Id(	hostCode, ALU_FUNC_AND, REG_EBP, REG_NONE, 0,
					offsetof(Registers, r_eip), operands[0]);
}

void MicroOp::ConvertTypeSetEipToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_SETEIP);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	if(operandTypes[0] == OPERAND_TYPE_IMMEDIATE_32)
	{
		EmitMOV_Md_Id(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eip), operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32)
	{
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eip), operands[0]);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16)
	{
		EmitMOVZX_Gd_Rw(hostCode, REG_EAX, operands[0]);
		EmitMOV_Md_Gd(	hostCode, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_eip), REG_EAX);
	}
	else
		assert(false);
}

//Disable warning C4146: unary minus operator applied to unsigned type
#pragma warning(push)
#pragma warning(disable: 4146)

void MicroOp::ConvertTypeDUpdateToHostCode(std::vector <Byte> & hostCode) const
{
	assert(type == TYPE_DUPDATE);
	assert(flagsRead == 0);
	assert(flagsWritten == 0);

	assert(operands[0] != REG_EAX);
	assert(operands[0] != REG_ECX);

	if(operandTypes[0] == OPERAND_TYPE_HOST_REG_16)
	{
		assert(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_16);
		
		EmitPUSH_Rd(hostCode, REG_EAX);

		EmitMOV_Rw_Iw(hostCode, REG_EAX, operands[1]);
		EmitMOV_Rw_Iw(hostCode, REG_ECX, -operands[1]);

		EmitALU_Md_Id(	hostCode, ALU_FUNC_TEST, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_systemFlags), EFLAGS_D_FLAG);
		EmitCMOV_Gd_Rd(hostCode, CONDITION_NZ, REG_EAX, REG_ECX);

		EmitALU_Gw_Rw(hostCode, ALU_FUNC_ADD, operands[0], REG_EAX);

		EmitPOP_Rd(hostCode, REG_EAX);
	}
	else if(operandTypes[0] == OPERAND_TYPE_HOST_REG_32)
	{
		assert(operandTypes[1] == OPERAND_TYPE_IMMEDIATE_32);

		EmitPUSH_Rd(hostCode, REG_EAX);

		EmitMOV_Rd_Id(hostCode, REG_EAX, operands[1]);
		EmitMOV_Rd_Id(hostCode, REG_ECX, -operands[1]);

		EmitALU_Md_Id(	hostCode, ALU_FUNC_TEST, REG_EBP, REG_NONE, 0,
						offsetof(Registers, r_systemFlags), EFLAGS_D_FLAG);
		EmitCMOV_Gd_Rd(hostCode, CONDITION_NZ, REG_EAX, REG_ECX);

		EmitALU_Gd_Rd(hostCode, ALU_FUNC_ADD, operands[0], REG_EAX);

		EmitPOP_Rd(hostCode, REG_EAX);
	}
	else
		assert(false);
}

#pragma warning(pop)
