//	VMachine
//	Convert a guest instruction to microcode
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "MicroCode/MicroCode.h"

namespace
{
	const Dword A32 = OPCODE_A32;
	const Dword O32 = OPCODE_O32;

	//Constants to shorten the conversion code
	const OperandType GUEST		= OPERAND_TYPE_GUEST_REG;
	const OperandType GUEST_SEG	= OPERAND_TYPE_GUEST_SEG_REG;

	const OperandType HOST_L8	= OPERAND_TYPE_HOST_REG_L8;
	const OperandType HOST_H8	= OPERAND_TYPE_HOST_REG_H8;
	const OperandType HOST_16	= OPERAND_TYPE_HOST_REG_16;
	const OperandType HOST_32	= OPERAND_TYPE_HOST_REG_32;

	const OperandType IMM_8		= OPERAND_TYPE_IMMEDIATE_8;
	const OperandType IMM_16	= OPERAND_TYPE_IMMEDIATE_16;
	const OperandType IMM_32	= OPERAND_TYPE_IMMEDIATE_32;

	const OperandType MEM_16	= OPERAND_TYPE_SCRATCH_MEMORY_16;
	const OperandType MEM_32	= OPERAND_TYPE_SCRATCH_MEMORY_32;
	const OperandType MEM_64	= OPERAND_TYPE_SCRATCH_MEMORY_64;
	const OperandType MEM_80	= OPERAND_TYPE_SCRATCH_MEMORY_80;

	const FlagSet OF	= FLAGSET_O_FLAG;
	const FlagSet SF	= FLAGSET_S_FLAG;
	const FlagSet ZF	= FLAGSET_Z_FLAG;
	const FlagSet AF	= FLAGSET_A_FLAG;
	const FlagSet PF	= FLAGSET_P_FLAG;
	const FlagSet CF	= FLAGSET_C_FLAG;
	const FlagSet ALLF	= FLAGSET_STATUS_FLAGS;

	//And two #defines...
#define AddOp microCode.push_back(false, MicroOp
#define AddOpId microCode.push_back(true, MicroOp

	Dword GetOverrideSegRegBase(PrefixBitset prefixes)
	{
		if((prefixes & PREFIX_CS) != 0)
			return REG_CS_BASE;

		if((prefixes & PREFIX_DS) != 0)
			return REG_DS_BASE;

		if((prefixes & PREFIX_ES) != 0)
			return REG_ES_BASE;

		if((prefixes & PREFIX_FS) != 0)
			return REG_FS_BASE;

		if((prefixes & PREFIX_GS) != 0)
			return REG_GS_BASE;

		if((prefixes & PREFIX_SS) != 0)
			return REG_SS_BASE;

		return REG_NONE;
	}

	void Get8BitRegFromReg(Dword reg, OperandType & hostRegType, Dword & reg8)
	{
		reg8 = REG_NONE;

		switch(reg)
		{
		case 0:	hostRegType = HOST_L8;	reg8 = REG_EAX;	break;
		case 1:	hostRegType = HOST_L8;	reg8 = REG_ECX;	break;
		case 2:	hostRegType = HOST_L8;	reg8 = REG_EDX;	break;
		case 3:	hostRegType = HOST_L8;	reg8 = REG_EBX;	break;
		case 4:	hostRegType = HOST_H8;	reg8 = REG_EAX;	break;
		case 5:	hostRegType = HOST_H8;	reg8 = REG_ECX;	break;
		case 6:	hostRegType = HOST_H8;	reg8 = REG_EDX;	break;
		case 7:	hostRegType = HOST_H8;	reg8 = REG_EBX;	break;
		}

		assert(reg8 != REG_NONE);
	}

	Dword GetSwFromGd(Dword Gd)
	{
		assert(Gd < 8);

		switch(Gd)
		{
			case 0: return REG_ES;
			case 1: return REG_CS;
			case 2: return REG_SS;
			case 3: return REG_DS;
			case 4: return REG_FS;
			case 5: return REG_GS;
			case 6: return REG_DS;	//Reserved
			case 7: return REG_DS;	//Reserved
		}

		return REG_NONE;
	}

	Dword GetCdFromGd(Dword Gd)
	{
		assert(Gd < 8);

		switch(Gd)
		{
			case 0: return REG_CR0;
			case 1: return REG_NONE;
			case 2: return REG_CR2;
			case 3: return REG_CR3;
			case 4: return REG_CR4;
			case 5: return REG_NONE;
			case 6: return REG_NONE;	//Reserved
			case 7: return REG_NONE;	//Reserved
		}

		return REG_NONE;
	}

	Dword GetDdFromGd(Dword Gd)
	{
		assert(Gd < 8);

		return REG_DR0 + Gd;
	}
}

//Output an effective address calculation as microcode
//Places the address in REG_EBX
bool VMachine::OutputEffectiveAddressCalculation(	Dword opcode, PrefixBitset prefixes,
													bool translationMayChangeCsDsEsSsBasesZeroFlag,
													Dword & r_eip, MicroCode & microCode,
													Dword & Gd, bool addSegRegBase)
{
	Dword defSegRegBase;

	if((opcode & A32) == 0)
	{
		//Decode the ModR/M byte
		Dword baseReg, indexReg, displacement;
		if(!DecodeModRMByteRegMemoryA16(r_eip, Gd, defSegRegBase, baseReg, indexReg,
										displacement))
		{
			return false;
		}

		if(baseReg == REG_NONE && indexReg == REG_NONE)
		{
												//Mov EBX, displacement_16 (zero extended)
			AddOp(TYPE_MOV, HOST_32, REG_EBX, IMM_32, (displacement & 0xffff)));
		}
		else if(indexReg == REG_NONE)
		{
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
															//Lea16 EBX, [CX, displacement]
			AddOp(TYPE_LEA16, HOST_32, REG_EBX, HOST_16, REG_ECX, IMM_16, displacement));
		}
		else
		{
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, indexReg));	//Get EDX, indexReg
														//Lea16 EBX, [CX, DX, displacement]
			AddOp(TYPE_LEA16, HOST_32, REG_EBX, HOST_16, REG_ECX, HOST_16, REG_EDX, IMM_16, displacement));
		}
	}
	else
	{
		//Decode the ModR/M byte
		Dword baseReg, indexReg, shift, displacement;
		if(!DecodeModRMByteRegMemoryA32(r_eip, Gd, defSegRegBase, baseReg, indexReg,
										shift, displacement))
		{
			return false;
		}

		if(indexReg == REG_NONE)
		{
			assert(shift == 0);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
												//Lea32 EBX, [ECX, REG_NONE, 0, displacement]
			AddOp(TYPE_LEA32, HOST_32, REG_EBX, HOST_32, REG_ECX, OPERAND_TYPE_NONE, 0, IMM_8, 0, IMM_32, displacement));
		}
		else
		{
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, indexReg));	//Get EDX, indexReg
												//Lea32 EBX, [ECX, EDX, shift, displacement]
			AddOp(TYPE_LEA32, HOST_32, REG_EBX, HOST_32, REG_ECX, HOST_32, REG_EDX, IMM_8, shift, IMM_32, displacement));
		}
	}

	if(addSegRegBase)
	{
		//Calculate which segment register to use
		Dword segRegBase = GetOverrideSegRegBase(prefixes);
		if(segRegBase == REG_NONE)
			segRegBase = defSegRegBase;

#ifdef ZERO_BASE_OPTIMISATION
		bool csDsEsSsBasesZero = (	registers->r_cs.base == 0 && registers->r_ds.base == 0 &&
									registers->r_es.base == 0 && registers->r_ss.base == 0);

		if(	translationMayChangeCsDsEsSsBasesZeroFlag || !csDsEsSsBasesZero ||
			segRegBase == REG_FS_BASE || segRegBase == REG_GS_BASE)
#endif
		{
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, segRegBase));			//Get ECX, SEG.BASE
																			//Add EBX, ECX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_ECX));
		}
	}

	return true;
}

bool VMachine::ConvertGuestInstructionToMicroCode(	Dword opcode, PrefixBitset prefixes,
													Dword startEip, Dword & r_eip,
													bool & translationMayChangeCsDsEsSsBasesZeroFlag,
													MicroCode & microCode,
													Dword & guestImmDwordOffset,
													bool & instructionMayCauseException)
{
	switch(opcode)
	{
	//DATA TRANSFER INSTRUCTIONS

	//MOV Mb, Gb
	case 0x8800: case (A32 | 0x8800): case (O32 | 0x8800): case (A32 | O32 | 0x8800):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gb));				//Get EAX, Gb_32
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostRegType, REG_EAX));	//Write [EBX], {AL|AH}

			break;
		}

	//MOV Gb, Mb
	case 0x8a00: case (A32 | 0x8a00): case (O32 | 0x8a00): case (A32 | O32 | 0x8a00):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));		//Read AL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));				//Get ECX, Gb_32
			AddOp(TYPE_MOV, hostRegType, REG_ECX, HOST_L8, REG_EAX));	//Mov C{L|H}, AL
			AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));				//Put Gb_32, ECX

			break;
		}

	//MOV Rb, Gb
	case 0x88c0: case (A32 | 0x88c0): case (O32 | 0x88c0): case (A32 | O32 | 0x88c0):
	//MOV Gb, Rb
	case 0x8ac0: case (A32 | 0x8ac0): case (O32 | 0x8ac0): case (A32 | O32 | 0x8ac0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//If a "Gb, Rb" instruction, swap Gd and Rd
			if((opcode & 0xff00) == 0x8a00)
				std::swap(Gd, Rd);

			//Calculate Gb and Rb
			OperandType GbHostRegType, RbHostRegType;
			Dword Gb, Rb;
			Get8BitRegFromReg(Gd, GbHostRegType, Gb);
			Get8BitRegFromReg(Rd, RbHostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));			//Get ECX, Rb_32
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gb));			//Get EDX, Gb_32
																	//Mov {CL|CH}, {DL|DH}
			AddOp(TYPE_MOV, RbHostRegType, REG_ECX, GbHostRegType, REG_EDX));
			AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_ECX));			//Put Rb_32, ECX

			break;
		}

	case 0x8900:			case (A32 | 0x8900):		//MOV Mw, Gw
	case (O32 | 0x8900):	case (A32 | O32 | 0x8900):	//MOV Md, Gd
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			OperandType hostRegType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));				//Get ECX, Gd
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostRegType, REG_ECX));	//Write [EBX], eCX

			break;
		}

	case 0x8b00:	case (A32 | 0x8b00):	//MOV Gw, Mw
	case 0x0fb700:	case (A32 | 0x0fb700):	//MOV(ZX) Gw, Mw
	case 0x0fbf00:	case (A32 | 0x0fbf00):	//MOV(SX) Gw, Mw
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));	//Read AX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));	//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	case (O32 | 0x8b00): case (A32 | O32 | 0x8b00):	//MOV Gd, Md
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));	//Read ECX, [EBX]
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	case 0x89c0: case (A32 | 0x89c0):	//MOV Rw, Gw
	case 0x8bc0: case (A32 | 0x8bc0):	//MOV Gw, Rw
	case 0x0fb7c0:	case (A32 | 0x0fb7c0):	//MOV(ZX) Gw, Rw
	case 0x0fbfc0:	case (A32 | 0x0fbfc0):	//MOV(SX) Gw, Rw
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//If an "Rw, Gw" instruction, swap Rd and Gd
			if((opcode & 0xff00) == 0x8900)
				std::swap(Gd, Rd);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));			//Get EDX, Rd
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EDX));	//Mov CX, DX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	case (O32 | 0x89c0): case (A32 | O32 | 0x89c0):	//MOV Rd, Gd
	case (O32 | 0x8bc0): case (A32 | O32 | 0x8bc0):	//MOV Gd, Rd
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//If an "Rd, Gd" instruction, swap Rd and Gd
			if((opcode & 0xff00) == 0x8900)
				std::swap(Gd, Rd);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));		//Get ECX, Rd
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));		//Put Gd, ECX

			break;
		}

	//MOV Regb, Ib
	case 0xb0: case (A32 | 0xb0): case (O32 | 0xb0): case (A32 | O32 | 0xb0):
	case 0xb1: case (A32 | 0xb1): case (O32 | 0xb1): case (A32 | O32 | 0xb1):
	case 0xb2: case (A32 | 0xb2): case (O32 | 0xb2): case (A32 | O32 | 0xb2):
	case 0xb3: case (A32 | 0xb3): case (O32 | 0xb3): case (A32 | O32 | 0xb3):
	case 0xb4: case (A32 | 0xb4): case (O32 | 0xb4): case (A32 | O32 | 0xb4):
	case 0xb5: case (A32 | 0xb5): case (O32 | 0xb5): case (A32 | O32 | 0xb5):
	case 0xb6: case (A32 | 0xb6): case (O32 | 0xb6): case (A32 | O32 | 0xb6):
	case 0xb7: case (A32 | 0xb7): case (O32 | 0xb7): case (A32 | O32 | 0xb7):
		{
			instructionMayCauseException = false;

			OperandType hostRegType = ((opcode & 0x04) == 0x00) ? HOST_L8 : HOST_H8;
			Dword reg = opcode & 0x03;

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, reg));			//Get ECX, reg
			AddOp(TYPE_MOV, hostRegType, REG_ECX, IMM_8, Ib));		//Mov C{L|H}, Ib
			AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_ECX));			//Put reg, ECX

			break;
		}

	//MOV Regw, Iw
	case 0xb8:			case 0xb9:			case 0xba:			case 0xbb:
	case 0xbc:			case 0xbd:			case 0xbe:			case 0xbf:
	case (A32 | 0xb8):	case (A32 | 0xb9):	case (A32 | 0xba):	case (A32 | 0xbb):
	case (A32 | 0xbc):	case (A32 | 0xbd):	case (A32 | 0xbe):	case (A32 | 0xbf):
		{
			instructionMayCauseException = false;

			Dword reg = REG_EAX + (opcode & 0x07);

			Word Iw;
			if(!ReadCodeWord(r_eip, Iw))
				return false;
			r_eip += 2;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, reg));			//Get ECX, reg
			AddOp(TYPE_MOV, HOST_16, REG_ECX, IMM_16, Iw));			//Mov CX, Iw
			AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_ECX));			//Put reg, ECX

			break;
		}

	//MOV Regd, Id
	case (O32 | 0xb8):	case (O32 | 0xb9):	case (O32 | 0xba):	case (O32 | 0xbb):
	case (O32 | 0xbc):	case (O32 | 0xbd):	case (O32 | 0xbe):	case (O32 | 0xbf):
	case (A32 | O32 | 0xb8):	case (A32 | O32 | 0xb9):
	case (A32 | O32 | 0xba):	case (A32 | O32 | 0xbb):
	case (A32 | O32 | 0xbc):	case (A32 | O32 | 0xbd):
	case (A32 | O32 | 0xbe):	case (A32 | O32 | 0xbf):
		{
			instructionMayCauseException = false;

			Dword reg = REG_EAX + (opcode & 0x07);

			Dword Id;
			guestImmDwordOffset = r_eip - startEip;
			if(!ReadCodeDword(r_eip, Id))
				return false;
			r_eip += 4;

			//TODO: Put reg, Id
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, reg));			//Get ECX, reg
			AddOpId(TYPE_MOV, HOST_32, REG_ECX, IMM_32, Id));		//Mov ECX, Id
			AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_ECX));			//Put reg, ECX

			break;
		}

	//MOV Mb, Ib
	case 0xc600: case (A32 | 0xc600): case (O32 | 0xc600): case (A32 | O32 | 0xc600):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Read the immediate byte
			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_WRITE, HOST_32, REG_EBX, IMM_8, Ib));	//Write [EBX], Ib

			break;
		}

	case 0xc700: case (A32 | 0xc700):	//MOV Mw, Iw
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Read the immediate word
			Word Iw;
			if(!ReadCodeWord(r_eip, Iw))
				return false;
			r_eip += 2;

			AddOp(TYPE_WRITE, HOST_32, REG_EBX, IMM_16, Iw));	//Write [EBX], Iw

			break;
		}

	case (O32 | 0xc700): case (A32 | O32 | 0xc700):	//MOV Md, Id
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Read the immediate dword
			Dword Id;
			guestImmDwordOffset = r_eip - startEip;
			if(!ReadCodeDword(r_eip, Id))
				return false;
			r_eip += 4;

			AddOpId(TYPE_WRITE, HOST_32, REG_EBX, IMM_32, Id));	//Write [EBX], Id

			break;
		}

	case 0xa0:			case (A32 | 0xa0):			//MOV AL, Ob
	case (O32 | 0xa0):	case (A32 | O32 | 0xa0):
	case 0xa1:			case (A32 | 0xa1):			//MOV AX, Ow
	case (O32 | 0xa1):	case (A32 | O32 | 0xa1):	//MOV EAX, Od
		{
			//Read the offset
			Dword off;
			if((opcode & A32) == 0)
			{
				Word off16;
				if(!ReadCodeWord(r_eip, off16))
					return false;
				r_eip += 2;

				off = off16;
			}
			else
			{
				guestImmDwordOffset = r_eip - startEip;
				if(!ReadCodeDword(r_eip, off))
					return false;
				r_eip += 4;
			}

			//Calculate which segment register to use
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			//Calculate which host register type to use
			OperandType hostType = HOST_L8;
			if(opcode == 0xa1 || opcode == (A32 | 0xa1))
				hostType = HOST_16;
			if(opcode == (O32 | 0xa1) || opcode == (A32 | O32 | 0xa1))
				hostType = HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, segRegBase));	//Get ECX, segRegBase
			if((opcode & A32) != 0)									//Add ECX, off
				AddOpId(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ECX, IMM_32, off));
			else
				AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ECX, IMM_32, off));
			AddOp(TYPE_READ, hostType, REG_EAX, HOST_32, REG_ECX));	//Read {AL|eAX}, [ECX]
			if(hostType == HOST_32)									//If 32 bits read
			{
				AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));	//Put gEAX, EAX
			}
			else													//Otherwise
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));	//Get ECX, gEAX
				AddOp(TYPE_MOV, hostType, REG_ECX, hostType, REG_EAX));//Mov {CL|CX}, {AL|AX}
				AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_ECX));	//Put gEAX, ECX
			}

			break;
		}

	case 0xa2:			case (A32 | 0xa2):			//MOV Ob, AL
	case (O32 | 0xa2):	case (A32 | O32 | 0xa2):
	case 0xa3:			case (A32 | 0xa3):			//MOV Ow, AX
	case (O32 | 0xa3):	case (A32 | O32 | 0xa3):	//MOV Od, EAX
		{
			//Read the offset
			Dword off;
			if((opcode & A32) == 0)
			{
				Word off16;
				if(!ReadCodeWord(r_eip, off16))
					return false;
				r_eip += 2;

				off = off16;
			}
			else
			{
				guestImmDwordOffset = r_eip - startEip;
				if(!ReadCodeDword(r_eip, off))
					return false;
				r_eip += 4;
			}

			//Calculate which segment register to use
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			//Calculate which host register type to use
			OperandType hostType = HOST_L8;
			if(opcode == 0xa3 || opcode == (A32 | 0xa3))
				hostType = HOST_16;
			if(opcode == (O32 | 0xa3) || opcode == (A32 | O32 | 0xa3))
				hostType = HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, segRegBase));	//Get ECX, segRegBase
			if((opcode & A32) != 0)									//Add ECX, off
				AddOpId(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ECX, IMM_32, off));
			else
				AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ECX, IMM_32, off));
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EAX));		//Get EDX, gEAX
			AddOp(TYPE_WRITE, HOST_32, REG_ECX, hostType, REG_EDX));//Write [ECX], {DL|eDX}

			break;
		}

	//MOV Mw, Sw
	case 0x8c00: case (A32 | 0x8c00): case (O32 | 0x8c00): case (A32 | O32 | 0x8c00):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Sw
			Dword Sw = GetSwFromGd(Gd);

			AddOp(TYPE_GETSEG, HOST_16, REG_ECX, GUEST_SEG, Sw));	//GETSEG CX, Sw
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_16, REG_ECX));	//Write [EBX], CX
			
			break;
		}

	//MOV Rw, Sw
	case 0x8cc0: case (A32 | 0x8cc0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Sw
			Dword Sw = GetSwFromGd(Gd);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GETSEG, HOST_16, REG_EDX, GUEST_SEG, Sw));	//GETSEG DX, Sw
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EDX));	//Mov CX, DX
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));			//Put Rd, ECX

			break;
		}

	//MOV Rd, Sw
	case (O32 | 0x8cc0): case (A32 | O32 | 0x8cc0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Sw
			Dword Sw = GetSwFromGd(Gd);

			AddOp(TYPE_GETSEG, HOST_16, REG_EDX, GUEST_SEG, Sw));	//GETSEG DX, Sw
			AddOp(TYPE_ZEROEX, HOST_32, REG_EDX, HOST_16, REG_EDX));//ZeroEx EDX, DX
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));			//Put Rd, EDX

			break;
		}

	//MOV Sw, Mw. Used for Sw = ES, DS, FS, GS, (DS, DS)
	case 0x8e00: case (A32 | 0x8e00): case (O32 | 0x8e00): case (A32 | O32 | 0x8e00):
	case 0x8e18: case (A32 | 0x8e18): case (O32 | 0x8e18): case (A32 | O32 | 0x8e18):
	case 0x8e20: case (A32 | 0x8e20): case (O32 | 0x8e20): case (A32 | O32 | 0x8e20):
	case 0x8e28: case (A32 | 0x8e28): case (O32 | 0x8e28): case (A32 | O32 | 0x8e28):
	case 0x8e30: case (A32 | 0x8e30): case (O32 | 0x8e30): case (A32 | O32 | 0x8e30):
	case 0x8e38: case (A32 | 0x8e38): case (O32 | 0x8e38): case (A32 | O32 | 0x8e38):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Sw
			Dword Sw = GetSwFromGd(Gd);

			if(Sw == REG_DS || Sw == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			AddOp(TYPE_READ, HOST_16, REG_ECX, HOST_32, REG_EBX));	//Read CX, [EBX]
			AddOp(TYPE_PUTSEG, GUEST_SEG, Sw, HOST_16, REG_ECX));	//Putseg Sw, CX
						
			break;
		}

	//MOV Sw, Rw. Used for Sw = ES, DS, FS, GS, (DS, DS)
	case 0x8ec0: case (A32 | 0x8ec0): case (O32 | 0x8ec0): case (A32 | O32 | 0x8ec0):
	case 0x8ed8: case (A32 | 0x8ed8): case (O32 | 0x8ed8): case (A32 | O32 | 0x8ed8):
	case 0x8ee0: case (A32 | 0x8ee0): case (O32 | 0x8ee0): case (A32 | O32 | 0x8ee0):
	case 0x8ee8: case (A32 | 0x8ee8): case (O32 | 0x8ee8): case (A32 | O32 | 0x8ee8):
	case 0x8ef0: case (A32 | 0x8ef0): case (O32 | 0x8ef0): case (A32 | O32 | 0x8ef0):
	case 0x8ef8: case (A32 | 0x8ef8): case (O32 | 0x8ef8): case (A32 | O32 | 0x8ef8):
		{
			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Sw
			Dword Sw = GetSwFromGd(Gd);

			if(Sw == REG_DS || Sw == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_PUTSEG, GUEST_SEG, Sw, HOST_16, REG_ECX));	//Putseg Sw, CX

			break;
		}

	//MOV Rd, Cd
	case 0x0f20c0: case (A32 | 0x0f20c0): case (O32 | 0x0f20c0): case (A32 | O32 | 0x0f20c0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Cd
			Dword Cd = GetCdFromGd(Gd);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Cd));		//Get ECX, Cd
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	//MOV Rd, Dd
	case 0x0f21c0: case (A32 | 0x0f21c0): case (O32 | 0x0f21c0): case (A32 | O32 | 0x0f21c0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Cd
			Dword Dd = GetCdFromGd(Gd);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Dd));		//Get ECX, Dd
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	case 0x0fb600:	case (A32 | 0x0fb600):	//MOVZX Gw, Mb
	case 0x0fbe00:	case (A32 | 0x0fbe00):	//MOVSX Gw, Mb
		{
			MicroOpType exType = ((opcode & 0xff00) == 0xb600) ? TYPE_ZEROEX : TYPE_SIGNEX;

			//Effective address => EBX
			Dword Gw;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gw))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));	//Read AL, [EBX]
			AddOp(exType, HOST_16, REG_EAX, HOST_L8, REG_EAX));		//Ex AX, AL
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gw));			//Get ECX, Gw
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));	//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, Gw, HOST_32, REG_ECX));			//Put Gw, ECX

			break;
		}

	case (O32 | 0x0fb600):	case (A32 | O32 | 0x0fb600):	//MOVZX Gd, Mb
	case (O32 | 0x0fbe00):	case (A32 | O32 | 0x0fbe00):	//MOVSX Gd, Mb
		{
			MicroOpType exType = ((opcode & 0xff00) == 0xb600) ? TYPE_ZEROEX : TYPE_SIGNEX;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));	//Read AL, [EBX]
			AddOp(exType, HOST_32, REG_EAX, HOST_L8, REG_EAX));		//Ex EAX, AL
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case 0x0fb6c0:	case (A32 | 0x0fb6c0):	//MOVZX Gw, Rb
	case 0x0fbec0:	case (A32 | 0x0fbec0):	//MOVSX Gw, Rb
		{
			instructionMayCauseException = false;

			MicroOpType exType = ((opcode & 0xff00) == 0xb600) ? TYPE_ZEROEX : TYPE_SIGNEX;

			Dword Gw, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gw, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Rb));			//Get EAX, Rb_32
			AddOp(exType, HOST_16, REG_EAX, hostRegType, REG_EAX));	//Ex AX, {AL|AH}
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gw));			//Get ECX, Gw
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));	//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, Gw, HOST_32, REG_ECX));			//Put Gw, ECX

			break;
		}

	case (O32 | 0x0fb6c0):	case (A32 | O32 | 0x0fb6c0):	//MOVZX Gd, Rb
	case (O32 | 0x0fbec0):	case (A32 | O32 | 0x0fbec0):	//MOVSX Gd, Rb
		{
			instructionMayCauseException = false;

			MicroOpType exType = ((opcode & 0xff00) == 0xb600) ? TYPE_ZEROEX : TYPE_SIGNEX;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Rb));			//Get EAX, Rb_32
			AddOp(exType, HOST_32, REG_EAX, hostRegType, REG_EAX));	//Ex EAX, {AL|AH}
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case (O32 | 0x0fb700):	case (A32 | O32 | 0x0fb700):	//MOVZX Gd, Mw
	case (O32 | 0x0fbf00):	case (A32 | O32 | 0x0fbf00):	//MOVSX Gd, Mw
		{
			MicroOpType exType = ((opcode & 0xff00) == 0xb700) ? TYPE_ZEROEX : TYPE_SIGNEX;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));	//Read AX, [EBX]
			AddOp(exType, HOST_32, REG_EAX, HOST_16, REG_EAX));		//Ex EAX, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case (O32 | 0x0fb7c0):	case (A32 | O32 | 0x0fb7c0):	//MOVZX Gd, Rw
	case (O32 | 0x0fbfc0):	case (A32 | O32 | 0x0fbfc0):	//MOVSX Gd, Rw
		{
			instructionMayCauseException = false;

			MicroOpType exType = ((opcode & 0xff00) == 0xb700) ? TYPE_ZEROEX : TYPE_SIGNEX;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Rd));			//Get EAX, Rd
			AddOp(exType, HOST_32, REG_EAX, HOST_16, REG_EAX));		//Ex EAX, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	//CMOV Gw, Mw
	case 0x0f4200: case (A32 | 0x0f4200): case 0x0f4300: case (A32 | 0x0f4300):
	case 0x0f4400: case (A32 | 0x0f4400): case 0x0f4500: case (A32 | 0x0f4500):
	case 0x0f4600: case (A32 | 0x0f4600): case 0x0f4700: case (A32 | 0x0f4700):
	case 0x0f4800: case (A32 | 0x0f4800): case 0x0f4900: case (A32 | 0x0f4900):
	case 0x0f4a00: case (A32 | 0x0f4a00): case 0x0f4b00: case (A32 | 0x0f4b00):
		{
			Dword cond = (opcode & 0x0f00) >> 8;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));			//Read AX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));					//Get ECX, Gd
																		//CMov [cond] CX, AX
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_16, REG_ECX, HOST_16, REG_EAX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));					//Put Gd, ECX

			break;
		}

	//CMOV Gd, Md
	case (O32 | 0x0f4200): case (A32 | O32 | 0x0f4200):
	case (O32 | 0x0f4300): case (A32 | O32 | 0x0f4300):
	case (O32 | 0x0f4400): case (A32 | O32 | 0x0f4400):
	case (O32 | 0x0f4500): case (A32 | O32 | 0x0f4500):
	case (O32 | 0x0f4600): case (A32 | O32 | 0x0f4600):
	case (O32 | 0x0f4700): case (A32 | O32 | 0x0f4700):
	case (O32 | 0x0f4800): case (A32 | O32 | 0x0f4800):
	case (O32 | 0x0f4900): case (A32 | O32 | 0x0f4900):
	case (O32 | 0x0f4a00): case (A32 | O32 | 0x0f4a00):
	case (O32 | 0x0f4b00): case (A32 | O32 | 0x0f4b00):
		{
			Dword cond = (opcode & 0x0f00) >> 8;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_32, REG_EAX, HOST_32, REG_EBX));			//Read EAX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));					//Get ECX, Gd
																	//CMov [cond] ECX, EAX
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EAX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));					//Put Gd, ECX

			break;
		}

	//CMOV Gw, Rw
	case 0x0f42c0: case (A32 | 0x0f42c0): case 0x0f43c0: case (A32 | 0x0f43c0):
	case 0x0f44c0: case (A32 | 0x0f44c0): case 0x0f45c0: case (A32 | 0x0f45c0):
	case 0x0f46c0: case (A32 | 0x0f46c0): case 0x0f47c0: case (A32 | 0x0f47c0):
	case 0x0f48c0: case (A32 | 0x0f48c0): case 0x0f49c0: case (A32 | 0x0f49c0):
	case 0x0f4ac0: case (A32 | 0x0f4ac0): case 0x0f4bc0: case (A32 | 0x0f4bc0):
		{
			instructionMayCauseException = false;

			Dword cond = (opcode & 0x0f00) >> 8;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));					//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));					//Get EDX, Rd
																		//CMov [cond] CX, DX
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_16, REG_ECX, HOST_16, REG_EDX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));					//Put Gd, ECX

			break;
		}

	//CMOV Gd, Rd
	case (O32 | 0x0f42c0): case (A32 | O32 | 0x0f42c0):
	case (O32 | 0x0f43c0): case (A32 | O32 | 0x0f43c0):
	case (O32 | 0x0f44c0): case (A32 | O32 | 0x0f44c0):
	case (O32 | 0x0f45c0): case (A32 | O32 | 0x0f45c0):
	case (O32 | 0x0f46c0): case (A32 | O32 | 0x0f46c0):
	case (O32 | 0x0f47c0): case (A32 | O32 | 0x0f47c0):
	case (O32 | 0x0f48c0): case (A32 | O32 | 0x0f48c0):
	case (O32 | 0x0f49c0): case (A32 | O32 | 0x0f49c0):
	case (O32 | 0x0f4ac0): case (A32 | O32 | 0x0f4ac0):
	case (O32 | 0x0f4bc0): case (A32 | O32 | 0x0f4bc0):
		{
			instructionMayCauseException = false;

			Dword cond = (opcode & 0x0f00) >> 8;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));					//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));					//Get EDX, Rd
																	//CMov [cond] ECX, EDX
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));					//Put Gd, ECX

			break;
		}

	//PUSH ES, CS, SS, DS, FS, GS (O16)
	case 0x06:			case 0x0e:				case 0x16:
	case 0x1e:			case 0x0fa0:			case 0x0fa8:
	case (A32 | 0x06):	case (A32 | 0x0e):		case (A32 | 0x16):
	case (A32 | 0x1e):	case (A32 | 0x0fa0):	case (A32 | 0x0fa8):
	//PUSH AX, CX, DX, BX, SP, BP, SI, DI
	case 0x50:			case 0x51:			case 0x52:			case 0x53:
	case 0x54:			case 0x55:			case 0x56:			case 0x57:
	case (A32 | 0x50):	case (A32 | 0x51):	case (A32 | 0x52):	case (A32 | 0x53):
	case (A32 | 0x54):	case (A32 | 0x55):	case (A32 | 0x56):	case (A32 | 0x57):
		{
			//No special-case code required for PUSH SP
			Dword reg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0x06:		reg = REG_ES;	break;
			case 0x0e:		reg = REG_CS;	break;
			case 0x16:		reg = REG_SS;	break;
			case 0x1e:		reg = REG_DS;	break;
			case 0x0fa0:	reg = REG_FS;	break;
			case 0x0fa8:	reg = REG_GS;	break;
			case 0x9c:		break;
			default:		reg = REG_EAX + (opcode & 0x07);
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
																//Sub eSI, 2
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 2));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

																//If segment register push
			if(	reg == REG_CS || reg == REG_DS || reg == REG_ES || reg == REG_SS ||
				reg == REG_FS || reg == REG_GS)
			{
				AddOp(TYPE_GETSEG, HOST_16, REG_EBX, GUEST_SEG, reg));	//GetSeg BX, reg
			}
			else												//Else
				AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, reg));		//Get EBX, reg

			AddOp(TYPE_WRITE, HOST_32, REG_EDI, HOST_16, REG_EBX));	//Write [EDI], BX
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	//PUSH ES, CS, SS, DS, FS, GS (O32)
	case (O32 | 0x06):			case (O32 | 0x0e):
	case (O32 | 0x16):			case (O32 | 0x1e):
	case (O32 | 0x0fa0):		case (O32 | 0x0fa8):
	case (A32 | O32 | 0x06):	case (A32 | O32 | 0x0e):
	case (A32 | O32 | 0x16):	case (A32 | O32 | 0x1e):
	case (A32 | O32 | 0x0fa0):	case (A32 | O32 | 0x0fa8):
	//PUSH EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
	case (O32 | 0x50):			case (O32 | 0x51):
	case (O32 | 0x52):			case (O32 | 0x53):
	case (O32 | 0x54):			case (O32 | 0x55):
	case (O32 | 0x56):			case (O32 | 0x57):
	case (A32 | O32 | 0x50):	case (A32 | O32 | 0x51):
	case (A32 | O32 | 0x52):	case (A32 | O32 | 0x53):
	case (A32 | O32 | 0x54):	case (A32 | O32 | 0x55):
	case (A32 | O32 | 0x56):	case (A32 | O32 | 0x57):
		{
			//No special-case code required for PUSH ESP
			Dword reg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0x06:		reg = REG_ES;	break;
			case 0x0e:		reg = REG_CS;	break;
			case 0x16:		reg = REG_SS;	break;
			case 0x1e:		reg = REG_DS;	break;
			case 0x0fa0:	reg = REG_FS;	break;
			case 0x0fa8:	reg = REG_GS;	break;
			case 0x9c:		break;
			default:		reg = REG_EAX + (opcode & 0x07);
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
																//Sub eSI, 4
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 4));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

																//If segment register push
			if(	reg == REG_CS || reg == REG_DS || reg == REG_ES || reg == REG_SS ||
				reg == REG_FS || reg == REG_GS)
			{
				AddOp(TYPE_MOV, HOST_32, REG_EBX, IMM_32, 0));			//Mov EBX, 0
				AddOp(TYPE_GETSEG, HOST_16, REG_EBX, GUEST_SEG, reg));	//GetSeg BX, reg
			}
			else												//Else
				AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, reg));		//Get EBX, reg

			AddOp(TYPE_WRITE, HOST_32, REG_EDI, HOST_32, REG_EBX));	//Write [EDI], EBX
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	case 0xff30:			case (A32 | 0xff30):		//PUSH Mw
	case (O32 | 0xff30):	case (A32 | O32 | 0xff30):	//PUSH Md
		{
			//Effective address => EBX
			Dword Gw;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gw))
			{
				return false;
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			OperandType hostType = HOST_16;
			Dword stackOffset = 2;
			if((opcode & O32) != 0)
			{
				hostType = HOST_32;
				stackOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
																	//Sub eSI, SOFF
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, stackOffset));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EDI, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EAX));

			AddOp(TYPE_READ, hostType, REG_EAX, HOST_32, REG_EBX));	//Read eAX, [EBX]
			AddOp(TYPE_WRITE, HOST_32, REG_EDI, hostType, REG_EAX));//Write [EDI], eAX
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	case 0x68: case (A32 | 0x68):	//PUSH Iw
	case 0x6a: case (A32 | 0x6a):	//PUSH Ib (O16)
		{
			Word Iw;
			if(opcode == 0x68 || opcode == (A32 | 0x68))
			{
				if(!ReadCodeWord(r_eip, Iw))
					return false;
				r_eip += 2;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Iw = static_cast<SignedByte>(Ib);
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
																	//Sub eSI, 2
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 2));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EDI, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EAX));

			AddOp(TYPE_WRITE, HOST_32, REG_EDI, IMM_16, Iw));		//Write [EDI], Iw
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	case (O32 | 0x68): case (A32 | O32 | 0x68):	//PUSH Id
	case (O32 | 0x6a): case (A32 | O32 | 0x6a):	//PUSH Ib (O32)
		{
			Dword Id;
			if(opcode == (O32 | 0x68) || opcode == (A32 | O32 | 0x68))
			{
				guestImmDwordOffset = r_eip - startEip;
				if(!ReadCodeDword(r_eip, Id))
					return false;
				r_eip += 4;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Id = static_cast<SignedByte>(Ib);
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
																	//Sub eSI, 4
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 4));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EDI, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EAX));

			if(opcode == (O32 | 0x68) || opcode == (A32 | O32 | 0x68))	//Write [EDI], Id
				AddOpId(TYPE_WRITE, HOST_32, REG_EDI, IMM_32, Id));
			else
				AddOp(TYPE_WRITE, HOST_32, REG_EDI, IMM_32, Id));

			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	//POP ES, CS, DS, FS, GS (O16)
	case 0x07:				case 0x0f:				case 0x1f:
	case 0x0fa1:			case 0x0fa9:
	case (A32 | 0x07):		case (A32 | 0x0f):		case (A32 | 0x1f):
	case (A32 | 0x0fa1):	case (A32 | 0x0fa9):
	//POP AX, CX, DX, BX, SP, BP, SI, DI
	case 0x58:			case 0x59:			case 0x5a:			case 0x5b:
	case 0x5c:			case 0x5d:			case 0x5e:			case 0x5f:
	case (A32 | 0x58):	case (A32 | 0x59):	case (A32 | 0x5a):	case (A32 | 0x5b):
	case (A32 | 0x5c):	case (A32 | 0x5d):	case (A32 | 0x5e):	case (A32 | 0x5f):
		{
			Dword reg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0x07:		reg = REG_ES;	break;
			case 0x0f:		reg = REG_CS;	break;
			case 0x1f:		reg = REG_DS;	break;
			case 0x0fa1:	reg = REG_FS;	break;
			case 0x0fa9:	reg = REG_GS;	break;
			default:		reg = REG_EAX + (opcode & 0x07);
			}

			if(reg == REG_DS || reg == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
			AddOp(stackMovType, HOST_32, REG_EDX, stackHostType, REG_ESI));	//SMov EDX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDX, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDX, HOST_32, REG_EBX));

																//If not segment register pop
			if(	reg != REG_CS && reg != REG_DS && reg != REG_ES && reg != REG_SS &&
				reg != REG_FS && reg != REG_GS)
			{
				AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, reg));	//Get EBX, reg
			}

			AddOp(TYPE_READ, HOST_16, REG_EBX, HOST_32, REG_EDX));	//Read BX, [EDX]

																//If segment register pop
			if(	reg == REG_CS || reg == REG_DS || reg == REG_ES || reg == REG_FS ||
				reg == REG_GS)
			{
				AddOp(TYPE_PUTSEG, GUEST_SEG, reg, HOST_16, REG_EBX));	//PutSeg reg, BX
			}
			else												//Else
				AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_EBX));		//Put reg, EBX

			if(reg != REG_ESP)									//If not POP SP
			{
																	//Add eSI, 2
				AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, 2));
				AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI
			}

			break;
		}

	//POP ES, CS, DS, FS, GS (O32)
	case (O32 | 0x07):			case (O32 | 0x0f):			case (O32 | 0x1f):
	case (O32 | 0x0fa1):		case (O32 | 0x0fa9):
	case (A32 | O32 | 0x07):	case (A32 | O32 | 0x0f):	case (A32 | O32 | 0x1f):
	case (A32 | O32 | 0x0fa1):	case (A32 | O32 | 0x0fa9):
	//POP EAX, ECX, EDX, EBX, ESP, EBP, ESI, EDI
	case (O32 | 0x58):			case (O32 | 0x59):
	case (O32 | 0x5a):			case (O32 | 0x5b):
	case (O32 | 0x5c):			case (O32 | 0x5d):
	case (O32 | 0x5e):			case (O32 | 0x5f):
	case (A32 | O32 | 0x58):	case (A32 | O32 | 0x59):
	case (A32 | O32 | 0x5a):	case (A32 | O32 | 0x5b):
	case (A32 | O32 | 0x5c):	case (A32 | O32 | 0x5d):
	case (A32 | O32 | 0x5e):	case (A32 | O32 | 0x5f):
		{
			Dword reg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0x07:		reg = REG_ES;	break;
			case 0x0f:		reg = REG_CS;	break;
			case 0x1f:		reg = REG_DS;	break;
			case 0x0fa1:	reg = REG_FS;	break;
			case 0x0fa9:	reg = REG_GS;	break;
			default:		reg = REG_EAX + (opcode & 0x07);
			}

			if(reg == REG_DS || reg == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
			AddOp(stackMovType, HOST_32, REG_EDX, stackHostType, REG_ESI));	//SMov EDX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDX, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDX, HOST_32, REG_EBX));

			AddOp(TYPE_READ, HOST_32, REG_EBX, HOST_32, REG_EDX));	//Read EBX, [EDX]

																//If segment register pop
			if(	reg == REG_CS || reg == REG_DS || reg == REG_ES || reg == REG_FS ||
				reg == REG_GS)
			{
				AddOp(TYPE_PUTSEG, GUEST_SEG, reg, HOST_16, REG_EBX));	//PutSeg reg, BX
			}
			else												//Else
				AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_EBX));		//Put reg, EBX

			if(reg != REG_ESP)									//If not POP ESP
			{
																	//Add eSI, 4
				AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, 4));
				AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI
			}

			break;
		}

	case 0x8f00:			case (A32 | 0x8f00):		//POP Mw
	case (O32 | 0x8f00):	case (A32 | O32 | 0x8f00):	//POP Md
		{
			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;
			Dword stackOffset = ((opcode & O32) == 0) ? 2 : 4;

			Dword Gd, defSegRegBase;

			//Calculate the effective address
			if((opcode & A32) == 0)
			{
				
				Dword baseReg, indexReg, displacement;
				if(!DecodeModRMByteRegMemoryA16(r_eip, Gd, defSegRegBase, baseReg, indexReg,
												displacement))
				{
					return false;
				}

				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
				AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, indexReg));	//Get EDX, indexReg
				if(baseReg == REG_ESP)			//If baseReg == REG_ESP, add eCX, stackOffset
					AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ECX, stackImmType, stackOffset));
				if(indexReg == REG_ESP)			//If indexReg == REG_ESP, add eDX, stackOffset
					AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_EDX, stackImmType, stackOffset));
														//Lea16 EBX, [CX, DX, displacement]
				AddOp(TYPE_LEA16, HOST_32, REG_EBX, HOST_16, REG_ECX, HOST_16, REG_EDX, IMM_16, displacement));
			}
			else
			{
				Dword baseReg, indexReg, shift, displacement;
				if(!DecodeModRMByteRegMemoryA32(r_eip, Gd, defSegRegBase, baseReg, indexReg,
										shift, displacement))
				{
					return false;
				}

				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, baseReg));		//Get ECX, baseReg
				AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, indexReg));	//Get EDX, indexReg
				if(baseReg == REG_ESP)			//If baseReg == REG_ESP, add eCX, stackOffset
					AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ECX, stackImmType, stackOffset));
				if(indexReg == REG_ESP)			//If indexReg == REG_ESP, add eDX, stackOffset
					AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_EDX, stackImmType, stackOffset));
												//Lea32 EBX, [ECX, EDX, shift, displacement]
				AddOp(TYPE_LEA32, HOST_32, REG_EBX, HOST_32, REG_ECX, HOST_32, REG_EDX, IMM_8, shift, IMM_32, displacement));
			}

			//Calculate which segment register to use
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = defSegRegBase;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, segRegBase));				//Get ECX, SEG.BASE
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_ECX));	//Add EBX, ECX

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
			AddOp(stackMovType, HOST_32, REG_EDX, stackHostType, REG_ESI));	//SMov EDX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EDX, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDX, HOST_32, REG_EAX));
			
			AddOp(TYPE_READ, hostType, REG_EAX, HOST_32, REG_EDX));	//Read eAX, [EDX]
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EAX));//Write [EBX], eAX

																	//Add eSI, stackOffset
			AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, stackOffset));
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI

			break;
		}

	//XCHG Rb, Gb
	case 0x86c0: case (A32 | 0x86c0): case (O32 | 0x86c0): case (A32 | O32 | 0x86c0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Gb and Rb
			OperandType GbHostRegType, RbHostRegType;
			Dword Gb, Rb;
			Get8BitRegFromReg(Gd, GbHostRegType, Gb);
			Get8BitRegFromReg(Rd, RbHostRegType, Rb);

			if(Gb == Rb)
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));		//Get ECX, Gb_32
				AddOp(TYPE_MOV, HOST_L8, REG_EDX, GbHostRegType, REG_ECX));//Mov DL, ECX_GbTT
																	//Mov ECX_GbTT, ECX_RbTT
				AddOp(TYPE_MOV, GbHostRegType, REG_ECX, RbHostRegType, REG_ECX));
				AddOp(TYPE_MOV, RbHostRegType, REG_ECX, HOST_L8, REG_EDX));//Mov ECX_RbTT, DL
				AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));			//Put Gb_32, ECX
			}
			else
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));		//Get ECX, Gb_32
				AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rb));		//Get EDX, Rb_32
				AddOp(TYPE_MOV, HOST_L8, REG_EBX, GbHostRegType, REG_ECX));//Mov BL, ECX_GbTT
																	//Mov ECX_GbTT, EDX_RbTT
				AddOp(TYPE_MOV, GbHostRegType, REG_ECX, RbHostRegType, REG_EDX));
				AddOp(TYPE_MOV, RbHostRegType, REG_EDX, HOST_L8, REG_EBX));//Mov EDX_RbTT, BL
				AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));		//Put Gb_32, ECX
				AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_EDX));		//Put Rb_32, EDX
			}
			break;
		}

	//XCHG Mb, Gb
	case 0x8600: case (A32 | 0x8600): case (O32 | 0x8600): case (A32 | O32 | 0x8600):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));		//Read CL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gb));				//Get EDX, Gb_32
			AddOp(TYPE_MOV, HOST_32, REG_ESI, HOST_32, REG_ECX));		//Mov ESI, ECX
			AddOp(TYPE_MOV, HOST_32, REG_EDI, HOST_32, REG_EDX));		//Mov EDI, EDX
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostRegType, REG_EDX));	//Write [EBX], {DL|DH}
			AddOp(TYPE_MOV, HOST_32, REG_ECX, HOST_32, REG_ESI));		//Mov ECX, ESI
			AddOp(TYPE_MOV, HOST_32, REG_EDX, HOST_32, REG_EDI));		//Mov EDX, EDI
			AddOp(TYPE_MOV, hostRegType, REG_EDX, HOST_L8, REG_ECX));	//Mov {DL|DH}, CL
			AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_EDX));				//Put Gb_32, EDX

			break;
		}

	case 0x87c0:			case (A32 | 0x87c0):		//XCHG Rw, Gw
	case (O32 | 0x87c0):	case (A32 | O32 | 0x87c0):	//XCHG Rd, Gd
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));			//Get EDX, Rd
			AddOp(TYPE_MOV, hostType, REG_EBX, hostType, REG_ECX));	//Mov eBX, eCX
			AddOp(TYPE_MOV, hostType, REG_ECX, hostType, REG_EDX));	//Mov eCX, eDX
			AddOp(TYPE_MOV, hostType, REG_EDX, hostType, REG_EBX));	//Mov eDX, eBX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));			//Put Rd, EDX

			break;
		}

	case 0x8700:			case (A32 | 0x8700):		//XCHG Mw, Gw
	case (O32 | 0x8700):	case (A32 | O32 | 0x8700):	//XCHG Md, Gd
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_READ, hostType, REG_ESI, HOST_32, REG_EBX));	//Read eSI, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, Gd));			//Get EDI, Gd
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EDI));//Write [EBX], eDI
			AddOp(TYPE_MOV, hostType, REG_EDI, hostType, REG_ESI));	//Mov eDI, eSI
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EDI));			//Put Gd, EDI

			break;
		}

	//XCHG AX, regw
	case 0x91:	case 0x92:	case 0x93:	case 0x94:	case 0x95:	case 0x96:	case 0x97:
	case (A32 | 0x91):	case (A32 | 0x92):	case (A32 | 0x93):	case (A32 | 0x94):
	case (A32 | 0x95):	case (A32 | 0x96):	case (A32 | 0x97):
	//XCHG EAX, regd
	case (O32 | 0x91):	case (O32 | 0x92):	case (O32 | 0x93):	case (O32 | 0x94):
	case (O32 | 0x95):	case (O32 | 0x96):	case (O32 | 0x97):
	case (A32 | O32 | 0x91):	case (A32 | O32 | 0x92):	case (A32 | O32 | 0x93):
	case (A32 | O32 | 0x94):	case (A32 | O32 | 0x95):	case (A32 | O32 | 0x96):
	case (A32 | O32 | 0x97):
		{
			instructionMayCauseException = false;

			Dword regw = opcode & 0x07;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, regw));		//Get EDX, regw_32
			AddOp(TYPE_MOV, hostType, REG_EBX, hostType, REG_ECX));	//Mov eBX, eCX
			AddOp(TYPE_MOV, hostType, REG_ECX, hostType, REG_EDX));	//Mov eCX, eDX
			AddOp(TYPE_MOV, hostType, REG_EDX, hostType, REG_EBX));	//Mov eDX, eBX
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_ECX));		//Put gEAX, ECX
			AddOp(TYPE_PUT, GUEST, regw, HOST_32, REG_EDX));		//Put regw_32, EDX

			break;
		}

	//XADD Rb, Gb
	case 0x0fc0c0: case (A32 | 0x0fc0c0): case (O32 | 0x0fc0c0): case (A32 | O32 | 0x0fc0c0):
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Gb and Rb
			OperandType GbHostRegType, RbHostRegType;
			Dword Gb, Rb;
			Get8BitRegFromReg(Gd, GbHostRegType, Gb);
			Get8BitRegFromReg(Rd, RbHostRegType, Rb);

			if(Gb == Rb)
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));		//Get ECX, Gb_32
				AddOp(TYPE_MOV, HOST_L8, REG_EDX, GbHostRegType, REG_ECX));//Mov DL, ECX_GbTT
																	//Mov ECX_GbTT, ECX_RbTT
				AddOp(TYPE_MOV, GbHostRegType, REG_ECX, RbHostRegType, REG_ECX));
																	//Add ECX_RbTT, DL [F]
				AddOp(TYPE_ALU, ALU_FUNC_ADD, 0, ALLF, RbHostRegType, REG_ECX, HOST_L8, REG_EDX));
				AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));		//Put Gb_32, ECX
			}
			else
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));		//Get ECX, Gb_32
				AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rb));		//Get EDX, Rb_32
				AddOp(TYPE_MOV, HOST_L8, REG_EBX, GbHostRegType, REG_ECX));//Mov BL, ECX_GbTT
																	//Mov ECX_GbTT, EDX_RbTT
				AddOp(TYPE_MOV, GbHostRegType, REG_ECX, RbHostRegType, REG_EDX));
																	//Add EDX_RbTT, BL [F]
				AddOp(TYPE_ALU, ALU_FUNC_ADD, 0, ALLF, RbHostRegType, REG_EDX, HOST_L8, REG_EBX));
				AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));		//Put Gb_32, ECX
				AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_EDX));		//Put Rb_32, EDX
			}
			break;
		}

	//XADD Mb, Gb
	case 0x0fc000: case (A32 | 0x0fc000): case (O32 | 0x0fc000): case (A32 | O32 | 0x0fc000):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));		//Read CL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gb));				//Get EDX, Gb_32
																		//Add {DL|DH}, CL [F]
			AddOp(TYPE_ALU, ALU_FUNC_ADD, 0, ALLF, hostRegType, REG_EDX, HOST_L8, REG_ECX));
			AddOp(TYPE_MOV, HOST_32, REG_ESI, HOST_32, REG_ECX));		//Mov ESI, ECX
			AddOp(TYPE_MOV, HOST_32, REG_EDI, HOST_32, REG_EDX));		//Mov EDI, EDX
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostRegType, REG_EDX));	//Write [EBX], {DL|DH}
			AddOp(TYPE_MOV, HOST_32, REG_ECX, HOST_32, REG_ESI));		//Mov ECX, ESI
			AddOp(TYPE_MOV, HOST_32, REG_EDX, HOST_32, REG_EDI));		//Mov EDX, EDI
			AddOp(TYPE_MOV, hostRegType, REG_EDX, HOST_L8, REG_ECX));	//Mov {DL|DH}, CL
			AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_EDX));				//Put Gb_32, EDX

			break;
		}

	case 0x0fc1c0:			case (A32 | 0x0fc1c0):			//XADD Rw, Gw
	case (O32 | 0x0fc1c0):	case (A32 | O32 | 0x0fc1c0):	//XADD Rd, Gd
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));			//Get EDX, Rd
			AddOp(TYPE_MOV, hostType, REG_EBX, hostType, REG_ECX));	//Mov eBX, eCX
			AddOp(TYPE_MOV, hostType, REG_ECX, hostType, REG_EDX));	//Mov eCX, eDX
																	//Add eDX, eBX [F]
			AddOp(TYPE_ALU, ALU_FUNC_ADD, 0, ALLF, hostType, REG_EDX, hostType, REG_EBX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));			//Put Rd, EDX

			break;
		}

	case 0x0fc100:			case (A32 | 0x0fc100):			//XADD Mw, Gw
	case (O32 | 0x0fc100):	case (A32 | O32 | 0x0fc100):	//XADD Md, Gd
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_READ, hostType, REG_ESI, HOST_32, REG_EBX));	//Read eSI, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, Gd));			//Get EDI, Gd
																	//Add eDI, eSI [F]
			AddOp(TYPE_ALU, ALU_FUNC_ADD, 0, ALLF, hostType, REG_EDI, hostType, REG_ESI));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EDI));//Write [EBX], eDI
			AddOp(TYPE_MOV, hostType, REG_EDI, hostType, REG_ESI));	//Mov eDI, eSI
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EDI));			//Put Gd, EDI

			break;
		}

	//BINARY ARITHMETIC INSTRUCTIONS

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Mb, Gb
	case 0x0000: case (A32 | 0x0000): case (O32 | 0x0000): case (A32 | O32 | 0x0000):
	case 0x0800: case (A32 | 0x0800): case (O32 | 0x0800): case (A32 | O32 | 0x0800):
	case 0x1000: case (A32 | 0x1000): case (O32 | 0x1000): case (A32 | O32 | 0x1000):
	case 0x1800: case (A32 | 0x1800): case (O32 | 0x1800): case (A32 | O32 | 0x1800):
	case 0x2000: case (A32 | 0x2000): case (O32 | 0x2000): case (A32 | O32 | 0x2000):
	case 0x2800: case (A32 | 0x2800): case (O32 | 0x2800): case (A32 | O32 | 0x2800):
	case 0x3000: case (A32 | 0x3000): case (O32 | 0x3000): case (A32 | O32 | 0x3000):
	case 0x3800: case (A32 | 0x3800): case (O32 | 0x3800): case (A32 | O32 | 0x3800):
	//TEST Mb, Gb
	case 0x8400: case (A32 | 0x8400): case (O32 | 0x8400): case (A32 | O32 | 0x8400):
		{
			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0x8400)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x3800) >> 11;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));		//Read AL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));				//Get ECX, Gb_32

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)		//Alu AL, {CL|CH} [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_L8, REG_EAX, hostRegType, REG_ECX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_L8, REG_EAX, hostRegType, REG_ECX));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_EAX));	//Write [EBX], AL

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Mw, Gw
	case 0x0100:	case (A32 | 0x0100):	case 0x0900:	case (A32 | 0x0900):
	case 0x1100:	case (A32 | 0x1100):	case 0x1900:	case (A32 | 0x1900):
	case 0x2100:	case (A32 | 0x2100):	case 0x2900:	case (A32 | 0x2900):
	case 0x3100:	case (A32 | 0x3100):	case 0x3900:	case (A32 | 0x3900):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Md, Gd
	case (O32 | 0x0100):	case (A32 | O32 | 0x0100):
	case (O32 | 0x0900):	case (A32 | O32 | 0x0900):
	case (O32 | 0x1100):	case (A32 | O32 | 0x1100):
	case (O32 | 0x1900):	case (A32 | O32 | 0x1900):
	case (O32 | 0x2100):	case (A32 | O32 | 0x2100):
	case (O32 | 0x2900):	case (A32 | O32 | 0x2900):
	case (O32 | 0x3100):	case (A32 | O32 | 0x3100):
	case (O32 | 0x3900):	case (A32 | O32 | 0x3900):
	//TEST Mw, Gw
	case 0x8500:			case (A32 | 0x8500):
	//TEST Md, Gd
	case (O32 | 0x8500):	case (A32 | O32 | 0x8500):
		{
			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0x8500)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x3800) >> 11;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, hostType, REG_EAX, HOST_32, REG_EBX));	//Read eAX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu eAX, eCX [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_EAX, hostType, REG_ECX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_EAX, hostType, REG_ECX));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EAX));//Write [EBX], eAX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rw, Gw
	case 0x01c0:	case (A32 | 0x01c0):	case 0x09c0:	case (A32 | 0x09c0):
	case 0x11c0:	case (A32 | 0x11c0):	case 0x19c0:	case (A32 | 0x19c0):
	case 0x21c0:	case (A32 | 0x21c0):	case 0x29c0:	case (A32 | 0x29c0):
	case 0x31c0:	case (A32 | 0x31c0):	case 0x39c0:	case (A32 | 0x39c0):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rd, Gd
	case (O32 | 0x01c0):	case (A32 | O32 | 0x01c0):
	case (O32 | 0x09c0):	case (A32 | O32 | 0x09c0):
	case (O32 | 0x11c0):	case (A32 | O32 | 0x11c0):
	case (O32 | 0x19c0):	case (A32 | O32 | 0x19c0):
	case (O32 | 0x21c0):	case (A32 | O32 | 0x21c0):
	case (O32 | 0x29c0):	case (A32 | O32 | 0x29c0):
	case (O32 | 0x31c0):	case (A32 | O32 | 0x31c0):
	case (O32 | 0x39c0):	case (A32 | O32 | 0x39c0):
	//TEST Rw, Gw
	case 0x85c0:			case (A32 | 0x85c0):
	//TEST Rd, Gd
	case (O32 | 0x85c0):	case (A32 | O32 | 0x85c0):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0x85c0)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x3800) >> 11;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));		//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gd));		//Get EDX, Gd

																//Alu eCX, eDX [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_ECX, hostType, REG_EDX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));	//Put Rd, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP AL, Ib
	case 0x04:	case (A32 | 0x04):	case (O32 | 0x04):	case (A32 | O32 | 0x04):
	case 0x0c:	case (A32 | 0x0c):	case (O32 | 0x0c):	case (A32 | O32 | 0x0c):
	case 0x14:	case (A32 | 0x14):	case (O32 | 0x14):	case (A32 | O32 | 0x14):
	case 0x1c:	case (A32 | 0x1c):	case (O32 | 0x1c):	case (A32 | O32 | 0x1c):
	case 0x24:	case (A32 | 0x24):	case (O32 | 0x24):	case (A32 | O32 | 0x24):
	case 0x2c:	case (A32 | 0x2c):	case (O32 | 0x2c):	case (A32 | O32 | 0x2c):
	case 0x34:	case (A32 | 0x34):	case (O32 | 0x34):	case (A32 | O32 | 0x34):
	case 0x3c:	case (A32 | 0x3c):	case (O32 | 0x3c):	case (A32 | O32 | 0x3c):
	//TEST AL, Ib
	case 0xa8:	case (A32 | 0xa8):	case (O32 | 0xa8):	case (A32 | O32 | 0xa8):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xa8)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu AL, Ib [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_L8, REG_EAX, IMM_8, Ib));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_L8, REG_EAX, IMM_8, Ib));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));	//Put gEAX, EAX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP AX, Iw
	case 0x05:	case (A32 | 0x05):	case 0x0d:	case (A32 | 0x0d):
	case 0x15:	case (A32 | 0x15):	case 0x1d:	case (A32 | 0x1d):
	case 0x25:	case (A32 | 0x25):	case 0x2d:	case (A32 | 0x2d):
	case 0x35:	case (A32 | 0x35):	case 0x3d:	case (A32 | 0x3d):
	//TEST AX, Iw
	case 0xa9:	case (A32 | 0xa9):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xa9)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Word Iw;
			if(!ReadCodeWord(r_eip, Iw))
				return false;
			r_eip += 2;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));	//Get EAX, gEAX

																//Alu AX, Iw [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_16, REG_EAX, IMM_16, Iw));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_16, REG_EAX, IMM_16, Iw));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));	//Put gEAX, EAX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP EAX, Id
	case (O32 | 0x05):	case (A32 | O32 | 0x05):
	case (O32 | 0x0d):	case (A32 | O32 | 0x0d):
	case (O32 | 0x15):	case (A32 | O32 | 0x15):
	case (O32 | 0x1d):	case (A32 | O32 | 0x1d):
	case (O32 | 0x25):	case (A32 | O32 | 0x25):
	case (O32 | 0x2d):	case (A32 | O32 | 0x2d):
	case (O32 | 0x35):	case (A32 | O32 | 0x35):
	case (O32 | 0x3d):	case (A32 | O32 | 0x3d):
	//TEST EAX, Id
	case (O32 | 0xa9):	case (A32 | O32 | 0xa9):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xa9)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Dword Id;
			guestImmDwordOffset = r_eip - startEip;
			if(!ReadCodeDword(r_eip, Id))
				return false;
			r_eip += 4;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu EAX, Id [F]
				AddOpId(TYPE_ALU, aluFunc, CF, ALLF, HOST_32, REG_EAX, IMM_32, Id));
			else
				AddOpId(TYPE_ALU, aluFunc, 0, ALLF, HOST_32, REG_EAX, IMM_32, Id));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));	//Put gEAX, EAX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rb, Gb
	case 0x00c0: case (A32 | 0x00c0): case (O32 | 0x00c0): case (A32 | O32 | 0x00c0):
	case 0x08c0: case (A32 | 0x08c0): case (O32 | 0x08c0): case (A32 | O32 | 0x08c0):
	case 0x10c0: case (A32 | 0x10c0): case (O32 | 0x10c0): case (A32 | O32 | 0x10c0):
	case 0x18c0: case (A32 | 0x18c0): case (O32 | 0x18c0): case (A32 | O32 | 0x18c0):
	case 0x20c0: case (A32 | 0x20c0): case (O32 | 0x20c0): case (A32 | O32 | 0x20c0):
	case 0x28c0: case (A32 | 0x28c0): case (O32 | 0x28c0): case (A32 | O32 | 0x28c0):
	case 0x30c0: case (A32 | 0x30c0): case (O32 | 0x30c0): case (A32 | O32 | 0x30c0):
	case 0x38c0: case (A32 | 0x38c0): case (O32 | 0x38c0): case (A32 | O32 | 0x38c0):
	//TEST Rb, Gb
	case 0x84c0: case (A32 | 0x84c0): case (O32 | 0x84c0): case (A32 | O32 | 0x84c0):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gb, Rb
	case 0x02c0: case (A32 | 0x02c0): case (O32 | 0x02c0): case (A32 | O32 | 0x02c0):
	case 0x0ac0: case (A32 | 0x0ac0): case (O32 | 0x0ac0): case (A32 | O32 | 0x0ac0):
	case 0x12c0: case (A32 | 0x12c0): case (O32 | 0x12c0): case (A32 | O32 | 0x12c0):
	case 0x1ac0: case (A32 | 0x1ac0): case (O32 | 0x1ac0): case (A32 | O32 | 0x1ac0):
	case 0x22c0: case (A32 | 0x22c0): case (O32 | 0x22c0): case (A32 | O32 | 0x22c0):
	case 0x2ac0: case (A32 | 0x2ac0): case (O32 | 0x2ac0): case (A32 | O32 | 0x2ac0):
	case 0x32c0: case (A32 | 0x32c0): case (O32 | 0x32c0): case (A32 | O32 | 0x32c0):
	case 0x3ac0: case (A32 | 0x3ac0): case (O32 | 0x3ac0): case (A32 | O32 | 0x3ac0):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0x84c0)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x3800) >> 11;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//If a "Gb, Rb" instruction, swap Gd and Rd
			if((opcode & 0x0700) == 0x0200)
				std::swap(Gd, Rd);

			//Calculate Gb and Rb
			OperandType GbHostRegType, RbHostRegType;
			Dword Gb, Rb;
			Get8BitRegFromReg(Gd, GbHostRegType, Gb);
			Get8BitRegFromReg(Rd, RbHostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));		//Get ECX, Rb_32
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gb));		//Get EDX, Gb_32

																//Alu {CL|CH}, {DL|DH} [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, RbHostRegType, REG_ECX, GbHostRegType, REG_EDX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, RbHostRegType, REG_ECX, GbHostRegType, REG_EDX));

			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_ECX));		//Put Rb_32, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gb, Mb
	case 0x0200: case (A32 | 0x0200): case (O32 | 0x0200): case (A32 | O32 | 0x0200):
	case 0x0a00: case (A32 | 0x0a00): case (O32 | 0x0a00): case (A32 | O32 | 0x0a00):
	case 0x1200: case (A32 | 0x1200): case (O32 | 0x1200): case (A32 | O32 | 0x1200):
	case 0x1a00: case (A32 | 0x1a00): case (O32 | 0x1a00): case (A32 | O32 | 0x1a00):
	case 0x2200: case (A32 | 0x2200): case (O32 | 0x2200): case (A32 | O32 | 0x2200):
	case 0x2a00: case (A32 | 0x2a00): case (O32 | 0x2a00): case (A32 | O32 | 0x2a00):
	case 0x3200: case (A32 | 0x3200): case (O32 | 0x3200): case (A32 | O32 | 0x3200):
	case 0x3a00: case (A32 | 0x3a00): case (O32 | 0x3a00): case (A32 | O32 | 0x3a00):
		{
			Dword aluFunc = (opcode & 0x3800) >> 11;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate Gb and Rb
			OperandType hostRegType;
			Dword Gb;
			Get8BitRegFromReg(Gd, hostRegType, Gb);

			AddOp(TYPE_READ, HOST_L8, REG_EDX, HOST_32, REG_EBX));	//Read DL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gb));			//Get ECX, Gb_32
			
																	//Alu {CH|CL}, DL [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostRegType, REG_ECX, HOST_L8, REG_EDX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostRegType, REG_ECX, HOST_L8, REG_EDX));

			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_PUT, GUEST, Gb, HOST_32, REG_ECX));		//Put Gb_32, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gw, Mw
	case 0x0300:	case (A32 | 0x0300):	case 0x0b00:	case (A32 | 0x0b00):
	case 0x1300:	case (A32 | 0x1300):	case 0x1b00:	case (A32 | 0x1b00):
	case 0x2300:	case (A32 | 0x2300):	case 0x2b00:	case (A32 | 0x2b00):
	case 0x3300:	case (A32 | 0x3300):	case 0x3b00:	case (A32 | 0x3b00):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gd, Md
	case (O32 | 0x0300):	case (A32 | O32 | 0x0300):
	case (O32 | 0x0b00):	case (A32 | O32 | 0x0b00):
	case (O32 | 0x1300):	case (A32 | O32 | 0x1300):
	case (O32 | 0x1b00):	case (A32 | O32 | 0x1b00):
	case (O32 | 0x2300):	case (A32 | O32 | 0x2300):
	case (O32 | 0x2b00):	case (A32 | O32 | 0x2b00):
	case (O32 | 0x3300):	case (A32 | O32 | 0x3300):
	case (O32 | 0x3b00):	case (A32 | O32 | 0x3b00):
		{
			Dword aluFunc = (opcode & 0x3800) >> 11;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, hostType, REG_EDX, HOST_32, REG_EBX));	//Read eDX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd

																	//Alu eCX, eDX [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_ECX, hostType, REG_EDX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));

			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));		//Put Gd, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gw, Rw
	case 0x03c0:	case (A32 | 0x03c0):	case 0x0bc0:	case (A32 | 0x0bc0):
	case 0x13c0:	case (A32 | 0x13c0):	case 0x1bc0:	case (A32 | 0x1bc0):
	case 0x23c0:	case (A32 | 0x23c0):	case 0x2bc0:	case (A32 | 0x2bc0):
	case 0x33c0:	case (A32 | 0x33c0):	case 0x3bc0:	case (A32 | 0x3bc0):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Gd, Rd
	case (O32 | 0x03c0):	case (A32 | O32 | 0x03c0):
	case (O32 | 0x0bc0):	case (A32 | O32 | 0x0bc0):
	case (O32 | 0x13c0):	case (A32 | O32 | 0x13c0):
	case (O32 | 0x1bc0):	case (A32 | O32 | 0x1bc0):
	case (O32 | 0x23c0):	case (A32 | O32 | 0x23c0):
	case (O32 | 0x2bc0):	case (A32 | O32 | 0x2bc0):
	case (O32 | 0x33c0):	case (A32 | O32 | 0x33c0):
	case (O32 | 0x3bc0):	case (A32 | O32 | 0x3bc0):
		{
			instructionMayCauseException = false;

			Dword aluFunc = (opcode & 0x3800) >> 11;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));		//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));		//Get EDX, Rd

																//Alu eCX, eDX [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_ECX, hostType, REG_EDX));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));

			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));		//Put Gd, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Mb, Ib
	case 0x8000:	case (A32 | 0x8000):	case (O32 | 0x8000):	case (A32 | O32 | 0x8000):
	case 0x8200:	case (A32 | 0x8200):	case (O32 | 0x8200):	case (A32 | O32 | 0x8200):
	case 0x8008:	case (A32 | 0x8008):	case (O32 | 0x8008):	case (A32 | O32 | 0x8008):
	case 0x8208:	case (A32 | 0x8208):	case (O32 | 0x8208):	case (A32 | O32 | 0x8208):
	case 0x8010:	case (A32 | 0x8010):	case (O32 | 0x8010):	case (A32 | O32 | 0x8010):
	case 0x8210:	case (A32 | 0x8210):	case (O32 | 0x8210):	case (A32 | O32 | 0x8210):
	case 0x8018:	case (A32 | 0x8018):	case (O32 | 0x8018):	case (A32 | O32 | 0x8018):
	case 0x8218:	case (A32 | 0x8218):	case (O32 | 0x8218):	case (A32 | O32 | 0x8218):
	case 0x8020:	case (A32 | 0x8020):	case (O32 | 0x8020):	case (A32 | O32 | 0x8020):
	case 0x8220:	case (A32 | 0x8220):	case (O32 | 0x8220):	case (A32 | O32 | 0x8220):
	case 0x8028:	case (A32 | 0x8028):	case (O32 | 0x8028):	case (A32 | O32 | 0x8028):
	case 0x8228:	case (A32 | 0x8228):	case (O32 | 0x8228):	case (A32 | O32 | 0x8228):
	case 0x8030:	case (A32 | 0x8030):	case (O32 | 0x8030):	case (A32 | O32 | 0x8030):
	case 0x8230:	case (A32 | 0x8230):	case (O32 | 0x8230):	case (A32 | O32 | 0x8230):
	case 0x8038:	case (A32 | 0x8038):	case (O32 | 0x8038):	case (A32 | O32 | 0x8038):
	case 0x8238:	case (A32 | 0x8238):	case (O32 | 0x8238):	case (A32 | O32 | 0x8238):
	//TEST Mb, Ib
	case 0xf600:	case (A32 | 0xf600):	case (O32 | 0xf600):	case (A32 | O32 | 0xf600):
		{
			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xf600)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));	//Read AL, [EBX]

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu AL, Ib [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_L8, REG_EAX, IMM_8, Ib));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_L8, REG_EAX, IMM_8, Ib));

													//If not a "CMP" or "TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_EAX));	//Write [EBX], AL

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Mw, Iw
	case 0x8100:	case (A32 | 0x8100):	case 0x8108:	case (A32 | 0x8108):
	case 0x8110:	case (A32 | 0x8110):	case 0x8118:	case (A32 | 0x8118):
	case 0x8120:	case (A32 | 0x8120):	case 0x8128:	case (A32 | 0x8128):
	case 0x8130:	case (A32 | 0x8130):	case 0x8138:	case (A32 | 0x8138):
	//TEST Mw, Iw
	case 0xf700:	case (A32 | 0xf700):
		{
			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xf700)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Word Iw;
			if(!ReadCodeWord(r_eip, Iw))
				return false;
			r_eip += 2;

			AddOp(TYPE_READ, HOST_16, REG_ECX, HOST_32, REG_EBX));		//Read CX, [EBX]
			
																		//Alu CX, Iw [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_16, REG_ECX, IMM_16, Iw));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_16, REG_ECX, IMM_16, Iw));

													//If not a "CMP" or "TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_16, REG_ECX));	//Write [EBX], CX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Md, Id
	case (O32 | 0x8100):	case (A32 | O32 | 0x8100):
	case (O32 | 0x8108):	case (A32 | O32 | 0x8108):
	case (O32 | 0x8110):	case (A32 | O32 | 0x8110):
	case (O32 | 0x8118):	case (A32 | O32 | 0x8118):
	case (O32 | 0x8120):	case (A32 | O32 | 0x8120):
	case (O32 | 0x8128):	case (A32 | O32 | 0x8128):
	case (O32 | 0x8130):	case (A32 | O32 | 0x8130):
	case (O32 | 0x8138):	case (A32 | O32 | 0x8138):
	//TEST Md, Id
	case (O32 | 0xf700):	case (A32 | O32 | 0xf700):
		{
			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xf700)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Dword Id;
			guestImmDwordOffset = r_eip - startEip;
			if(!ReadCodeDword(r_eip, Id))
				return false;
			r_eip += 4;

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));	//Read ECX, [EBX]
			
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu ECX, Id [F]
				AddOpId(TYPE_ALU, aluFunc, CF, ALLF, HOST_32, REG_ECX, IMM_32, Id));
			else
				AddOpId(TYPE_ALU, aluFunc, 0, ALLF, HOST_32, REG_ECX, IMM_32, Id));

													//If not a "CMP" or "TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_32, REG_ECX));	//Write [EBX], ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Mw, Ib
	case 0x8300:	case (A32 | 0x8300):	case 0x8308:	case (A32 | 0x8308):
	case 0x8310:	case (A32 | 0x8310):	case 0x8318:	case (A32 | 0x8318):
	case 0x8320:	case (A32 | 0x8320):	case 0x8328:	case (A32 | 0x8328):
	case 0x8330:	case (A32 | 0x8330):	case 0x8338:	case (A32 | 0x8338):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Md, Ib
	case (O32 | 0x8300):	case (A32 | O32 | 0x8300):
	case (O32 | 0x8308):	case (A32 | O32 | 0x8308):
	case (O32 | 0x8310):	case (A32 | O32 | 0x8310):
	case (O32 | 0x8318):	case (A32 | O32 | 0x8318):
	case (O32 | 0x8320):	case (A32 | O32 | 0x8320):
	case (O32 | 0x8328):	case (A32 | O32 | 0x8328):
	case (O32 | 0x8330):	case (A32 | O32 | 0x8330):
	case (O32 | 0x8338):	case (A32 | O32 | 0x8338):
		{
			Dword aluFunc = (opcode & 0x38) >> 3;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;
			OperandType immType = ((opcode & O32) == 0) ? IMM_16 : IMM_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Read the immediate byte
			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_READ, hostType, REG_ECX, HOST_32, REG_EBX));	//Read eCX, [EBX]

															//Alu eCX, Ib (sign-extended) [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
			{
				AddOp(	TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_ECX, immType,
						static_cast<SignedByte>(Ib)));
			}
			else
			{
				AddOp(	TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_ECX, immType,
						static_cast<SignedByte>(Ib)));
			}
			
			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_ECX));//Write [EBX], eCX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rb, Ib
	case 0x80c0:	case (A32 | 0x80c0):	case (O32 | 0x80c0):	case (A32 | O32 | 0x80c0):
	case 0x82c0:	case (A32 | 0x82c0):	case (O32 | 0x82c0):	case (A32 | O32 | 0x82c0):
	case 0x80c8:	case (A32 | 0x80c8):	case (O32 | 0x80c8):	case (A32 | O32 | 0x80c8):
	case 0x82c8:	case (A32 | 0x82c8):	case (O32 | 0x82c8):	case (A32 | O32 | 0x82c8):
	case 0x80d0:	case (A32 | 0x80d0):	case (O32 | 0x80d0):	case (A32 | O32 | 0x80d0):
	case 0x82d0:	case (A32 | 0x82d0):	case (O32 | 0x82d0):	case (A32 | O32 | 0x82d0):
	case 0x80d8:	case (A32 | 0x80d8):	case (O32 | 0x80d8):	case (A32 | O32 | 0x80d8):
	case 0x82d8:	case (A32 | 0x82d8):	case (O32 | 0x82d8):	case (A32 | O32 | 0x82d8):
	case 0x80e0:	case (A32 | 0x80e0):	case (O32 | 0x80e0):	case (A32 | O32 | 0x80e0):
	case 0x82e0:	case (A32 | 0x82e0):	case (O32 | 0x82e0):	case (A32 | O32 | 0x82e0):
	case 0x80e8:	case (A32 | 0x80e8):	case (O32 | 0x80e8):	case (A32 | O32 | 0x80e8):
	case 0x82e8:	case (A32 | 0x82e8):	case (O32 | 0x82e8):	case (A32 | O32 | 0x82e8):
	case 0x80f0:	case (A32 | 0x80f0):	case (O32 | 0x80f0):	case (A32 | O32 | 0x80f0):
	case 0x82f0:	case (A32 | 0x82f0):	case (O32 | 0x82f0):	case (A32 | O32 | 0x82f0):
	case 0x80f8:	case (A32 | 0x80f8):	case (O32 | 0x80f8):	case (A32 | O32 | 0x80f8):
	case 0x82f8:	case (A32 | 0x82f8):	case (O32 | 0x82f8):	case (A32 | O32 | 0x82f8):
	//TEST Rb, Ib
	case 0xf6c0:	case (A32 | 0xf6c0):	case (O32 | 0xf6c0):	case (A32 | O32 | 0xf6c0):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32))== 0xf6c0)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));			//Get ECX, Rb_32

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu {CL|CH}, Ib [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, hostRegType, REG_ECX, IMM_8, Ib));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, hostRegType, REG_ECX, IMM_8, Ib));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_ECX));		//Put Rb_32, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rw, Ib
	case 0x83c0:	case (A32 | 0x83c0):	case 0x83c8:	case (A32 | 0x83c8):
	case 0x83d0:	case (A32 | 0x83d0):	case 0x83d8:	case (A32 | 0x83d8):
	case 0x83e0:	case (A32 | 0x83e0):	case 0x83e8:	case (A32 | 0x83e8):
	case 0x83f0:	case (A32 | 0x83f0):	case 0x83f8:	case (A32 | 0x83f8):
	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rd, Ib
	case (O32 | 0x83c0):	case (A32 | O32 | 0x83c0):
	case (O32 | 0x83c8):	case (A32 | O32 | 0x83c8):
	case (O32 | 0x83d0):	case (A32 | O32 | 0x83d0):
	case (O32 | 0x83d8):	case (A32 | O32 | 0x83d8):
	case (O32 | 0x83e0):	case (A32 | O32 | 0x83e0):
	case (O32 | 0x83e8):	case (A32 | O32 | 0x83e8):
	case (O32 | 0x83f0):	case (A32 | O32 | 0x83f0):
	case (O32 | 0x83f8):	case (A32 | O32 | 0x83f8):
		{
			instructionMayCauseException = false;

			Dword aluFunc = (opcode & 0x38) >> 3;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;
			OperandType immType = ((opcode & O32) == 0) ? IMM_16 : IMM_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Byte immByte;
			if(!ReadCodeByte(r_eip++, immByte))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));		//Get ECX, Rd

														//Alu eCX, Ib (sign-extended) [F]
			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)
			{
				AddOp(	TYPE_ALU, aluFunc, CF, ALLF, hostType, REG_ECX, immType,
						static_cast<SignedByte>(immByte)));
			}
			else
			{
				AddOp(	TYPE_ALU, aluFunc, 0, ALLF, hostType, REG_ECX, immType,
						static_cast<SignedByte>(immByte)));
			}

			if(aluFunc != ALU_FUNC_CMP)							//If not a "CMP" instruction
				AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));	//Put Rd, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rw, Iw
	case 0x81c0:	case (A32 | 0x81c0):	case 0x81c8:	case (A32 | 0x81c8):
	case 0x81d0:	case (A32 | 0x81d0):	case 0x81d8:	case (A32 | 0x81d8):
	case 0x81e0:	case (A32 | 0x81e0):	case 0x81e8:	case (A32 | 0x81e8):
	case 0x81f0:	case (A32 | 0x81f0):	case 0x81f8:	case (A32 | 0x81f8):
	//TEST Rw, Iw
	case 0xf7c0:	case (A32 | 0xf7c0):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xf7c0)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Word Iw;
			if(!ReadCodeWord(r_eip, Iw))
				return false;
			r_eip += 2;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu CX, Iw [F]
				AddOp(TYPE_ALU, aluFunc, CF, ALLF, HOST_16, REG_ECX, IMM_16, Iw));
			else
				AddOp(TYPE_ALU, aluFunc, 0, ALLF, HOST_16, REG_ECX, IMM_16, Iw));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	//ADD, OR, ADC, SBB, AND, SUB, XOR, CMP Rd, Id
	case (O32 | 0x81c0):	case (A32 | O32 | 0x81c0):
	case (O32 | 0x81c8):	case (A32 | O32 | 0x81c8):
	case (O32 | 0x81d0):	case (A32 | O32 | 0x81d0):
	case (O32 | 0x81d8):	case (A32 | O32 | 0x81d8):
	case (O32 | 0x81e0):	case (A32 | O32 | 0x81e0):
	case (O32 | 0x81e8):	case (A32 | O32 | 0x81e8):
	case (O32 | 0x81f0):	case (A32 | O32 | 0x81f0):
	case (O32 | 0x81f8):	case (A32 | O32 | 0x81f8):
	//TEST Rd, Id
	case (O32 | 0xf7c0):	case (A32 | O32 | 0xf7c0):
		{
			instructionMayCauseException = false;

			Dword aluFunc;
			if((opcode & ~(A32 | O32)) == 0xf7c0)
				aluFunc = ALU_FUNC_TEST;
			else
				aluFunc = (opcode & 0x38) >> 3;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Dword Id;
			guestImmDwordOffset = r_eip - startEip;
			if(!ReadCodeDword(r_eip, Id))
				return false;
			r_eip += 4;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd

			if(aluFunc == ALU_FUNC_ADC || aluFunc == ALU_FUNC_SBB)	//Alu ECX, Id [F]
				AddOpId(TYPE_ALU, aluFunc, CF, ALLF, HOST_32, REG_ECX, IMM_32, Id));
			else
				AddOpId(TYPE_ALU, aluFunc, 0, ALLF, HOST_32, REG_ECX, IMM_32, Id));

														//If not a "CMP"/"TEST" instruction
			if(aluFunc != ALU_FUNC_CMP && aluFunc != ALU_FUNC_TEST)
				AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	case 0xf710:	case (A32 | 0xf710):				//NOT Mw
	case 0xf718:	case (A32 | 0xf718):				//NEG Mw
	case 0xff00:	case (A32 | 0xff00):				//INC Mw
	case 0xff08:	case (A32 | 0xff08):				//DEC Mw
	case (O32 | 0xf710):	case (A32 | O32 | 0xf710):	//NOT Md
	case (O32 | 0xf718):	case (A32 | O32 | 0xf718):	//NEG Md
	case (O32 | 0xff00):	case (A32 | O32 | 0xff00):	//INC Md
	case (O32 | 0xff08):	case (A32 | O32 | 0xff08):	//DEC Md
		{
			Dword ualuFunc = FUNC_NONE;
			FlagSet flagsWritten = 0;
			switch(opcode & ~(A32 | O32))
			{
				case 0xf710: ualuFunc = UALU_FUNC_NOT; flagsWritten = 0;			break;
				case 0xf718: ualuFunc = UALU_FUNC_NEG; flagsWritten = ALLF;			break;
				case 0xff00: ualuFunc = UALU_FUNC_INC; flagsWritten = ALLF & ~CF;	break;
				case 0xff08: ualuFunc = UALU_FUNC_DEC; flagsWritten = ALLF & ~CF;	break;
			}
			assert(ualuFunc != FUNC_NONE);

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;
			
			AddOp(TYPE_READ, hostType, REG_ECX, HOST_32, REG_EBX));			//Read eCX, [EBX]
			AddOp(TYPE_UALU, ualuFunc, 0, flagsWritten, hostType, REG_ECX));//Ualu eCX [F]
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_ECX));		//Write [EBX], eCX

			break;
		}

	//INC Rw
	case 0x40:	case (A32 | 0x40):	case 0x41:	case (A32 | 0x41):
	case 0x42:	case (A32 | 0x42):	case 0x43:	case (A32 | 0x43):
	case 0x44:	case (A32 | 0x44):	case 0x45:	case (A32 | 0x45):
	case 0x46:	case (A32 | 0x46):	case 0x47:	case (A32 | 0x47):
	case 0xffc0:	case (A32 | 0xffc0):
	//DEC Rw
	case 0x48:	case (A32 | 0x48):	case 0x49:	case (A32 | 0x49):
	case 0x4a:	case (A32 | 0x4a):	case 0x4b:	case (A32 | 0x4b):
	case 0x4c:	case (A32 | 0x4c):	case 0x4d:	case (A32 | 0x4d):
	case 0x4e:	case (A32 | 0x4e):	case 0x4f:	case (A32 | 0x4f):
	case 0xffc8:	case (A32 | 0xffc8):
	//NOT Rw
	case 0xf7d0:	case (A32 | 0xf7d0):
	//NEG Rw
	case 0xf7d8:	case (A32 | 0xf7d8):
	//INC Rd
	case (O32 | 0x40):	case (A32 | O32 | 0x40):	case (O32 | 0x41):	case (A32 | O32 | 0x41):
	case (O32 | 0x42):	case (A32 | O32 | 0x42):	case (O32 | 0x43):	case (A32 | O32 | 0x43):
	case (O32 | 0x44):	case (A32 | O32 | 0x44):	case (O32 | 0x45):	case (A32 | O32 | 0x45):
	case (O32 | 0x46):	case (A32 | O32 | 0x46):	case (O32 | 0x47):	case (A32 | O32 | 0x47):
	case (O32 | 0xffc0):	case (A32 | O32 | 0xffc0):
	//DEC Rd
	case (O32 | 0x48):	case (A32 | O32 | 0x48):	case (O32 | 0x49):	case (A32 | O32 | 0x49):
	case (O32 | 0x4a):	case (A32 | O32 | 0x4a):	case (O32 | 0x4b):	case (A32 | O32 | 0x4b):
	case (O32 | 0x4c):	case (A32 | O32 | 0x4c):	case (O32 | 0x4d):	case (A32 | O32 | 0x4d):
	case (O32 | 0x4e):	case (A32 | O32 | 0x4e):	case (O32 | 0x4f):	case (A32 | O32 | 0x4f):
	case (O32 | 0xffc8):	case (A32 | O32 | 0xffc8):
	//NOT Rd
	case (O32 | 0xf7d0):	case (A32 | O32 | 0xf7d0):
	//NEG Rd
	case (O32 | 0xf7d8):	case (A32 | O32 | 0xf7d8):
		{
			instructionMayCauseException = false;

			Dword ualuFunc = FUNC_NONE;
			FlagSet flagsWritten = 0;
			switch(opcode & ~(A32 | O32))
			{
				case 0xf7d0: ualuFunc = UALU_FUNC_NOT; flagsWritten = 0;			break;
				case 0xf7d8: ualuFunc = UALU_FUNC_NEG; flagsWritten = ALLF;			break;
				case 0xffc0: ualuFunc = UALU_FUNC_INC; flagsWritten = ALLF & ~CF;	break;
				case 0xffc8: ualuFunc = UALU_FUNC_DEC; flagsWritten = ALLF & ~CF;	break;
				default:
					ualuFunc = ((opcode & 0x08) == 0) ? UALU_FUNC_INC : UALU_FUNC_DEC;
					flagsWritten = ALLF & ~CF;
			}
			assert(ualuFunc != FUNC_NONE);

			Dword reg;
			if(	(opcode & ~(A32 | O32)) == 0xf7d0 || (opcode & ~(A32 | O32)) == 0xf7d8 ||
				(opcode & ~(A32 | O32)) == 0xffc0 || (opcode & ~(A32 | O32)) == 0xffc8)
			{
				Dword Gd;
				DecodeModRMByteRegReg(r_eip, Gd, reg);
			}
			else
				reg = REG_EAX + (opcode & 0x07);

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;
			
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, reg));					//Get ECX, reg
			AddOp(TYPE_UALU, ualuFunc, 0, flagsWritten, hostType, REG_ECX));//Ualu eCX [F]
			AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_ECX));					//Put reg, ECX

			break;
		}

	case 0xfe00: case (A32 | 0xfe00): case (O32 | 0xfe00): case (A32 | O32 | 0xfe00): //INC Mb
	case 0xfe08: case (A32 | 0xfe08): case (O32 | 0xfe08): case (A32 | O32 | 0xfe08): //DEC Mb
	case 0xf610: case (A32 | 0xf610): case (O32 | 0xf610): case (A32 | O32 | 0xf610): //NOT Mb
	case 0xf618: case (A32 | 0xf618): case (O32 | 0xf618): case (A32 | O32 | 0xf618): //NEG Mb
		{
			Dword ualuFunc = FUNC_NONE;
			FlagSet flagsWritten = 0;
			switch(opcode & ~(A32 | O32))
			{
				case 0xfe00: ualuFunc = UALU_FUNC_INC; flagsWritten = ALLF & ~CF;	break;
				case 0xfe08: ualuFunc = UALU_FUNC_DEC; flagsWritten = ALLF & ~CF;	break;
				case 0xf610: ualuFunc = UALU_FUNC_NOT; flagsWritten = 0;			break;
				case 0xf618: ualuFunc = UALU_FUNC_NEG; flagsWritten = ALLF;			break;
			}
			assert(ualuFunc != FUNC_NONE);

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));		//Read CL, [EBX]
			AddOp(TYPE_UALU, ualuFunc, 0, flagsWritten, HOST_L8, REG_ECX));	//Ualu CL [F]
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_ECX));		//Write [EBX], CL

			break;
		}

	case 0xfec0: case (A32 | 0xfec0): case (O32 | 0xfec0): case (A32 | O32 | 0xfec0): //INC Rb
	case 0xfec8: case (A32 | 0xfec8): case (O32 | 0xfec8): case (A32 | O32 | 0xfec8): //DEC Rb
	case 0xf6d0: case (A32 | 0xf6d0): case (O32 | 0xf6d0): case (A32 | O32 | 0xf6d0): //NOT Rb
	case 0xf6d8: case (A32 | 0xf6d8): case (O32 | 0xf6d8): case (A32 | O32 | 0xf6d8): //NEG Rb
		{
			instructionMayCauseException = false;

			Dword ualuFunc = FUNC_NONE;
			FlagSet flagsWritten = 0;
			switch(opcode & ~(A32 | O32))
			{
				case 0xfec0: ualuFunc = UALU_FUNC_INC; flagsWritten = ALLF & ~CF;	break;
				case 0xfec8: ualuFunc = UALU_FUNC_DEC; flagsWritten = ALLF & ~CF;	break;
				case 0xf6d0: ualuFunc = UALU_FUNC_NOT; flagsWritten = 0;			break;
				case 0xf6d8: ualuFunc = UALU_FUNC_NEG; flagsWritten = ALLF;			break;
			}
			assert(ualuFunc != FUNC_NONE);

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);
			
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));				//Get ECX, Rb_32
			AddOp(TYPE_UALU, ualuFunc, 0, flagsWritten, hostRegType, REG_ECX));//Ualu eCX [F]
			AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_ECX));				//Put Rb_32, ECX

			break;
		}

	//MUL Rb
	case 0xf6e0: case (A32 | 0xf6e0): case (O32 | 0xf6e0): case (A32 | O32 | 0xf6e0):
	//IMUL Rb
	case 0xf6e8: case (A32 | 0xf6e8): case (O32 | 0xf6e8): case (A32 | O32 | 0xf6e8):
		{
			instructionMayCauseException = false;

			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf6e0) ? MUL_FUNC_MUL_16_8_8 :
							MUL_FUNC_IMUL_16_8_8;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));			//Get ECX, Rb_32
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
																	//Mul AX, AL, {CL|CH} [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_16, REG_EAX, HOST_L8, REG_EAX, hostRegType, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX

			break;
		}

	//MUL Mb
	case 0xf620: case (A32 | 0xf620): case (O32 | 0xf620): case (A32 | O32 | 0xf620):
	//IMUL Mb
	case 0xf628: case (A32 | 0xf628): case (O32 | 0xf628): case (A32 | O32 | 0xf628):
		{
			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf620) ? MUL_FUNC_MUL_16_8_8 :
							MUL_FUNC_IMUL_16_8_8;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));	//Read CL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
																	//Mul AX, AL, CL [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_16, REG_EAX, HOST_L8, REG_EAX, HOST_L8, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX

			break;
		}

	case 0xf720:	case (A32 | 0xf720):	//MUL Mw
	case 0xf728:	case (A32 | 0xf728):	//IMUL Mw
		{
			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf720) ? MUL_FUNC_MUL_32_16_16 :
							MUL_FUNC_IMUL_32_16_16;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_ECX, HOST_32, REG_EBX));	//Read CX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Mul DX:AX, AX, CX [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case 0xf7e0:	case (A32 | 0xf7e0):	//MUL Rw
	case 0xf7e8:	case (A32 | 0xf7e8):	//IMUL Rw
		{
			instructionMayCauseException = false;

			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf7e0) ? MUL_FUNC_MUL_32_16_16 :
							MUL_FUNC_IMUL_32_16_16;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Mul DX:AX, AX, CX [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case (O32 | 0xf7e0):	case (A32 | O32 | 0xf7e0):	//MUL Rd
	case (O32 | 0xf7e8):	case (A32 | O32 | 0xf7e8):	//IMUL Rd
		{
			instructionMayCauseException = false;

			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf7e0) ? MUL_FUNC_MUL_64_32_32 :
							MUL_FUNC_IMUL_64_32_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case (O32 | 0xf720):	case (A32 | O32 | 0xf720):	//MUL Md
	case (O32 | 0xf728):	case (A32 | O32 | 0xf728):	//IMUL Md
		{
			Dword mulFunc = ((opcode & ~(A32 | O32)) == 0xf720) ? MUL_FUNC_MUL_64_32_32 :
							MUL_FUNC_IMUL_64_32_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));	//Read ECX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(TYPE_MUL, mulFunc, 0, ALLF, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case 0x6b00:	case (A32 | 0x6b00):	//IMUL Gw, Mw, Ib
	case 0x6900:	case (A32 | 0x6900):	//IMUL Gw, Mw, Iw
		{
			//TODO: IMUL_FUNC_16_16_16 - no need to calculate the upper half
			//TODO: Using IMUL_32_16_16, are the flags correct?

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Word Iw;
			if(opcode == 0x6900 || opcode == (A32 | 0x6900))
			{
				if(!ReadCodeWord(r_eip, Iw))
					return false;
				r_eip += 2;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Iw = static_cast<SignedByte>(Ib);
			}

			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));	//Read AX, [EBX]
			AddOp(TYPE_MOV, HOST_16, REG_ECX, IMM_16, Iw));			//Mov CX, Iw
																	//Mul DX:AX, AX, CX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_32_16_16, 0, ALLF, HOST_16, REG_EDX,
					HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));	//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	case (O32 | 0x6b00):	case (A32 | O32 | 0x6b00):	//IMUL Gd, Md, Ib
	case (O32 | 0x6900):	case (A32 | O32 | 0x6900):	//IMUL Gd, Md, Id
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Dword Id;
			if(opcode == (O32 | 0x6900) || opcode == (A32 | O32 | 0x6900))
			{
				guestImmDwordOffset = r_eip - startEip;
				if(!ReadCodeDword(r_eip, Id))
					return false;
				r_eip += 4;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Id = static_cast<SignedByte>(Ib);
			}

			AddOp(TYPE_READ, HOST_32, REG_EAX, HOST_32, REG_EBX));	//Read EAX, [EBX]
			if(opcode == (O32 | 0x6900) || opcode == (A32 | O32 | 0x6900))	//Mov ECX, Id
				AddOpId(TYPE_MOV, HOST_32, REG_ECX, IMM_32, Id));
			else
				AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, Id));
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_64_32_32, 0, ALLF, HOST_32, REG_EDX,
					HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case 0x6bc0:	case (A32 | 0x6bc0):	//IMUL Gw, Rw, Ib
	case 0x69c0:	case (A32 | 0x69c0):	//IMUL Gw, Rw, Iw
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Word Iw;
			if(opcode == 0x69c0 || opcode == (A32 | 0x69c0))
			{
				if(!ReadCodeWord(r_eip, Iw))
					return false;
				r_eip += 2;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Iw = static_cast<SignedByte>(Ib);
			}

			AddOp(TYPE_MOV, HOST_16, REG_ECX, IMM_16, Iw));			//Mov CX, Iw
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Rd));			//Get EAX, Rd
																	//Mul DX:AX, AX, CX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_32_16_16, 0, ALLF, HOST_16, REG_EDX,
					HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));	//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	case (O32 | 0x6bc0):	case (A32 | O32 | 0x6bc0):	//IMUL Gd, Rd, Ib
	case (O32 | 0x69c0):	case (A32 | O32 | 0x69c0):	//IMUL Gd, Rd, Id
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Dword Id;
			if(opcode == (O32 | 0x69c0) || opcode == (A32 | O32 | 0x69c0))
			{
				guestImmDwordOffset = r_eip - startEip;
				if(!ReadCodeDword(r_eip, Id))
					return false;
				r_eip += 4;
			}
			else
			{
				Byte Ib;
				if(!ReadCodeByte(r_eip++, Ib))
					return false;

				Id = static_cast<SignedByte>(Ib);
			}

			if(opcode == (O32 | 0x69c0) || opcode == (A32 | O32 | 0x69c0))	//Mov ECX, Id
				AddOpId(TYPE_MOV, HOST_32, REG_ECX, IMM_32, Id));
			else
				AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, Id));

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Rd));			//Get EAX, Rd
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(TYPE_MUL, MUL_FUNC_IMUL_64_32_32, 0, ALLF, HOST_32, REG_EDX,
					HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case 0x0faf00:	case (A32 | 0x0faf00):	//IMUL Gw, Mw
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_ECX, HOST_32, REG_EBX));	//Read CX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));			//Get EAX, Gd
																	//Mul DX:AX, AX, CX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_32_16_16, 0, ALLF, HOST_16, REG_EDX,
					HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case (O32 | 0x0faf00):	case (A32 | O32 | 0x0faf00):	//IMUL Gd, Md
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));	//Read ECX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));			//Get EAX, Gd
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_64_32_32, 0, ALLF, HOST_32, REG_EDX,
					HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case 0x0fafc0:	case (A32 | 0x0fafc0):	//IMUL Gw, Rw
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));			//Get EAX, Gd
																	//Mul DX:AX, AX, CX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_32_16_16, 0, ALLF, HOST_16, REG_EDX,
					HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	case (O32 | 0x0fafc0):	case (A32 | O32 | 0x0fafc0):	//IMUL Gd, Rd
		{
			instructionMayCauseException = false;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));			//Get EAX, Gd
																	//Mul EDX:EAX, EAX, ECX [F]
			AddOp(	TYPE_MUL, MUL_FUNC_IMUL_64_32_32, 0, ALLF, HOST_32, REG_EDX,
					HOST_32, REG_EAX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EAX));			//Put Gd, EAX

			break;
		}

	//DIV Mb
	case 0xf630: case (A32 | 0xf630): case (O32 | 0xf630): case (A32 | O32 | 0xf630):
	//IDIV Mb
	case 0xf638: case (A32 | 0xf638): case (O32 | 0xf638): case (A32 | O32 | 0xf638):
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf630) ? DIV_FUNC_DIV_16_16_8 :
							DIV_FUNC_IDIV_16_16_8;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));	//Read CL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
																	//Div AX, AX, CL [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_16, REG_EAX, HOST_16, REG_EAX, HOST_L8, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			
			break;
		}

	//DIV Rb
	case 0xf6f0: case (A32 | 0xf6f0): case (O32 | 0xf6f0): case (A32 | O32 | 0xf6f0):
	//IDIV Rb
	case 0xf6f8: case (A32 | 0xf6f8): case (O32 | 0xf6f8): case (A32 | O32 | 0xf6f8):
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf6f0) ? DIV_FUNC_DIV_16_16_8 :
							DIV_FUNC_IDIV_16_16_8;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));			//Get ECX, Rb_32
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
																	//Div AX, AX, {CL|CH} [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_16, REG_EAX, HOST_16, REG_EAX, hostRegType, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			
			break;
		}

	case 0xf730:	case (A32 | 0xf730):	//DIV Mw
	case 0xf738:	case (A32 | 0xf738):	//IDIV Mw
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf730) ? DIV_FUNC_DIV_32_32_16 :
							DIV_FUNC_IDIV_32_32_16;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_16, REG_ECX, HOST_32, REG_EBX));	//Read CX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Div DX:AX, DX:AX, CX [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case 0xf7f0:	case (A32 | 0xf7f0):	//DIV Rw
	case 0xf7f8:	case (A32 | 0xf7f8):	//IDIV Rw
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf7f0) ? DIV_FUNC_DIV_32_32_16 :
							DIV_FUNC_IDIV_32_32_16;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
																	//Div DX:AX, DX:AX, CX [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_EDX, HOST_16, REG_EAX, HOST_16, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case (O32 | 0xf7f0):	case (A32 | O32 | 0xf7f0):	//DIV Rd
	case (O32 | 0xf7f8):	case (A32 | O32 | 0xf7f8):	//IDIV Rd
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf7f0) ? DIV_FUNC_DIV_64_64_32 :
							DIV_FUNC_IDIV_64_64_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
															//Div EDX:EAX, EDX:EAX, ECX [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	case (O32 | 0xf730):	case (A32 | O32 | 0xf730):	//DIV Md
	case (O32 | 0xf738):	case (A32 | O32 | 0xf738):	//IDIV Md
		{
			Dword divFunc = ((opcode & ~(A32 | O32)) == 0xf730) ? DIV_FUNC_DIV_64_64_32 :
							DIV_FUNC_IDIV_64_64_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));	//Read ECX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EDX));		//Get EDX, gEDX
															//Div EDX:EAX, EDX:EAX, ECX [F]
			AddOp(TYPE_DIV, divFunc, 0, ALLF, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_EDX, HOST_32, REG_EAX, HOST_32, REG_ECX));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_EDX));		//Put gEDX, EDX

			break;
		}

	//ROTATE AND SHIFT INSTRUCTIONS
	//TODO: Be more precise about which flags are read
	//Rot/Shf Mb, Ib
	case 0xc000: case(A32 | 0xc000): case (O32 | 0xc000): case (A32 | O32 | 0xc000):
	case 0xc008: case(A32 | 0xc008): case (O32 | 0xc008): case (A32 | O32 | 0xc008):
	case 0xc010: case(A32 | 0xc010): case (O32 | 0xc010): case (A32 | O32 | 0xc010):
	case 0xc018: case(A32 | 0xc018): case (O32 | 0xc018): case (A32 | O32 | 0xc018):
	case 0xc020: case(A32 | 0xc020): case (O32 | 0xc020): case (A32 | O32 | 0xc020):
	case 0xc028: case(A32 | 0xc028): case (O32 | 0xc028): case (A32 | O32 | 0xc028):
	case 0xc030: case(A32 | 0xc030): case (O32 | 0xc030): case (A32 | O32 | 0xc030):
	case 0xc038: case(A32 | 0xc038): case (O32 | 0xc038): case (A32 | O32 | 0xc038):
	//Rot/Shf Mb, 1
	case 0xd000: case(A32 | 0xd000): case (O32 | 0xd000): case (A32 | O32 | 0xd000):
	case 0xd008: case(A32 | 0xd008): case (O32 | 0xd008): case (A32 | O32 | 0xd008):
	case 0xd010: case(A32 | 0xd010): case (O32 | 0xd010): case (A32 | O32 | 0xd010):
	case 0xd018: case(A32 | 0xd018): case (O32 | 0xd018): case (A32 | O32 | 0xd018):
	case 0xd020: case(A32 | 0xd020): case (O32 | 0xd020): case (A32 | O32 | 0xd020):
	case 0xd028: case(A32 | 0xd028): case (O32 | 0xd028): case (A32 | O32 | 0xd028):
	case 0xd030: case(A32 | 0xd030): case (O32 | 0xd030): case (A32 | O32 | 0xd030):
	case 0xd038: case(A32 | 0xd038): case (O32 | 0xd038): case (A32 | O32 | 0xd038):
		{
			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Byte Ib = 1;
			if((opcode & 0xff00) == 0xc000)
			{
				if(!ReadCodeByte(r_eip++, Ib))
					return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_ECX, HOST_32, REG_EBX));	//Read CL, [EBX]
																	//Rot/Shf CL, Ib [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, HOST_L8, REG_ECX, IMM_8, Ib));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_ECX));	//Write [EBX], CL

			break;
		}

	//Rot/Shf Rb, Ib
	case 0xc0c0: case(A32 | 0xc0c0): case (O32 | 0xc0c0): case (A32 | O32 | 0xc0c0):
	case 0xc0c8: case(A32 | 0xc0c8): case (O32 | 0xc0c8): case (A32 | O32 | 0xc0c8):
	case 0xc0d0: case(A32 | 0xc0d0): case (O32 | 0xc0d0): case (A32 | O32 | 0xc0d0):
	case 0xc0d8: case(A32 | 0xc0d8): case (O32 | 0xc0d8): case (A32 | O32 | 0xc0d8):
	case 0xc0e0: case(A32 | 0xc0e0): case (O32 | 0xc0e0): case (A32 | O32 | 0xc0e0):
	case 0xc0e8: case(A32 | 0xc0e8): case (O32 | 0xc0e8): case (A32 | O32 | 0xc0e8):
	case 0xc0f0: case(A32 | 0xc0f0): case (O32 | 0xc0f0): case (A32 | O32 | 0xc0f0):
	case 0xc0f8: case(A32 | 0xc0f8): case (O32 | 0xc0f8): case (A32 | O32 | 0xc0f8):
	//Rot/Shf Rb, 1
	case 0xd0c0: case(A32 | 0xd0c0): case (O32 | 0xd0c0): case (A32 | O32 | 0xd0c0):
	case 0xd0c8: case(A32 | 0xd0c8): case (O32 | 0xd0c8): case (A32 | O32 | 0xd0c8):
	case 0xd0d0: case(A32 | 0xd0d0): case (O32 | 0xd0d0): case (A32 | O32 | 0xd0d0):
	case 0xd0d8: case(A32 | 0xd0d8): case (O32 | 0xd0d8): case (A32 | O32 | 0xd0d8):
	case 0xd0e0: case(A32 | 0xd0e0): case (O32 | 0xd0e0): case (A32 | O32 | 0xd0e0):
	case 0xd0e8: case(A32 | 0xd0e8): case (O32 | 0xd0e8): case (A32 | O32 | 0xd0e8):
	case 0xd0f0: case(A32 | 0xd0f0): case (O32 | 0xd0f0): case (A32 | O32 | 0xd0f0):
	case 0xd0f8: case(A32 | 0xd0f8): case (O32 | 0xd0f8): case (A32 | O32 | 0xd0f8):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Byte Ib = 1;
			if((opcode & 0xff00) == 0xc000)
			{
				if(!ReadCodeByte(r_eip++, Ib))
					return false;
			}

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rb));			//Get ECX, Rb_32
																	//Rot/Shf {CL|CH}, Ib [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostRegType, REG_ECX, IMM_8, Ib));
			AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_ECX));			//Put Rb_32, ECX

			break;
		}

	//Rot/Shf Mw, Ib
	case 0xc100:	case (A32 | 0xc100):	case 0xc108:	case (A32 | 0xc108):
	case 0xc110:	case (A32 | 0xc110):	case 0xc118:	case (A32 | 0xc118):
	case 0xc120:	case (A32 | 0xc120):	case 0xc128:	case (A32 | 0xc128):
	case 0xc130:	case (A32 | 0xc130):	case 0xc138:	case (A32 | 0xc138):
	//Rot/Shf Mw, 1
	case 0xd100:	case (A32 |	0xd100):	case 0xd108:	case (A32 | 0xd108):
	case 0xd110:	case (A32 |	0xd110):	case 0xd118:	case (A32 | 0xd118):
	case 0xd120:	case (A32 |	0xd120):	case 0xd128:	case (A32 | 0xd128):
	case 0xd130:	case (A32 |	0xd130):	case 0xd138:	case (A32 | 0xd138):
	//Rot/Shf Md, Ib
	case (O32 | 0xc100):	case (A32 | O32 | 0xc100):	case (O32 | 0xc108):	case (A32 | O32 | 0xc108):
	case (O32 | 0xc110):	case (A32 | O32 | 0xc110):	case (O32 | 0xc118):	case (A32 | O32 | 0xc118):
	case (O32 | 0xc120):	case (A32 | O32 | 0xc120):	case (O32 | 0xc128):	case (A32 | O32 | 0xc128):
	case (O32 | 0xc130):	case (A32 | O32 | 0xc130):	case (O32 | 0xc138):	case (A32 | O32 | 0xc138):
	//Rot/Shf Md, 1
	case (O32 | 0xd100):	case (A32 |	O32 | 0xd100):	case (O32 | 0xd108):	case (A32 | O32 | 0xd108):
	case (O32 | 0xd110):	case (A32 |	O32 | 0xd110):	case (O32 | 0xd118):	case (A32 | O32 | 0xd118):
	case (O32 | 0xd120):	case (A32 |	O32 | 0xd120):	case (O32 | 0xd128):	case (A32 | O32 | 0xd128):
	case (O32 | 0xd130):	case (A32 |	O32 | 0xd130):	case (O32 | 0xd138):	case (A32 | O32 | 0xd138):
		{
			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Byte Ib = 1;
			if((opcode & 0xff00) == 0xc100)
			{
				if(!ReadCodeByte(r_eip++, Ib))
					return false;
			}

			AddOp(TYPE_READ, hostType, REG_ECX, HOST_32, REG_EBX));		//Read eCX, [EBX]
																		//Rot/Shf eCX, Ib [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostType, REG_ECX, IMM_8, Ib));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_ECX));	//Write [EBX], eCX

			break;
		}

	//Rot/Shf Rw, Ib
	case 0xc1c0:	case (A32 | 0xc1c0):	case 0xc1c8:	case (A32 | 0xc1c8):
	case 0xc1d0:	case (A32 | 0xc1d0):	case 0xc1d8:	case (A32 | 0xc1d8):
	case 0xc1e0:	case (A32 | 0xc1e0):	case 0xc1e8:	case (A32 | 0xc1e8):
	case 0xc1f0:	case (A32 | 0xc1f0):	case 0xc1f8:	case (A32 | 0xc1f8):
	//Rot/Shf Rw, 1
	case 0xd1c0:	case (A32 |	0xd1c0):	case 0xd1c8:	case (A32 | 0xd1c8):
	case 0xd1d0:	case (A32 |	0xd1d0):	case 0xd1d8:	case (A32 | 0xd1d8):
	case 0xd1e0:	case (A32 |	0xd1e0):	case 0xd1e8:	case (A32 | 0xd1e8):
	case 0xd1f0:	case (A32 |	0xd1f0):	case 0xd1f8:	case (A32 | 0xd1f8):
	//Rot/Shf Rd, Ib
	case (O32 | 0xc1c0):	case (A32 | O32 | 0xc1c0):	case (O32 | 0xc1c8):	case (A32 | O32 | 0xc1c8):
	case (O32 | 0xc1d0):	case (A32 | O32 | 0xc1d0):	case (O32 | 0xc1d8):	case (A32 | O32 | 0xc1d8):
	case (O32 | 0xc1e0):	case (A32 | O32 | 0xc1e0):	case (O32 | 0xc1e8):	case (A32 | O32 | 0xc1e8):
	case (O32 | 0xc1f0):	case (A32 | O32 | 0xc1f0):	case (O32 | 0xc1f8):	case (A32 | O32 | 0xc1f8):
	//Rot/Shf Rd, 1
	case (O32 | 0xd1c0):	case (A32 |	O32 | 0xd1c0):	case (O32 | 0xd1c8):	case (A32 | O32 | 0xd1c8):
	case (O32 | 0xd1d0):	case (A32 |	O32 | 0xd1d0):	case (O32 | 0xd1d8):	case (A32 | O32 | 0xd1d8):
	case (O32 | 0xd1e0):	case (A32 |	O32 | 0xd1e0):	case (O32 | 0xd1e8):	case (A32 | O32 | 0xd1e8):
	case (O32 | 0xd1f0):	case (A32 |	O32 | 0xd1f0):	case (O32 | 0xd1f8):	case (A32 | O32 | 0xd1f8):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Byte Ib = 1;
			if((opcode & 0xff00) == 0xc100)
			{
				if(!ReadCodeByte(r_eip++, Ib))
					return false;
			}

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));		//Get ECX, Rd
																//Rot/Shf eCX, Ib [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostType, REG_ECX, IMM_8, Ib));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	//Rot/Shf Mb, CL
	case 0xd200: case(A32 | 0xd200): case (O32 | 0xd200): case (A32 | O32 | 0xd200):
	case 0xd208: case(A32 | 0xd208): case (O32 | 0xd208): case (A32 | O32 | 0xd208):
	case 0xd210: case(A32 | 0xd210): case (O32 | 0xd210): case (A32 | O32 | 0xd210):
	case 0xd218: case(A32 | 0xd218): case (O32 | 0xd218): case (A32 | O32 | 0xd218):
	case 0xd220: case(A32 | 0xd220): case (O32 | 0xd220): case (A32 | O32 | 0xd220):
	case 0xd228: case(A32 | 0xd228): case (O32 | 0xd228): case (A32 | O32 | 0xd228):
	case 0xd230: case(A32 | 0xd230): case (O32 | 0xd230): case (A32 | O32 | 0xd230):
	case 0xd238: case(A32 | 0xd238): case (O32 | 0xd238): case (A32 | O32 | 0xd238):
		{
			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, HOST_L8, REG_EDX, HOST_32, REG_EBX));		//Read DL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));			//Get ECX, gECX
																		//Rot/Shf DL, CL [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, HOST_L8, REG_EDX, HOST_L8, REG_ECX));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_EDX));		//Write [EBX], DL

			break;
		}

	//Rot/Shf Rb, CL
	case 0xd2c0: case(A32 | 0xd2c0): case (O32 | 0xd2c0): case (A32 | O32 | 0xd2c0):
	case 0xd2c8: case(A32 | 0xd2c8): case (O32 | 0xd2c8): case (A32 | O32 | 0xd2c8):
	case 0xd2d0: case(A32 | 0xd2d0): case (O32 | 0xd2d0): case (A32 | O32 | 0xd2d0):
	case 0xd2d8: case(A32 | 0xd2d8): case (O32 | 0xd2d8): case (A32 | O32 | 0xd2d8):
	case 0xd2e0: case(A32 | 0xd2e0): case (O32 | 0xd2e0): case (A32 | O32 | 0xd2e0):
	case 0xd2e8: case(A32 | 0xd2e8): case (O32 | 0xd2e8): case (A32 | O32 | 0xd2e8):
	case 0xd2f0: case(A32 | 0xd2f0): case (O32 | 0xd2f0): case (A32 | O32 | 0xd2f0):
	case 0xd2f8: case(A32 | 0xd2f8): case (O32 | 0xd2f8): case (A32 | O32 | 0xd2f8):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);

			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rb));			//Get EDX, Rb_32
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));		//Get ECX, gECX
																	//Rot/Shf {DL|DH}, CL [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostRegType, REG_EDX, HOST_L8, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_EDX));			//Put Rb_32, EDX

			break;
		}

	//Rot/Shf Mw, CL
	case 0xd300:	case (A32 | 0xd300):	case 0xd308:	case (A32 | 0xd308):
	case 0xd310:	case (A32 | 0xd310):	case 0xd318:	case (A32 | 0xd318):
	case 0xd320:	case (A32 | 0xd320):	case 0xd328:	case (A32 | 0xd328):
	case 0xd330:	case (A32 | 0xd330):	case 0xd338:	case (A32 | 0xd338):
	//Rot/Shf Md, CL
	case (O32 | 0xd300):	case (A32 | O32 | 0xd300):	case (O32 | 0xd308):	case (A32 | O32 | 0xd308):
	case (O32 | 0xd310):	case (A32 | O32 | 0xd310):	case (O32 | 0xd318):	case (A32 | O32 | 0xd318):
	case (O32 | 0xd320):	case (A32 | O32 | 0xd320):	case (O32 | 0xd328):	case (A32 | O32 | 0xd328):
	case (O32 | 0xd330):	case (A32 | O32 | 0xd330):	case (O32 | 0xd338):	case (A32 | O32 | 0xd338):
		{
			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, hostType, REG_EDX, HOST_32, REG_EBX));		//Read eDX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));			//Get ECX, gECX
																		//Rot/Shf eDX, CL [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostType, REG_EDX, HOST_L8, REG_ECX));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EDX));	//Write [EBX], eDX

			break;
		}

	//Rot/Shf Rw, CL
	case 0xd3c0:	case (A32 |	0xd3c0):	case 0xd3c8:	case (A32 | 0xd3c8):
	case 0xd3d0:	case (A32 |	0xd3d0):	case 0xd3d8:	case (A32 | 0xd3d8):
	case 0xd3e0:	case (A32 |	0xd3e0):	case 0xd3e8:	case (A32 | 0xd3e8):
	case 0xd3f0:	case (A32 |	0xd3f0):	case 0xd3f8:	case (A32 | 0xd3f8):
	//Rot/Shf Rd, CL
	case (O32 | 0xd3c0):	case (A32 |	O32 | 0xd3c0):	case (O32 | 0xd3c8):	case (A32 | O32 | 0xd3c8):
	case (O32 | 0xd3d0):	case (A32 |	O32 | 0xd3d0):	case (O32 | 0xd3d8):	case (A32 | O32 | 0xd3d8):
	case (O32 | 0xd3e0):	case (A32 |	O32 | 0xd3e0):	case (O32 | 0xd3e8):	case (A32 | O32 | 0xd3e8):
	case (O32 | 0xd3f0):	case (A32 |	O32 | 0xd3f0):	case (O32 | 0xd3f8):	case (A32 | O32 | 0xd3f8):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = (opcode & 0x38) >> 3;
			if(shiftFunc == SHIFT_FUNC_SAL)
				shiftFunc = SHIFT_FUNC_SHL;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));			//Get EDX, Rd
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));		//Get ECX, gECX
																	//Rot/Shf eDX, CL [F]
			AddOp(TYPE_SHIFT, shiftFunc, ALLF, ALLF, hostType, REG_EDX, HOST_L8, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));			//Put Rd, EDX

			break;
		}

	//SHLD Rv, Gv, Ib
	case 0x0fa4c0: case (A32 | 0x0fa4c0): case (O32 | 0x0fa4c0): case (A32 | O32 | 0x0fa4c0):
	//SHRD Rv, Gv, Ib
	case 0x0facc0: case (A32 | 0x0facc0): case (O32 | 0x0facc0): case (A32 | O32 | 0x0facc0):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = ((opcode & ~(A32 | O32)) == 0x0fa4c0) ?
								SHIFTD_FUNC_SHLD : SHIFTD_FUNC_SHRD;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));		//Get EDX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));		//Get EAX, Gd
																//Shiftd eDX, eAX, Ib [F]
			AddOp(TYPE_SHIFTD, shiftFunc, ALLF, ALLF, hostType, REG_EDX, hostType, REG_EAX, IMM_8, Ib));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));		//Put Rd, EDX

			break;
		}

    //SHLD Mv, Gv, Ib
	case 0x0fa400: case (A32 | 0x0fa400): case (O32 | 0x0fa400): case (A32 | O32 | 0x0fa400):
	//SHRD Mv, Gv, Ib
	case 0x0fac00: case (A32 | 0x0fac00): case (O32 | 0x0fac00): case (A32 | O32 | 0x0fac00):
		{
			Dword shiftFunc = ((opcode & ~(A32 | O32)) == 0x0fa400) ?
								SHIFTD_FUNC_SHLD : SHIFTD_FUNC_SHRD;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_READ, hostType, REG_EDX, HOST_32, REG_EBX));	//Read eDX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));		//Get EAX, Gd
																//Shiftd eDX, eAX, Ib [F]
			AddOp(TYPE_SHIFTD, shiftFunc, ALLF, ALLF, hostType, REG_EDX, hostType, REG_EAX, IMM_8, Ib));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EDX));//Write [EBX], eDX

			break;
		}

	//SHLD Rv, Gv, CL
	case 0x0fa5c0: case (A32 | 0x0fa5c0): case (O32 | 0x0fa5c0): case (A32 | O32 | 0x0fa5c0):
	//SHRD Rv, Gv, CL
	case 0x0fadc0: case (A32 | 0x0fadc0): case (O32 | 0x0fadc0): case (A32 | O32 | 0x0fadc0):
		{
			instructionMayCauseException = false;

			Dword shiftFunc = ((opcode & ~(A32 | O32)) == 0x0fa5c0) ?
								SHIFTD_FUNC_SHLD : SHIFTD_FUNC_SHRD;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));		//Get EDX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));		//Get EAX, Gd
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));	//Get ECX, gECX
																//Shiftd eDX, eAX, CL [F]
			AddOp(TYPE_SHIFTD, shiftFunc, ALLF, ALLF, hostType, REG_EDX, hostType, REG_EAX, HOST_L8, REG_ECX));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_EDX));		//Put Rd, EDX

			break;
		}

    //SHLD Mv, Gv, CL
	case 0x0fa500: case (A32 | 0x0fa500): case (O32 | 0x0fa500): case (A32 | O32 | 0x0fa500):
	//SHRD Mv, Gv, CL
	case 0x0fad00: case (A32 | 0x0fad00): case (O32 | 0x0fad00): case (A32 | O32 | 0x0fad00):
		{
			Dword shiftFunc = ((opcode & ~(A32 | O32)) == 0x0fa500) ?
								SHIFTD_FUNC_SHLD : SHIFTD_FUNC_SHRD;

			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, hostType, REG_EDX, HOST_32, REG_EBX));	//Read eDX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, Gd));		//Get EAX, Gd
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));	//Get ECX, gECX
																//Shiftd eDX, eAX, CL [F]
			AddOp(TYPE_SHIFTD, shiftFunc, ALLF, ALLF, hostType, REG_EDX, hostType, REG_EAX, HOST_L8, REG_ECX));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, hostType, REG_EDX));//Write [EBX], eDX

			break;
		}

	//BIT INSTRUCTIONS

	//BT[S|R|C] M, Ib
	case 0x0fba20: case (A32 | 0x0fba20): case (O32 | 0x0fba20): case (A32 | O32 | 0x0fba20):
	case 0x0fba28: case (A32 | 0x0fba28): case (O32 | 0x0fba28): case (A32 | O32 | 0x0fba28):
	case 0x0fba30: case (A32 | 0x0fba30): case (O32 | 0x0fba30): case (A32 | O32 | 0x0fba30):
	case 0x0fba38: case (A32 | 0x0fba38): case (O32 | 0x0fba38): case (A32 | O32 | 0x0fba38):
		{
			Dword bitFunc = (opcode & 0x18) >> 3;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_READ, HOST_32, REG_ECX, HOST_32, REG_EBX));		//Read ECX, [EBX]
																		//Bit ECX, Ib%32 [F]
			AddOp(TYPE_BIT, bitFunc, 0, ALLF, HOST_32, REG_ECX, IMM_8, Ib & 0x1f));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_32, REG_ECX));		//Write [EBX], ECX

			break;
		}

	//BT[S|R|C] Rw, Ib
	case 0x0fbae0:			case 0x0fbae8:
	case 0x0fbaf0:			case 0x0fbaf8:
	case (A32 | 0x0fbae0):	case (A32 | 0x0fbae8):
	case (A32 | 0x0fbaf0):	case (A32 | 0x0fbaf8):
	//BT[S|R|C] Rd, Ib
	case (O32 | 0x0fbae0):			case (O32 | 0x0fbae8):
	case (O32 | 0x0fbaf0):			case (O32 | 0x0fbaf8):
	case (A32 | O32 | 0x0fbae0):	case (A32 | O32 | 0x0fbae8):
	case (A32 | O32 | 0x0fbaf0):	case (A32 | O32 | 0x0fbaf8):
		{
			instructionMayCauseException = false;

			Dword bitFunc = (opcode & 0x18) >> 3;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			//Calculate which host register type to use
			const OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));			//Get ECX, Rd
																	//Bit eCX, Ib [F]
			AddOp(TYPE_BIT, bitFunc, 0, ALLF, hostType, REG_ECX, IMM_8, Ib));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));			//Put Rd, ECX

			break;
		}

	//BT[S|R|C] Rw, Gw
	case 0x0fa3c0:			case 0x0fabc0:
	case 0x0fb3c0:			case 0x0fbbc0:
	case (A32 | 0x0fa3c0):	case (A32 | 0x0fabc0):
	case (A32 | 0x0fb3c0):	case (A32 | 0x0fbbc0):
	//BT[S|R|C] Rd, Gd
	case (O32 | 0x0fa3c0):			case (O32 | 0x0fabc0):
	case (O32 | 0x0fb3c0):			case (O32 | 0x0fbbc0):
	case (A32 | O32 | 0x0fa3c0):	case (A32 | O32 | 0x0fabc0):
	case (A32 | O32 | 0x0fb3c0):	case (A32 | O32 | 0x0fbbc0):
		{
			instructionMayCauseException = false;

			Dword bitFunc = (opcode & 0x1800) >> 11;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate which host register type to use
			const OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Rd));		//Get ECX, Rd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gd));		//Get EDX, Gd
																//Bit eCX, eDX [F]
			AddOp(TYPE_BIT, bitFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));
			AddOp(TYPE_PUT, GUEST, Rd, HOST_32, REG_ECX));		//Put Rd, ECX

			break;
		}

	//BT[S|R|C] M, Gv
	case 0x0fa300: case (A32 | 0x0fa300): case (O32 | 0x0fa300): case (A32 | O32 | 0x0fa300):
	case 0x0fab00: case (A32 | 0x0fab00): case (O32 | 0x0fab00): case (A32 | O32 | 0x0fab00):
	case 0x0fb300: case (A32 | 0x0fb300): case (O32 | 0x0fb300): case (A32 | O32 | 0x0fb300):
	case 0x0fbb00: case (A32 | 0x0fbb00): case (O32 | 0x0fbb00): case (A32 | O32 | 0x0fbb00):
		{
			Dword bitFunc = (opcode & 0x1800) >> 11;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, Gd));					//Get ESI, Gd
			if((opcode & O32) == 0)									//If O16, SignEx ESI, SI
				AddOp(TYPE_SIGNEX, HOST_32, REG_ESI, HOST_16, REG_ESI));
			AddOp(TYPE_MOV, HOST_32, REG_EDX, HOST_32, REG_ESI));			//Mov EDX, ESI
			AddOp(TYPE_SHIFT, SHIFT_FUNC_SAR, HOST_32, REG_EDX, IMM_8, 3));	//Sar EDX, 3
			AddOp(TYPE_ALU, ALU_FUNC_AND, HOST_32, REG_EDX, IMM_32, ~0x3));	//And EDX, ~0x3
			AddOp(TYPE_ALU, ALU_FUNC_AND, HOST_32, REG_ESI, IMM_32, 0x1f));	//And ESI, 0x1f

			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EDX));//Add EBX, EDX
			AddOp(TYPE_READ, HOST_32, REG_EAX, HOST_32, REG_EBX));			//Read EAX, [EBX]

																			//Bit EAX, ESI [F]
			AddOp(TYPE_BIT, bitFunc, 0, ALLF, HOST_32, REG_EAX, HOST_32, REG_ESI));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_32, REG_EAX));			//Write [EBX], EAX

			break;
		}

	//STRING INSTRUCTIONS

	case 0xa4: case (A32 | 0xa4): case (O32 | 0xa4): case (A32 | O32 | 0xa4): //MOVSB
	case 0xa5:			case (A32 | 0xa5):			//MOVSW
	case (O32 | 0xa5):	case (A32 | O32 | 0xa5):	//MOVSD
		{
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			OperandType indexHostType = HOST_16;
			OperandType indexImmType = IMM_16;
			MicroOpType indexMovType = TYPE_ZEROEX;
			if((opcode & A32) != 0)
			{
				indexHostType = HOST_32;
				indexImmType = IMM_32;
				indexMovType = TYPE_MOV;
			}

			OperandType hostRegType = HOST_L8;
			Dword regOffset = 1;

			if((opcode & ~A32) == 0xa5)
			{
				hostRegType = HOST_16;
				regOffset = 2;
			}
			if((opcode & ~A32) == (O32 | 0xa5))
			{
				hostRegType = HOST_32;
				regOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ESI));		//Get EBX, gESI
			AddOp(indexMovType, HOST_32, REG_ESI, indexHostType, REG_EBX));	//IMov ESI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, segRegBase));	//Get EBX, segRegBase
																	//Add ESI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ESI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EDI));		//Get EBX, gEDI
			AddOp(indexMovType, HOST_32, REG_EDI, indexHostType, REG_EBX));	//IMov EDI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ES_BASE));	//Get EBX, ES.BASE
																	//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));
			
			AddOp(TYPE_READ, hostRegType, REG_EAX, HOST_32, REG_ESI));	//Read A, [ESI]
			AddOp(TYPE_WRITE, HOST_32, REG_EDI, hostRegType, REG_EAX));	//Write [EDI], A

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESI));		//Get ESI, gESI
																	//DUpdate eSI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_ESI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_ESI, HOST_32, REG_ESI));		//Put gESI, ESI

			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EDI));		//Get EDI, gEDI
																	//DUpdate eDI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_EDI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_EDI, HOST_32, REG_EDI));		//Put gEDI, EDI

			break;
		}

	case 0xa6: case (A32 | 0xa6): case (O32 | 0xa6): case (A32 | O32 | 0xa6): //CMPSB
	case 0xa7:			case (A32 | 0xa7):			//CMPSW
	case (O32 | 0xa7):	case (A32 | O32 | 0xa7):	//CMPSD
		{
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			OperandType indexHostType = HOST_16;
			OperandType indexImmType = IMM_16;
			MicroOpType indexMovType = TYPE_ZEROEX;
			if((opcode & A32) != 0)
			{
				indexHostType = HOST_32;
				indexImmType = IMM_32;
				indexMovType = TYPE_MOV;
			}

			OperandType hostRegType = HOST_L8;
			Dword regOffset = 1;

			if((opcode & ~A32) == 0xa7)
			{
				hostRegType = HOST_16;
				regOffset = 2;
			}
			if((opcode & ~A32) == (O32 | 0xa7))
			{
				hostRegType = HOST_32;
				regOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ESI));		//Get EBX, gESI
			AddOp(indexMovType, HOST_32, REG_ESI, indexHostType, REG_EBX));	//IMov ESI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, segRegBase));	//Get EBX, segRegBase
																	//Add ESI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ESI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EDI));		//Get EBX, gEDI
			AddOp(indexMovType, HOST_32, REG_EDI, indexHostType, REG_EBX));	//IMov EDI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ES_BASE));	//Get EBX, ES.BASE
																	//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));
			
			AddOp(TYPE_READ, hostRegType, REG_EBX, HOST_32, REG_ESI));	//Read B, [ESI]
			AddOp(TYPE_READ, hostRegType, REG_EAX, HOST_32, REG_EDI));	//Read A, [EDI]
			
																		//Cmp A, B
			AddOp(TYPE_ALU, ALU_FUNC_CMP, 0, ALLF, hostRegType, REG_EAX, hostRegType, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESI));		//Get ESI, gESI
																	//DUpdate eSI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_ESI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_ESI, HOST_32, REG_ESI));		//Put gESI, ESI

			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EDI));		//Get EDI, gEDI
																	//DUpdate eDI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_EDI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_EDI, HOST_32, REG_EDI));		//Put gEDI, EDI

			break;
		}

	case 0xaa: case (A32 | 0xaa): case (O32 | 0xaa): case (A32 | O32 | 0xaa): //STOSB
	case 0xab:			case (A32 | 0xab):			//STOSW
	case (O32 | 0xab):	case (A32 | O32 | 0xab):	//STOSD
		{
			OperandType indexHostType = HOST_16;
			OperandType indexImmType = IMM_16;
			MicroOpType indexMovType = TYPE_ZEROEX;
			if((opcode & A32) != 0)
			{
				indexHostType = HOST_32;
				indexImmType = IMM_32;
				indexMovType = TYPE_MOV;
			}

			OperandType hostRegType = HOST_L8;
			Dword regOffset = 1;

			if((opcode & ~A32) == 0xab)
			{
				hostRegType = HOST_16;
				regOffset = 2;
			}
			if((opcode & ~A32) == (O32 | 0xab))
			{
				hostRegType = HOST_32;
				regOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EDI));		//Get EBX, gEDI
			AddOp(indexMovType, HOST_32, REG_EDI, indexHostType, REG_EBX));	//IMov EDI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ES_BASE));	//Get EBX, ES.BASE
																	//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EAX));		//Get EBX, gEAX
			AddOp(TYPE_WRITE, HOST_32, REG_EDI, hostRegType, REG_EBX));	//Write [EDI], B

			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EDI));		//Get EDI, gEDI
																	//DUpdate eDI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_EDI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_EDI, HOST_32, REG_EDI));		//Put gEDI, EDI

			break;
		}

	case 0xac: case (A32 | 0xac): case (O32 | 0xac): case (A32 | O32 | 0xac): //LODSB
	case 0xad:			case (A32 | 0xad):			//LODSW
	case (O32 | 0xad):	case (A32 | O32 | 0xad):	//LODSD
		{
			//Calculate which segment register to use
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			OperandType indexHostType = HOST_16;
			OperandType indexImmType = IMM_16;
			MicroOpType indexMovType = TYPE_ZEROEX;
			if((opcode & A32) != 0)
			{
				indexHostType = HOST_32;
				indexImmType = IMM_32;
				indexMovType = TYPE_MOV;
			}

			OperandType hostRegType = HOST_L8;
			Dword regOffset = 1;

			if((opcode & ~A32) == 0xad)
			{
				hostRegType = HOST_16;
				regOffset = 2;
			}
			if((opcode & ~A32) == (O32 | 0xad))
			{
				hostRegType = HOST_32;
				regOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ESI));		//Get EBX, gESI
			AddOp(indexMovType, HOST_32, REG_ESI, indexHostType, REG_EBX));	//IMov ESI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, segRegBase));	//Get EBX, segRegBase
																	//Add ESI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_ESI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EAX));		//Get EBX, gEAX
			AddOp(TYPE_READ, hostRegType, REG_EAX, HOST_32, REG_ESI));	//Read A, [ESI]
			AddOp(TYPE_MOV, hostRegType, REG_EBX, hostRegType, REG_EAX));	//Mov B, A
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EBX));		//Put gEAX, EBX

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESI));		//Get ESI, gESI
																	//DUpdate eSI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_ESI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_ESI, HOST_32, REG_ESI));		//Put gESI, ESI

			break;
		}

	case 0xae: case (A32 | 0xae): case (O32 | 0xae): case (A32 | O32 | 0xae): //SCASB
	case 0xaf:			case (A32 | 0xaf):			//SCASW
	case (O32 | 0xaf):	case (A32 | O32 | 0xaf):	//SCASD
		{
			OperandType indexHostType = HOST_16;
			OperandType indexImmType = IMM_16;
			MicroOpType indexMovType = TYPE_ZEROEX;
			if((opcode & A32) != 0)
			{
				indexHostType = HOST_32;
				indexImmType = IMM_32;
				indexMovType = TYPE_MOV;
			}

			OperandType hostRegType = HOST_L8;
			Dword regOffset = 1;

			if((opcode & ~A32) == 0xaf)
			{
				hostRegType = HOST_16;
				regOffset = 2;
			}
			if((opcode & ~A32) == (O32 | 0xaf))
			{
				hostRegType = HOST_32;
				regOffset = 4;
			}

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EDI));		//Get EBX, gEDI
			AddOp(indexMovType, HOST_32, REG_EDI, indexHostType, REG_EBX));	//IMov EDI, eBX
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_ES_BASE));	//Get EBX, ES.BASE
																	//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EAX));			//Get EBX, gEAX
			AddOp(TYPE_READ, hostRegType, REG_EAX, HOST_32, REG_EDI));	//Read A, [EDI]
			
																	//Cmp B, A
			AddOp(TYPE_ALU, ALU_FUNC_CMP, 0, ALLF, hostRegType, REG_EBX, hostRegType, REG_EAX));

			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EDI));		//Get EDI, gEDI
																	//DUpdate eDI, regOffset
			AddOp(TYPE_DUPDATE, indexHostType, REG_EDI, indexImmType, regOffset));
			AddOp(TYPE_PUT, GUEST, REG_EDI, HOST_32, REG_EDI));		//Put gEDI, EDI

			break;
		}

	//CONTROL TRANSFER INSTRUCTIONS

	case 0xe9: case (A32 | 0xe9):	//JMP Jw
		{
			Word Jw;
			if(!ReadCodeWord(r_eip, Jw))
				return false;
			r_eip += 2;

			Dword instrLength = r_eip - startEip;

			AddOp(TYPE_ADDEIP, IMM_32, Jw + instrLength));		//AddEip, Jw
			AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	case (O32 | 0xe9): case (A32 | O32 | 0xe9):	//JMP Jd
		{
			Dword Jd;
			if(!ReadCodeDword(r_eip, Jd))
				return false;
			r_eip += 4;

			Dword instrLength = r_eip - startEip;

			AddOp(TYPE_ADDEIP, IMM_32, Jd + instrLength));		//AddEip, Jd

			break;
		}

	case 0xeb: case (A32 | 0xeb): case (O32 | 0xeb): case (A32 | O32 | 0xeb):	//JMP Jb
		{
			Byte Jb;
			if(!ReadCodeByte(r_eip++, Jb))
				return false;

			Dword instrLength = r_eip - startEip;

																//AddEip, Jb (sign-extended)
			AddOp(TYPE_ADDEIP, IMM_32, static_cast<SignedByte>(Jb) + instrLength));
			if((opcode & O32) == 0)								//If O16
				AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	//Jcond Jb
	case 0x72: case (A32 | 0x72): case (O32 | 0x72): case (A32 | O32 | 0x72):
	case 0x73: case (A32 | 0x73): case (O32 | 0x73): case (A32 | O32 | 0x73):
	case 0x74: case (A32 | 0x74): case (O32 | 0x74): case (A32 | O32 | 0x74):
	case 0x75: case (A32 | 0x75): case (O32 | 0x75): case (A32 | O32 | 0x75):
	case 0x76: case (A32 | 0x76): case (O32 | 0x76): case (A32 | O32 | 0x76):
	case 0x77: case (A32 | 0x77): case (O32 | 0x77): case (A32 | O32 | 0x77):
	case 0x78: case (A32 | 0x78): case (O32 | 0x78): case (A32 | O32 | 0x78):
	case 0x79: case (A32 | 0x79): case (O32 | 0x79): case (A32 | O32 | 0x79):
	case 0x7a: case (A32 | 0x7a): case (O32 | 0x7a): case (A32 | O32 | 0x7a):
	case 0x7b: case (A32 | 0x7b): case (O32 | 0x7b): case (A32 | O32 | 0x7b):
	case 0xe3: case (O32 | 0xe3):				//JCXZ Jb
	case (A32 | 0xe3): case (A32 | O32 | 0xe3):	//JECXZ Jb
		{
			//TODO: Be more precise about which flags are read

			Dword cond = opcode & 0x0f;
			
			if((opcode & ~(O32)) == 0xe3)
				cond = CONDITION_CXZ;
			else if((opcode & ~(O32)) == (A32 | 0xe3))
				cond = CONDITION_ECXZ;
			
			Byte Jb;
			if(!ReadCodeByte(r_eip++, Jb))
				return false;

			Dword instrLength = r_eip - startEip;

			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, instrLength));	//Mov ECX, instrLength
												//Mov EDX, Jb + instrLength (sign-extended)
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, static_cast<SignedByte>(Jb) + instrLength));
			if(cond < 0x10)						//Cmov [cond] ECX, EDX [read F (if rqd)]
				AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			else
				AddOp(TYPE_CMOV, cond, HOST_32, REG_ECX, HOST_32, REG_EDX));
			AddOp(TYPE_ADDEIP, HOST_32, REG_ECX));				//AddEip, ECX
			if((opcode & O32) == 0)								//If O16
				AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	//Jcond Jw
	case 0x0f82: case (A32 | 0x0f82): case 0x0f83: case (A32 | 0x0f83):
	case 0x0f84: case (A32 | 0x0f84): case 0x0f85: case (A32 | 0x0f85):
	case 0x0f86: case (A32 | 0x0f86): case 0x0f87: case (A32 | 0x0f87):
	case 0x0f88: case (A32 | 0x0f88): case 0x0f89: case (A32 | 0x0f89):
	case 0x0f8a: case (A32 | 0x0f8a): case 0x0f8b: case (A32 | 0x0f8b):
		{
			//TODO: Be more precise about which flags are read

			Dword cond = opcode & 0x0f;
			
			Word Jw;
			if(!ReadCodeWord(r_eip, Jw))
				return false;
			r_eip += 2;

			Dword instrLength = r_eip - startEip;

			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, instrLength));	//Mov ECX, instrLength
												//Mov EDX, Jw + instrLength
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, Jw + instrLength));
												//Cmov [cond] ECX, EDX [read F (if rqd)]
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			
			AddOp(TYPE_ADDEIP, HOST_32, REG_ECX));				//AddEip, ECX
			AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	//Jcond Jd
	case (O32 | 0x0f82): case (A32 | O32 | 0x0f82): case (O32 | 0x0f83): case (A32 | O32 | 0x0f83):
	case (O32 | 0x0f84): case (A32 | O32 | 0x0f84): case (O32 | 0x0f85): case (A32 | O32 | 0x0f85):
	case (O32 | 0x0f86): case (A32 | O32 | 0x0f86): case (O32 | 0x0f87): case (A32 | O32 | 0x0f87):
	case (O32 | 0x0f88): case (A32 | O32 | 0x0f88): case (O32 | 0x0f89): case (A32 | O32 | 0x0f89):
	case (O32 | 0x0f8a): case (A32 | O32 | 0x0f8a): case (O32 | 0x0f8b): case (A32 | O32 | 0x0f8b):
		{
			//TODO: Be more precise about which flags are read

			Dword cond = opcode & 0x0f;
			
			Dword Jd;
			if(!ReadCodeDword(r_eip, Jd))
				return false;
			r_eip += 4;

			Dword instrLength = r_eip - startEip;

			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, instrLength));	//Mov ECX, instrLength
												//Mov EDX, Jd + instrLength
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, Jd + instrLength));
												//Cmov [cond] ECX, EDX [read F (if rqd)]
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			
			AddOp(TYPE_ADDEIP, HOST_32, REG_ECX));				//AddEip, ECX

			break;
		}

	case 0xe0: case (A32 | 0xe0): case (O32 | 0xe0): case (A32 | O32 | 0xe0):	//LOOPNE Jb
	case 0xe1: case (A32 | 0xe1): case (O32 | 0xe1): case (A32 | O32 | 0xe1):	//LOOPE Jb
	case 0xe2: case (A32 | 0xe2): case (O32 | 0xe2): case (A32 | O32 | 0xe2):	//LOOP Jb
		{
			Byte Jb;
			if(!ReadCodeByte(r_eip++, Jb))
				return false;

			Dword instrLength = r_eip - startEip;

			OperandType countHostType = ((opcode & A32) == 0) ? HOST_16 : HOST_32;
			OperandType countImmType = ((opcode & A32) == 0) ? IMM_16 : IMM_32;
			Dword cond = ((opcode & A32) == 0) ? CONDITION_CXZ : CONDITION_ECXZ;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_ECX));		//Get ECX, gECX
																	//Sub eCX, 1
			AddOp(TYPE_ALU, ALU_FUNC_SUB, countHostType, REG_ECX, countImmType, 1));
			AddOp(TYPE_PUT, GUEST, REG_ECX, HOST_32, REG_ECX));		//Put gECX, ECX

			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, instrLength));//Mov ECX, instrLength
											//Mov EDX, Jb + instrLength (sign-extended)
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, static_cast<SignedByte>(Jb) + instrLength));

			AddOp(TYPE_CMOV, cond, HOST_32, REG_EDX, HOST_32, REG_ECX));//Cmov [cond] EDX, ECX

			if((opcode & ~(A32 | O32)) == 0xe0)					//If LOOPNE
			{													//Cmov [Z] EDX, ECX
				AddOp(TYPE_CMOV, CONDITION_Z, ZF, 0, HOST_32, REG_EDX, HOST_32, REG_ECX));
			}
			else if((opcode & ~(A32 | O32)) == 0xe1)			//If LOOPE
			{													//Cmov [NZ] EDX, ECX
				AddOp(TYPE_CMOV, CONDITION_NZ, ZF, 0, HOST_32, REG_EDX, HOST_32, REG_ECX));
			}
			
			AddOp(TYPE_ADDEIP, HOST_32, REG_EDX));				//AddEip, EDX
			if((opcode & O32) == 0)								//If O16
				AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	case 0xe8:	case (A32 | 0xe8):	//CALL Jw
		{
			Word Jw;
			if(!ReadCodeWord(r_eip, Jw))
				return false;
			r_eip += 2;

			Dword instrLength = r_eip - startEip;

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
																//Sub eSI, 2
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 2));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EIP));	//Get EBX, gEIP
																//Add EBX, instrLength
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, IMM_32, instrLength));
			AddOp(TYPE_WRITE, HOST_32, REG_EDI, HOST_16, REG_EBX));//Write [EDI], BX
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI
			AddOp(TYPE_ADDEIP, IMM_32, Jw + instrLength));		//AddEip, Jw
			AddOp(TYPE_ANDEIP, IMM_32, 0xffff));				//AndEip, 0xffff

			break;
		}

	case (O32 | 0xe8):	case (A32 | O32 | 0xe8):	//CALL Jd
		{
			Dword Jd;
			if(!ReadCodeDword(r_eip, Jd))
				return false;
			r_eip += 4;

			Dword instrLength = r_eip - startEip;

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
																//Sub eSI, 4
			AddOp(TYPE_ALU, ALU_FUNC_SUB, stackHostType, REG_ESI, stackImmType, 4));
			AddOp(stackMovType, HOST_32, REG_EDI, stackHostType, REG_ESI));	//SMov EDI, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDI, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDI, HOST_32, REG_EBX));

			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_EIP));	//Get EBX, gEIP
																//Add EBX, instrLength
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, IMM_32, instrLength));
			AddOp(TYPE_WRITE, HOST_32, REG_EDI, HOST_32, REG_EBX));//Write [EDI], EBX
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI
			AddOp(TYPE_ADDEIP, IMM_32, Jd + instrLength));		//AddEip, Jd

			break;
		}

	case 0xc2:	case (A32 | 0xc2):	//RET Iw (O16)
	case 0xc3:	case (A32 | 0xc3):	//RET (O16)
		{
			Word Iw = 0;
			if((opcode & ~(A32 | O32)) == 0xc2)
			{
				if(!ReadCodeWord(r_eip, Iw))
					return false;
				r_eip += 2;
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
			AddOp(stackMovType, HOST_32, REG_EDX, stackHostType, REG_ESI));	//SMov EDX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDX, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDX, HOST_32, REG_EBX));

			AddOp(TYPE_READ, HOST_16, REG_EBX, HOST_32, REG_EDX));	//Read BX, [EDX]
			AddOp(TYPE_SETEIP, HOST_16, REG_EBX));				//SetEip BX
																//Add eSI, Iw + 2
			AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, Iw + 2));
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI

			break;
		}

	case (O32 | 0xc2):	case (A32 | O32 | 0xc2):	//RET Iw (O32)
	case (O32 | 0xc3):	case (A32 | O32 | 0xc3):	//RET (O32)
		{
			Word Iw = 0;
			if((opcode & ~(A32 | O32)) == 0xc2)
			{
				if(!ReadCodeWord(r_eip, Iw))
					return false;
				r_eip += 2;
			}

			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));	//Get ESI, gESP
			AddOp(stackMovType, HOST_32, REG_EDX, stackHostType, REG_ESI));	//SMov EDX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EBX, GUEST, REG_SS_BASE));	//Get EBX, SS.BASE
																//Add EDX, EBX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EDX, HOST_32, REG_EBX));

			AddOp(TYPE_READ, HOST_32, REG_EBX, HOST_32, REG_EDX));	//Read EBX, [EDX]
			AddOp(TYPE_SETEIP, HOST_32, REG_EBX));				//SetEip EBX
																//Add eSI, Iw + 4
			AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, Iw + 4));
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));	//Put gESP, ESI

			break;
		}

	//MISC INSTRUCTIONS

	//LEA Gv, M
	case 0x8d00: case (A32 | 0x8d00): case (O32 | 0x8d00): case (A32 | O32 | 0x8d00):
		{
			instructionMayCauseException = false;

			//Effective address (without segment register base adddition) => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd, false))
			{
				return false;
			}

			if((opcode & O32) == 0)
			{
				AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd
				AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EBX));	//Mov CX, BX
				AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX
			}
			else
			{
				AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EBX));			//Put Gd, EBX
			}
			
			break;
		}

	case 0x90: case (A32 | 0x90): case (O32 | 0x90): case (A32 | O32 | 0x90): //NOP
		instructionMayCauseException = false;
		break;

	case 0x98:	case (A32 | 0x98): //CBW
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));			//Get EAX, gEAX
			AddOp(TYPE_SIGNEX, HOST_16, REG_EAX, HOST_L8, REG_EAX));	//Movsx AX, AL
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));			//Put gEAX, EAX

			break;
		}

	case (O32 | 0x98):	case (A32 | O32 | 0x98): //CWDE
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));			//Get EAX, gEAX
			AddOp(TYPE_SIGNEX, HOST_32, REG_EAX, HOST_16, REG_EAX));	//Movsx EAX, AX
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));			//Put gEAX, EAX

			break;
		}

	case 0x99:	case (A32 | 0x99):	//CWD
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));			//Get EAX, gEAX
			AddOp(TYPE_SIGNEX, HOST_32, REG_EAX, HOST_16, REG_EAX));	//Movsx EAX, AX
			AddOp(TYPE_SHIFT, SHIFT_FUNC_SHR, HOST_32, REG_EAX, IMM_8, 16));//Shr EAX, 16
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EDX));			//Get ECX, EDX
			AddOp(TYPE_MOV, HOST_16, REG_ECX, HOST_16, REG_EAX));		//Mov CX, AX
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_ECX));			//Put gEDX, ECX

			break;
		}

	case (O32 | 0x99):	case (A32 | O32 | 0x99):	//CDQ
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));			//Get ECX, gEAX
			AddOp(TYPE_SHIFT, SHIFT_FUNC_SAR, HOST_32, REG_ECX, IMM_8, 31));//Sar ECX, 31
			AddOp(TYPE_PUT, GUEST, REG_EDX, HOST_32, REG_ECX));			//Put gEDX, ECX

			break;
		}

	case 0xc400:	case (A32 | 0xc400):	//LES Gw, Mp
	case 0xc500:	case (A32 | 0xc500):	//LDS Gw, Mp
	case 0x0fb400:	case (A32 | 0x0fb400):	//LFS Gw, Mp
	case 0x0fb500:	case (A32 | 0x0fb500):	//LGS Gw, Mp
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate which segment register is being loaded
			Dword segReg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0xc400:	segReg = REG_ES;	break;
			case 0xc500:	segReg = REG_DS;	break;
			case 0x0fb400:	segReg = REG_FS;	break;
			case 0x0fb500:	segReg = REG_GS;	break;
			}

			if(segReg == REG_DS || segReg == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			assert(segReg != REG_NONE);
			
			AddOp(TYPE_READ, HOST_16, REG_ESI, HOST_32, REG_EBX));		//Read SI, [EBX]
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, IMM_32, 2));//Add EBX, 2
			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));		//Read AX, [EBX]
			AddOp(TYPE_PUTSEG, GUEST_SEG, segReg, HOST_16, REG_EAX));	//PutSeg segReg, AX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Gd));				//Get EDX, Gd
			AddOp(TYPE_MOV, HOST_16, REG_EDX, HOST_16, REG_ESI));		//Mov DX, SI
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_EDX));				//Put Gd, EDX

			break;
		}

	case (O32 | 0xc400):	case (A32 | O32 | 0xc400):		//LES Gd, Mp
	case (O32 | 0xc500):	case (A32 | O32 | 0xc500):		//LDS Gd, Mp
	case (O32 | 0x0fb400):	case (A32 | O32 | 0x0fb400):	//LFS Gd, Mp
	case (O32 | 0x0fb500):	case (A32 | O32 | 0x0fb500):	//LGS Gd, Mp
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			//Calculate which segment register is being loaded
			Dword segReg = REG_NONE;

			switch(opcode & ~(A32 | O32))
			{
			case 0xc400:	segReg = REG_ES;	break;
			case 0xc500:	segReg = REG_DS;	break;
			case 0x0fb400:	segReg = REG_FS;	break;
			case 0x0fb500:	segReg = REG_GS;	break;
			}

			if(segReg == REG_DS || segReg == REG_ES)
				translationMayChangeCsDsEsSsBasesZeroFlag = true;

			assert(segReg != REG_NONE);
			
			AddOp(TYPE_READ, HOST_32, REG_ESI, HOST_32, REG_EBX));		//Read ESI, [EBX]
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, IMM_32, 4));//Add EBX, 4
			AddOp(TYPE_READ, HOST_16, REG_EAX, HOST_32, REG_EBX));		//Read AX, [EBX]
			AddOp(TYPE_PUTSEG, GUEST_SEG, segReg, HOST_16, REG_EAX));	//PutSeg segReg, AX
			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ESI));				//Put Gd, ESI

			break;
		}

	case 0xd7:	case (O32 | 0xd7):	//XLAT (A16)
		{
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EBX));		//Get EDX, gEBX
			AddOp(TYPE_ZEROEX, HOST_32, REG_EBX, HOST_L8, REG_ECX));//ZeroEx EBX, CL
										//Add BX, DX TODO: Correct to limit to 16 bits here?
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_16, REG_EBX, HOST_16, REG_EDX));
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, segRegBase));	//Get EDX, SEG.BASE
																	//Add EBX, EDX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EDX));
			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));	//Read AL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_MOV, HOST_L8, REG_ECX, HOST_L8, REG_EAX));	//Mov CL, AL
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_ECX));		//Put gEAX, ECX

			break;
		}

	case (A32 | 0xd7):	case (A32 | O32 | 0xd7):	//XLAT (A32)
		{
			Dword segRegBase = GetOverrideSegRegBase(prefixes);
			if(segRegBase == REG_NONE)
				segRegBase = REG_DS_BASE;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, REG_EBX));		//Get EDX, gEBX
			AddOp(TYPE_ZEROEX, HOST_32, REG_EBX, HOST_L8, REG_ECX));//ZeroEx EBX, CL
																	//Add EBX, EDX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EDX));
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, segRegBase));	//Get EDX, SEG.BASE
																	//Add EBX, EDX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EDX));
			AddOp(TYPE_READ, HOST_L8, REG_EAX, HOST_32, REG_EBX));	//Read AL, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_MOV, HOST_L8, REG_ECX, HOST_L8, REG_EAX));	//Mov CL, AL
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_ECX));		//Put gEAX, ECX

			break;
		}

	case 0x9e: case (A32 | 0x9e): case (O32 | 0x9e): case (A32 | O32 | 0x9e): //SAHF
		{	
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_GETEFLAGS, ALLF, 0, HOST_32, REG_EDX));		//GetEflags EDX
			AddOp(TYPE_MOV, HOST_L8, REG_EDX, HOST_H8, REG_ECX));	//Mov DL, CH
			AddOp(TYPE_PUTEFLAGS, 0, ALLF, HOST_32, REG_EDX));		//PutEflags EDX

			break;
		}

	case 0x9f: case (A32 | 0x9f): case (O32 | 0x9f): case (A32 | O32 | 0x9f): //LAHF
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, REG_EAX));		//Get ECX, gEAX
			AddOp(TYPE_GETEFLAGS, ALLF, 0, HOST_32, REG_EDX));		//GetEflags EDX
			AddOp(TYPE_MOV, HOST_H8, REG_ECX, HOST_L8, REG_EDX));	//Mov CH, DL
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_ECX));		//Put gEAX, ECX

			break;
		}

	case 0xc9:	case (A32 | 0xc9): //LEAVE O16
		{
			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EBP));		//Get EDI, gEBP
			AddOp(TYPE_MOV, stackHostType, REG_ESI, stackHostType, REG_EDI));//Mov eSI, eDI

			AddOp(stackMovType, HOST_32, REG_EBX, stackHostType, REG_ESI));//SMov EBX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EBX, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EAX));
			AddOp(TYPE_READ, HOST_16, REG_EDI, HOST_32, REG_EBX));	//Read DI, [EBX]

																	//Add eSI, 2
			AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, 2));
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI
			AddOp(TYPE_PUT, GUEST, REG_EBP, HOST_32, REG_EDI));		//Put gEBP, EDI

			break;
		}

	case (O32 | 0xc9):	case (A32 | O32 | 0xc9): //LEAVE O32
		{
			OperandType stackHostType = HOST_16;
			OperandType stackImmType = IMM_16;
			MicroOpType stackMovType = TYPE_ZEROEX;
			if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			{
				stackHostType = HOST_32;
				stackImmType = IMM_32;
				stackMovType = TYPE_MOV;
			}

			AddOp(TYPE_GET, HOST_32, REG_ESI, GUEST, REG_ESP));		//Get ESI, gESP
			AddOp(TYPE_GET, HOST_32, REG_EDI, GUEST, REG_EBP));		//Get EDI, gEBP
			AddOp(TYPE_MOV, stackHostType, REG_ESI, stackHostType, REG_EDI));//Mov eSI, eDI

			AddOp(stackMovType, HOST_32, REG_EBX, stackHostType, REG_ESI));//SMov EBX, eSI
			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_SS_BASE));	//Get EAX, SS.BASE
																	//Add EBX, EAX
			AddOp(TYPE_ALU, ALU_FUNC_ADD, HOST_32, REG_EBX, HOST_32, REG_EAX));
			AddOp(TYPE_READ, HOST_32, REG_EDI, HOST_32, REG_EBX));	//Read EDI, [EBX]

																	//Add eSI, 4
			AddOp(TYPE_ALU, ALU_FUNC_ADD, stackHostType, REG_ESI, stackImmType, 4));
			AddOp(TYPE_PUT, GUEST, REG_ESP, HOST_32, REG_ESI));		//Put gESP, ESI
			AddOp(TYPE_PUT, GUEST, REG_EBP, HOST_32, REG_EDI));		//Put gEBP, EDI

			break;
		}

	case 0xf5: case (A32 | 0xf5): case (O32 | 0xf5): case (A32 | O32 | 0xf5): //CMC
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GETEFLAGS, ALLF, 0, HOST_32, REG_ECX));		//GetEflags ECX
																	//Xor ECX, C_FLAG
			AddOp(TYPE_ALU, ALU_FUNC_XOR, HOST_32, REG_ECX, IMM_32, EFLAGS_C_FLAG));
			AddOp(TYPE_PUTEFLAGS, 0, ALLF, HOST_32, REG_ECX));		//PutEflags ECX

			break;
		}

	case 0xf8: case (A32 | 0xf8): case (O32 | 0xf8): case (A32 | O32 | 0xf8): //CLC
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GETEFLAGS, ALLF, 0, HOST_32, REG_ECX));		//GetEflags ECX
																	//And ECX, ~C_FLAG
			AddOp(TYPE_ALU, ALU_FUNC_AND, HOST_32, REG_ECX, IMM_32, ~EFLAGS_C_FLAG));
			AddOp(TYPE_PUTEFLAGS, 0, ALLF, HOST_32, REG_ECX));		//PutEflags ECX

			break;
		}

	case 0xf9: case (A32 | 0xf9): case (O32 | 0xf9): case (A32 | O32 | 0xf9): //STC
		{
			instructionMayCauseException = false;

			AddOp(TYPE_GETEFLAGS, ALLF, 0, HOST_32, REG_ECX));		//GetEflags ECX
																	//Or ECX, C_FLAG
			AddOp(TYPE_ALU, ALU_FUNC_OR, HOST_32, REG_ECX, IMM_32, EFLAGS_C_FLAG));
			AddOp(TYPE_PUTEFLAGS, 0, ALLF, HOST_32, REG_ECX));		//PutEflags ECX

			break;
		}

	case 0xfc: case (A32 | 0xfc): case (O32 | 0xfc): case (A32 | O32 | 0xfc): //CLD
		{
			instructionMayCauseException = false;

			AddOp(TYPE_CLD));

			break;
		}

	case 0xfd: case (A32 | 0xfd): case (O32 | 0xfd): case (A32 | O32 | 0xfd): //STD
		{
			instructionMayCauseException = false;

			AddOp(TYPE_STD));

			break;
		}

	//SETcc Mb
	case 0x0f9200: case (A32 | 0x0f9200): case (O32 | 0x0f9200): case (A32 | O32 | 0x0f9200):
	case 0x0f9300: case (A32 | 0x0f9300): case (O32 | 0x0f9300): case (A32 | O32 | 0x0f9300):
	case 0x0f9400: case (A32 | 0x0f9400): case (O32 | 0x0f9400): case (A32 | O32 | 0x0f9400):
	case 0x0f9500: case (A32 | 0x0f9500): case (O32 | 0x0f9500): case (A32 | O32 | 0x0f9500):
	case 0x0f9600: case (A32 | 0x0f9600): case (O32 | 0x0f9600): case (A32 | O32 | 0x0f9600):
	case 0x0f9700: case (A32 | 0x0f9700): case (O32 | 0x0f9700): case (A32 | O32 | 0x0f9700):
	case 0x0f9800: case (A32 | 0x0f9800): case (O32 | 0x0f9800): case (A32 | O32 | 0x0f9800):
	case 0x0f9900: case (A32 | 0x0f9900): case (O32 | 0x0f9900): case (A32 | O32 | 0x0f9900):
	case 0x0f9a00: case (A32 | 0x0f9a00): case (O32 | 0x0f9a00): case (A32 | O32 | 0x0f9a00):
	case 0x0f9b00: case (A32 | 0x0f9b00): case (O32 | 0x0f9b00): case (A32 | O32 | 0x0f9b00):
		{
			//TODO: Be more precise about which flags are read
			Dword cond = (opcode & 0x0f00) >> 8;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}
			
			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, 0));	//Mov ECX, 0
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, 1));	//Mov EDX, 1
															//Cmov [cond] ECX, EDX [read F]
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, HOST_L8, REG_ECX));	//Write [EBX], CL

			break;
		}

	//SETcc Rb
	case 0x0f92c0: case (A32 | 0x0f92c0): case (O32 | 0x0f92c0): case (A32 | O32 | 0x0f92c0):
	case 0x0f93c0: case (A32 | 0x0f93c0): case (O32 | 0x0f93c0): case (A32 | O32 | 0x0f93c0):
	case 0x0f94c0: case (A32 | 0x0f94c0): case (O32 | 0x0f94c0): case (A32 | O32 | 0x0f94c0):
	case 0x0f95c0: case (A32 | 0x0f95c0): case (O32 | 0x0f95c0): case (A32 | O32 | 0x0f95c0):
	case 0x0f96c0: case (A32 | 0x0f96c0): case (O32 | 0x0f96c0): case (A32 | O32 | 0x0f96c0):
	case 0x0f97c0: case (A32 | 0x0f97c0): case (O32 | 0x0f97c0): case (A32 | O32 | 0x0f97c0):
	case 0x0f98c0: case (A32 | 0x0f98c0): case (O32 | 0x0f98c0): case (A32 | O32 | 0x0f98c0):
	case 0x0f99c0: case (A32 | 0x0f99c0): case (O32 | 0x0f99c0): case (A32 | O32 | 0x0f99c0):
	case 0x0f9ac0: case (A32 | 0x0f9ac0): case (O32 | 0x0f9ac0): case (A32 | O32 | 0x0f9ac0):
	case 0x0f9bc0: case (A32 | 0x0f9bc0): case (O32 | 0x0f9bc0): case (A32 | O32 | 0x0f9bc0):
		{
			instructionMayCauseException = false;

			//TODO: Be more precise about which flags are read
			Dword cond = (opcode & 0x0f00) >> 8;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			//Calculate Rb
			OperandType hostRegType;
			Dword Rb;
			Get8BitRegFromReg(Rd, hostRegType, Rb);
			
			AddOp(TYPE_MOV, HOST_32, REG_ECX, IMM_32, 0));	//Mov ECX, 0
			AddOp(TYPE_MOV, HOST_32, REG_EDX, IMM_32, 1));	//Mov EDX, 1
															//Cmov [cond] ECX, EDX [read F]
			AddOp(TYPE_CMOV, cond, ALLF, 0, HOST_32, REG_ECX, HOST_32, REG_EDX));
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rb));	//Get EDX, Rb_32
			AddOp(TYPE_MOV, hostRegType, REG_EDX, HOST_L8, REG_ECX));//Mov {DL|DH}, CL
			AddOp(TYPE_PUT, GUEST, Rb, HOST_32, REG_EDX));	//Put Rb_32, EDX

			break;
		}

	//BSWAP regd
	case 0x0fc8: case (A32 | 0x0fc8): case (O32 | 0x0fc8): case (A32 | O32 | 0x0fc8):
	case 0x0fc9: case (A32 | 0x0fc9): case (O32 | 0x0fc9): case (A32 | O32 | 0x0fc9):
	case 0x0fca: case (A32 | 0x0fca): case (O32 | 0x0fca): case (A32 | O32 | 0x0fca):
	case 0x0fcb: case (A32 | 0x0fcb): case (O32 | 0x0fcb): case (A32 | O32 | 0x0fcb):
	case 0x0fcc: case (A32 | 0x0fcc): case (O32 | 0x0fcc): case (A32 | O32 | 0x0fcc):
	case 0x0fcd: case (A32 | 0x0fcd): case (O32 | 0x0fcd): case (A32 | O32 | 0x0fcd):
	case 0x0fce: case (A32 | 0x0fce): case (O32 | 0x0fce): case (A32 | O32 | 0x0fce):
	case 0x0fcf: case (A32 | 0x0fcf): case (O32 | 0x0fcf): case (A32 | O32 | 0x0fcf):
		{
			instructionMayCauseException = false;

			Dword reg = REG_EAX + (opcode & 0x07);

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, reg));		//Get ECX, reg
			AddOp(TYPE_BSWAP, HOST_32, REG_ECX));				//Bswap ECX
			AddOp(TYPE_PUT, GUEST, reg, HOST_32, REG_ECX));		//Put reg, ECX

			break;
		}

	//BSF, BSR Gw, Mw
	case (0x0fbc00):	case (A32 | 0x0fbc00):	case (0x0fbd00):	case (A32 | 0x0fbd00):
	//BSF, BSR Gd, Md
	case (O32 | 0x0fbc00):	case (A32 | O32 | 0x0fbc00):
	case (O32 | 0x0fbd00):	case (A32 | O32 | 0x0fbd00):
		{
			Dword bitScanFunc = (opcode & 0x0100) >> 8;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, hostType, REG_EDX, HOST_32, REG_EBX));	//Read eDX, [EBX]
			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));			//Get ECX, Gd

																	//BitScan eCX, eDX [F]
			AddOp(TYPE_BIT_SCAN, bitScanFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));

			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));			//Put Gd, ECX

			break;
		}

	//BSF, BSR Gw, Rw
	case (0x0fbcc0):	case (A32 | 0x0fbcc0):	case (0x0fbdc0):	case (A32 | 0x0fbdc0):
	//BSF, BSR Gd, Rd
	case (O32 | 0x0fbcc0):	case (A32 | O32 | 0x0fbcc0):
	case (O32 | 0x0fbdc0):	case (A32 | O32 | 0x0fbdc0):
		{
			instructionMayCauseException = false;

			Dword bitScanFunc = (opcode & 0x0100) >> 8;
			OperandType hostType = ((opcode & O32) == 0) ? HOST_16 : HOST_32;

			Dword Gd, Rd;
			if(!DecodeModRMByteRegReg(r_eip, Gd, Rd))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_ECX, GUEST, Gd));		//Get ECX, Gd
			AddOp(TYPE_GET, HOST_32, REG_EDX, GUEST, Rd));		//Get EDX, Rd
			
																//BitScan eCX, eDX [F]
			AddOp(TYPE_BIT_SCAN, bitScanFunc, 0, ALLF, hostType, REG_ECX, hostType, REG_EDX));

			AddOp(TYPE_PUT, GUEST, Gd, HOST_32, REG_ECX));		//Put Gd, ECX

			break;
		}

	//DAA, DAS, AAA, AAS
	case 0x27: case (A32 | 0x27): case (O32 | 0x27): case (A32 | O32 | 0x27):
	case 0x2f: case (A32 | 0x2f): case (O32 | 0x2f): case (A32 | O32 | 0x2f):
	case 0x37: case (A32 | 0x37): case (O32 | 0x37): case (A32 | O32 | 0x37):
	case 0x3f: case (A32 | 0x3f): case (O32 | 0x3f): case (A32 | O32 | 0x3f):
		{
			instructionMayCauseException = false;

			Dword asAdjustFunc = (opcode & 0x18) >> 3;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));				//Get EAX, gEAX
			AddOp(TYPE_AS_ADJUST, asAdjustFunc, ALLF, ALLF, HOST_32, REG_EAX));//ASAdjust EAX
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));				//Put gEAX, EAX

			break;
		}

	//AAM, AAD
	case 0xd4: case (A32 | 0xd4): case (O32 | 0xd4): case (A32 | O32 | 0xd4):
	case 0xd5: case (A32 | 0xd5): case (O32 | 0xd5): case (A32 | O32 | 0xd5):
		{
			instructionMayCauseException = false;

			Dword mdAdjustFunc = opcode & 0x01;

			Byte Ib;
			if(!ReadCodeByte(r_eip++, Ib))
				return false;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));				//Get EAX, gEAX
																		//MDAdjust EAX, Ib
			AddOp(TYPE_MD_ADJUST, mdAdjustFunc, 0, ALLF, HOST_32, REG_EAX, IMM_8, Ib));
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));				//Put gEAX, EAX

			break;
		}

	case 0x9b: case (A32 | 0x9b): case (O32 | 0x9b): case (A32 | O32 | 0x9b): //WAIT
		{
			AddOp(TYPE_WAIT));

			break;
		}

	//FPU INSTRUCTIONS

	//FIADD, FIMUL, FICOM, FICOMP, FISUB, FISUBR, FIDIV, FIDIVR word
	case 0xde00: case (A32 | 0xde00): case (O32 | 0xde00): case (A32 | O32 | 0xde00):
	case 0xde08: case (A32 | 0xde08): case (O32 | 0xde08): case (A32 | O32 | 0xde08):
	case 0xde10: case (A32 | 0xde10): case (O32 | 0xde10): case (A32 | O32 | 0xde10):
	case 0xde18: case (A32 | 0xde18): case (O32 | 0xde18): case (A32 | O32 | 0xde18):
	case 0xde20: case (A32 | 0xde20): case (O32 | 0xde20): case (A32 | O32 | 0xde20):
	case 0xde28: case (A32 | 0xde28): case (O32 | 0xde28): case (A32 | O32 | 0xde28):
	case 0xde30: case (A32 | 0xde30): case (O32 | 0xde30): case (A32 | O32 | 0xde30):
	case 0xde38: case (A32 | 0xde38): case (O32 | 0xde38): case (A32 | O32 | 0xde38):
	//FILD word
	case 0xdf00: case (A32 | 0xdf00): case (O32 | 0xdf00): case (A32 | O32 | 0xdf00):
	//FLDCW word
	case 0xd928: case (A32 | 0xd928): case (O32 | 0xd928): case (A32 | O32 | 0xd928):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, MEM_16, 0, HOST_32, REG_EBX));	//Read MEM_16, [EBX]
			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu

			break;
		}

	//FIST, FISTP word
	case 0xdf10: case (A32 | 0xdf10): case (O32 | 0xdf10): case (A32 | O32 | 0xdf10):
	case 0xdf18: case (A32 | 0xdf18): case (O32 | 0xdf18): case (A32 | O32 | 0xdf18):
	//FNSTCW word
	case 0xd938: case (A32 | 0xd938): case (O32 | 0xd938): case (A32 | O32 | 0xd938):
	//FNSTSW word
	case 0xdd38: case (A32 | 0xdd38): case (O32 | 0xdd38): case (A32 | O32 | 0xdd38):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, MEM_16, 0));//Write [EBX], MEM_16

			break;
		}

	//FADD, FMUL, FCOM, FCOMP, FSUB, FSUBR, FDIV, FDIVR single
	case 0xd800: case (A32 | 0xd800): case (O32 | 0xd800): case (A32 | O32 | 0xd800):
	case 0xd808: case (A32 | 0xd808): case (O32 | 0xd808): case (A32 | O32 | 0xd808):
	case 0xd810: case (A32 | 0xd810): case (O32 | 0xd810): case (A32 | O32 | 0xd810):
	case 0xd818: case (A32 | 0xd818): case (O32 | 0xd818): case (A32 | O32 | 0xd818):
	case 0xd820: case (A32 | 0xd820): case (O32 | 0xd820): case (A32 | O32 | 0xd820):
	case 0xd828: case (A32 | 0xd828): case (O32 | 0xd828): case (A32 | O32 | 0xd828):
	case 0xd830: case (A32 | 0xd830): case (O32 | 0xd830): case (A32 | O32 | 0xd830):
	case 0xd838: case (A32 | 0xd838): case (O32 | 0xd838): case (A32 | O32 | 0xd838):
	//FLD single
	case 0xd900: case (A32 | 0xd900): case (O32 | 0xd900): case (A32 | O32 | 0xd900):
	//FIADD, FIMUL, FICOM, FICOMP, FISUB, FISUBR, FIDIV, FIDIVR dword
	case 0xda00: case (A32 | 0xda00): case (O32 | 0xda00): case (A32 | O32 | 0xda00):
	case 0xda08: case (A32 | 0xda08): case (O32 | 0xda08): case (A32 | O32 | 0xda08):
	case 0xda10: case (A32 | 0xda10): case (O32 | 0xda10): case (A32 | O32 | 0xda10):
	case 0xda18: case (A32 | 0xda18): case (O32 | 0xda18): case (A32 | O32 | 0xda18):
	case 0xda20: case (A32 | 0xda20): case (O32 | 0xda20): case (A32 | O32 | 0xda20):
	case 0xda28: case (A32 | 0xda28): case (O32 | 0xda28): case (A32 | O32 | 0xda28):
	case 0xda30: case (A32 | 0xda30): case (O32 | 0xda30): case (A32 | O32 | 0xda30):
	case 0xda38: case (A32 | 0xda38): case (O32 | 0xda38): case (A32 | O32 | 0xda38):
	//FILD dword
	case 0xdb00: case (A32 | 0xdb00): case (O32 | 0xdb00): case (A32 | O32 | 0xdb00):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, MEM_32, 0, HOST_32, REG_EBX));	//Read MEM_32, [EBX]
			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu

			break;
		}

	//FST, FSTP single
	case 0xd910: case (A32 | 0xd910): case (O32 | 0xd910): case (A32 | O32 | 0xd910):
	case 0xd918: case (A32 | 0xd918): case (O32 | 0xd918): case (A32 | O32 | 0xd918):
	//FIST, FISTP dword
	case 0xdb10: case (A32 | 0xdb10): case (O32 | 0xdb10): case (A32 | O32 | 0xdb10):
	case 0xdb18: case (A32 | 0xdb18): case (O32 | 0xdb18): case (A32 | O32 | 0xdb18):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, MEM_32, 0));//Write [EBX], MEM_32

			break;
		}

	//FADD, FMUL, FCOM, FCOMP, FSUB, FSUBR, FDIV, FDIVR double
	case 0xdc00: case (A32 | 0xdc00): case (O32 | 0xdc00): case (A32 | O32 | 0xdc00):
	case 0xdc08: case (A32 | 0xdc08): case (O32 | 0xdc08): case (A32 | O32 | 0xdc08):
	case 0xdc10: case (A32 | 0xdc10): case (O32 | 0xdc10): case (A32 | O32 | 0xdc10):
	case 0xdc18: case (A32 | 0xdc18): case (O32 | 0xdc18): case (A32 | O32 | 0xdc18):
	case 0xdc20: case (A32 | 0xdc20): case (O32 | 0xdc20): case (A32 | O32 | 0xdc20):
	case 0xdc28: case (A32 | 0xdc28): case (O32 | 0xdc28): case (A32 | O32 | 0xdc28):
	case 0xdc30: case (A32 | 0xdc30): case (O32 | 0xdc30): case (A32 | O32 | 0xdc30):
	case 0xdc38: case (A32 | 0xdc38): case (O32 | 0xdc38): case (A32 | O32 | 0xdc38):
	//FLD double
	case 0xdd00: case (A32 | 0xdd00): case (O32 | 0xdd00): case (A32 | O32 | 0xdd00):
	//FILD qword
	case 0xdf28: case (A32 | 0xdf28): case (O32 | 0xdf28): case (A32 | O32 | 0xdf28):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, MEM_64, 0, HOST_32, REG_EBX));	//Read MEM_64, [EBX]
			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu

			break;
		}

	//FST, FSTP double
	case 0xdd10: case (A32 | 0xdd10): case (O32 | 0xdd10): case (A32 | O32 | 0xdd10):
	case 0xdd18: case (A32 | 0xdd18): case (O32 | 0xdd18): case (A32 | O32 | 0xdd18):
	//FISTP qword
	case 0xdf38: case (A32 | 0xdf38): case (O32 | 0xdf38): case (A32 | O32 | 0xdf38):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, MEM_64, 0));//Write [EBX], MEM_64

			break;
		}

	//FLD extended
	case 0xdb28: case (A32 | 0xdb28): case (O32 | 0xdb28): case (A32 | O32 | 0xdb28):
	//FBLD BCD
	case 0xdf20: case (A32 | 0xdf20): case (O32 | 0xdf20): case (A32 | O32 | 0xdf20):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_READ, MEM_80, 0, HOST_32, REG_EBX));	//Read MEM_80, [EBX]
			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu

			break;
		}

	//FSTP extended
	case 0xdb38: case (A32 | 0xdb38): case (O32 | 0xdb38): case (A32 | O32 | 0xdb38):
	//FBSTP BCD
	case 0xdf30: case (A32 | 0xdf30): case (O32 | 0xdf30): case (A32 | O32 | 0xdf30):
		{
			//Effective address => EBX
			Dword Gd;
			if(	!OutputEffectiveAddressCalculation(	opcode, prefixes,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													r_eip, microCode, Gd))
			{
				return false;
			}

			AddOp(TYPE_MEMORY_FPU, opcode & 0x7ff));		//MemoryFpu
			AddOp(TYPE_WRITE, HOST_32, REG_EBX, MEM_80, 0));//Write [EBX], MEM_80

			break;
		}

	//Valid register FPU instructions
	case 0xd8c0: case (A32 | 0xd8c0): case (O32 | 0xd8c0): case (A32 | O32 | 0xd8c0):
	case 0xd8c1: case (A32 | 0xd8c1): case (O32 | 0xd8c1): case (A32 | O32 | 0xd8c1):
	case 0xd8c2: case (A32 | 0xd8c2): case (O32 | 0xd8c2): case (A32 | O32 | 0xd8c2):
	case 0xd8c3: case (A32 | 0xd8c3): case (O32 | 0xd8c3): case (A32 | O32 | 0xd8c3):
	case 0xd8c4: case (A32 | 0xd8c4): case (O32 | 0xd8c4): case (A32 | O32 | 0xd8c4):
	case 0xd8c5: case (A32 | 0xd8c5): case (O32 | 0xd8c5): case (A32 | O32 | 0xd8c5):
	case 0xd8c6: case (A32 | 0xd8c6): case (O32 | 0xd8c6): case (A32 | O32 | 0xd8c6):
	case 0xd8c7: case (A32 | 0xd8c7): case (O32 | 0xd8c7): case (A32 | O32 | 0xd8c7):
	case 0xd8c8: case (A32 | 0xd8c8): case (O32 | 0xd8c8): case (A32 | O32 | 0xd8c8):
	case 0xd8c9: case (A32 | 0xd8c9): case (O32 | 0xd8c9): case (A32 | O32 | 0xd8c9):
	case 0xd8ca: case (A32 | 0xd8ca): case (O32 | 0xd8ca): case (A32 | O32 | 0xd8ca):
	case 0xd8cb: case (A32 | 0xd8cb): case (O32 | 0xd8cb): case (A32 | O32 | 0xd8cb):
	case 0xd8cc: case (A32 | 0xd8cc): case (O32 | 0xd8cc): case (A32 | O32 | 0xd8cc):
	case 0xd8cd: case (A32 | 0xd8cd): case (O32 | 0xd8cd): case (A32 | O32 | 0xd8cd):
	case 0xd8ce: case (A32 | 0xd8ce): case (O32 | 0xd8ce): case (A32 | O32 | 0xd8ce):
	case 0xd8cf: case (A32 | 0xd8cf): case (O32 | 0xd8cf): case (A32 | O32 | 0xd8cf):
	case 0xd8d0: case (A32 | 0xd8d0): case (O32 | 0xd8d0): case (A32 | O32 | 0xd8d0):
	case 0xd8d1: case (A32 | 0xd8d1): case (O32 | 0xd8d1): case (A32 | O32 | 0xd8d1):
	case 0xd8d2: case (A32 | 0xd8d2): case (O32 | 0xd8d2): case (A32 | O32 | 0xd8d2):
	case 0xd8d3: case (A32 | 0xd8d3): case (O32 | 0xd8d3): case (A32 | O32 | 0xd8d3):
	case 0xd8d4: case (A32 | 0xd8d4): case (O32 | 0xd8d4): case (A32 | O32 | 0xd8d4):
	case 0xd8d5: case (A32 | 0xd8d5): case (O32 | 0xd8d5): case (A32 | O32 | 0xd8d5):
	case 0xd8d6: case (A32 | 0xd8d6): case (O32 | 0xd8d6): case (A32 | O32 | 0xd8d6):
	case 0xd8d7: case (A32 | 0xd8d7): case (O32 | 0xd8d7): case (A32 | O32 | 0xd8d7):
	case 0xd8d8: case (A32 | 0xd8d8): case (O32 | 0xd8d8): case (A32 | O32 | 0xd8d8):
	case 0xd8d9: case (A32 | 0xd8d9): case (O32 | 0xd8d9): case (A32 | O32 | 0xd8d9):
	case 0xd8da: case (A32 | 0xd8da): case (O32 | 0xd8da): case (A32 | O32 | 0xd8da):
	case 0xd8db: case (A32 | 0xd8db): case (O32 | 0xd8db): case (A32 | O32 | 0xd8db):
	case 0xd8dc: case (A32 | 0xd8dc): case (O32 | 0xd8dc): case (A32 | O32 | 0xd8dc):
	case 0xd8dd: case (A32 | 0xd8dd): case (O32 | 0xd8dd): case (A32 | O32 | 0xd8dd):
	case 0xd8de: case (A32 | 0xd8de): case (O32 | 0xd8de): case (A32 | O32 | 0xd8de):
	case 0xd8df: case (A32 | 0xd8df): case (O32 | 0xd8df): case (A32 | O32 | 0xd8df):
	case 0xd8e0: case (A32 | 0xd8e0): case (O32 | 0xd8e0): case (A32 | O32 | 0xd8e0):
	case 0xd8e1: case (A32 | 0xd8e1): case (O32 | 0xd8e1): case (A32 | O32 | 0xd8e1):
	case 0xd8e2: case (A32 | 0xd8e2): case (O32 | 0xd8e2): case (A32 | O32 | 0xd8e2):
	case 0xd8e3: case (A32 | 0xd8e3): case (O32 | 0xd8e3): case (A32 | O32 | 0xd8e3):
	case 0xd8e4: case (A32 | 0xd8e4): case (O32 | 0xd8e4): case (A32 | O32 | 0xd8e4):
	case 0xd8e5: case (A32 | 0xd8e5): case (O32 | 0xd8e5): case (A32 | O32 | 0xd8e5):
	case 0xd8e6: case (A32 | 0xd8e6): case (O32 | 0xd8e6): case (A32 | O32 | 0xd8e6):
	case 0xd8e7: case (A32 | 0xd8e7): case (O32 | 0xd8e7): case (A32 | O32 | 0xd8e7):
	case 0xd8e8: case (A32 | 0xd8e8): case (O32 | 0xd8e8): case (A32 | O32 | 0xd8e8):
	case 0xd8e9: case (A32 | 0xd8e9): case (O32 | 0xd8e9): case (A32 | O32 | 0xd8e9):
	case 0xd8ea: case (A32 | 0xd8ea): case (O32 | 0xd8ea): case (A32 | O32 | 0xd8ea):
	case 0xd8eb: case (A32 | 0xd8eb): case (O32 | 0xd8eb): case (A32 | O32 | 0xd8eb):
	case 0xd8ec: case (A32 | 0xd8ec): case (O32 | 0xd8ec): case (A32 | O32 | 0xd8ec):
	case 0xd8ed: case (A32 | 0xd8ed): case (O32 | 0xd8ed): case (A32 | O32 | 0xd8ed):
	case 0xd8ee: case (A32 | 0xd8ee): case (O32 | 0xd8ee): case (A32 | O32 | 0xd8ee):
	case 0xd8ef: case (A32 | 0xd8ef): case (O32 | 0xd8ef): case (A32 | O32 | 0xd8ef):
	case 0xd8f0: case (A32 | 0xd8f0): case (O32 | 0xd8f0): case (A32 | O32 | 0xd8f0):
	case 0xd8f1: case (A32 | 0xd8f1): case (O32 | 0xd8f1): case (A32 | O32 | 0xd8f1):
	case 0xd8f2: case (A32 | 0xd8f2): case (O32 | 0xd8f2): case (A32 | O32 | 0xd8f2):
	case 0xd8f3: case (A32 | 0xd8f3): case (O32 | 0xd8f3): case (A32 | O32 | 0xd8f3):
	case 0xd8f4: case (A32 | 0xd8f4): case (O32 | 0xd8f4): case (A32 | O32 | 0xd8f4):
	case 0xd8f5: case (A32 | 0xd8f5): case (O32 | 0xd8f5): case (A32 | O32 | 0xd8f5):
	case 0xd8f6: case (A32 | 0xd8f6): case (O32 | 0xd8f6): case (A32 | O32 | 0xd8f6):
	case 0xd8f7: case (A32 | 0xd8f7): case (O32 | 0xd8f7): case (A32 | O32 | 0xd8f7):
	case 0xd8f8: case (A32 | 0xd8f8): case (O32 | 0xd8f8): case (A32 | O32 | 0xd8f8):
	case 0xd8f9: case (A32 | 0xd8f9): case (O32 | 0xd8f9): case (A32 | O32 | 0xd8f9):
	case 0xd8fa: case (A32 | 0xd8fa): case (O32 | 0xd8fa): case (A32 | O32 | 0xd8fa):
	case 0xd8fb: case (A32 | 0xd8fb): case (O32 | 0xd8fb): case (A32 | O32 | 0xd8fb):
	case 0xd8fc: case (A32 | 0xd8fc): case (O32 | 0xd8fc): case (A32 | O32 | 0xd8fc):
	case 0xd8fd: case (A32 | 0xd8fd): case (O32 | 0xd8fd): case (A32 | O32 | 0xd8fd):
	case 0xd8fe: case (A32 | 0xd8fe): case (O32 | 0xd8fe): case (A32 | O32 | 0xd8fe):
	case 0xd8ff: case (A32 | 0xd8ff): case (O32 | 0xd8ff): case (A32 | O32 | 0xd8ff):
	
	case 0xd9c0: case (A32 | 0xd9c0): case (O32 | 0xd9c0): case (A32 | O32 | 0xd9c0):
	case 0xd9c1: case (A32 | 0xd9c1): case (O32 | 0xd9c1): case (A32 | O32 | 0xd9c1):
	case 0xd9c2: case (A32 | 0xd9c2): case (O32 | 0xd9c2): case (A32 | O32 | 0xd9c2):
	case 0xd9c3: case (A32 | 0xd9c3): case (O32 | 0xd9c3): case (A32 | O32 | 0xd9c3):
	case 0xd9c4: case (A32 | 0xd9c4): case (O32 | 0xd9c4): case (A32 | O32 | 0xd9c4):
	case 0xd9c5: case (A32 | 0xd9c5): case (O32 | 0xd9c5): case (A32 | O32 | 0xd9c5):
	case 0xd9c6: case (A32 | 0xd9c6): case (O32 | 0xd9c6): case (A32 | O32 | 0xd9c6):
	case 0xd9c7: case (A32 | 0xd9c7): case (O32 | 0xd9c7): case (A32 | O32 | 0xd9c7):
	case 0xd9c8: case (A32 | 0xd9c8): case (O32 | 0xd9c8): case (A32 | O32 | 0xd9c8):
	case 0xd9c9: case (A32 | 0xd9c9): case (O32 | 0xd9c9): case (A32 | O32 | 0xd9c9):
	case 0xd9ca: case (A32 | 0xd9ca): case (O32 | 0xd9ca): case (A32 | O32 | 0xd9ca):
	case 0xd9cb: case (A32 | 0xd9cb): case (O32 | 0xd9cb): case (A32 | O32 | 0xd9cb):
	case 0xd9cc: case (A32 | 0xd9cc): case (O32 | 0xd9cc): case (A32 | O32 | 0xd9cc):
	case 0xd9cd: case (A32 | 0xd9cd): case (O32 | 0xd9cd): case (A32 | O32 | 0xd9cd):
	case 0xd9ce: case (A32 | 0xd9ce): case (O32 | 0xd9ce): case (A32 | O32 | 0xd9ce):
	case 0xd9cf: case (A32 | 0xd9cf): case (O32 | 0xd9cf): case (A32 | O32 | 0xd9cf):
	case 0xd9d0: case (A32 | 0xd9d0): case (O32 | 0xd9d0): case (A32 | O32 | 0xd9d0):
	case 0xd9e0: case (A32 | 0xd9e0): case (O32 | 0xd9e0): case (A32 | O32 | 0xd9e0):
	case 0xd9e1: case (A32 | 0xd9e1): case (O32 | 0xd9e1): case (A32 | O32 | 0xd9e1):
	case 0xd9e4: case (A32 | 0xd9e4): case (O32 | 0xd9e4): case (A32 | O32 | 0xd9e4):
	case 0xd9e5: case (A32 | 0xd9e5): case (O32 | 0xd9e5): case (A32 | O32 | 0xd9e5):
	case 0xd9e8: case (A32 | 0xd9e8): case (O32 | 0xd9e8): case (A32 | O32 | 0xd9e8):
	case 0xd9e9: case (A32 | 0xd9e9): case (O32 | 0xd9e9): case (A32 | O32 | 0xd9e9):
	case 0xd9ea: case (A32 | 0xd9ea): case (O32 | 0xd9ea): case (A32 | O32 | 0xd9ea):
	case 0xd9eb: case (A32 | 0xd9eb): case (O32 | 0xd9eb): case (A32 | O32 | 0xd9eb):
	case 0xd9ec: case (A32 | 0xd9ec): case (O32 | 0xd9ec): case (A32 | O32 | 0xd9ec):
	case 0xd9ed: case (A32 | 0xd9ed): case (O32 | 0xd9ed): case (A32 | O32 | 0xd9ed):
	case 0xd9ee: case (A32 | 0xd9ee): case (O32 | 0xd9ee): case (A32 | O32 | 0xd9ee):
	case 0xd9f0: case (A32 | 0xd9f0): case (O32 | 0xd9f0): case (A32 | O32 | 0xd9f0):
	case 0xd9f1: case (A32 | 0xd9f1): case (O32 | 0xd9f1): case (A32 | O32 | 0xd9f1):
	case 0xd9f2: case (A32 | 0xd9f2): case (O32 | 0xd9f2): case (A32 | O32 | 0xd9f2):
	case 0xd9f3: case (A32 | 0xd9f3): case (O32 | 0xd9f3): case (A32 | O32 | 0xd9f3):
	case 0xd9f4: case (A32 | 0xd9f4): case (O32 | 0xd9f4): case (A32 | O32 | 0xd9f4):
	case 0xd9f5: case (A32 | 0xd9f5): case (O32 | 0xd9f5): case (A32 | O32 | 0xd9f5):
	case 0xd9f6: case (A32 | 0xd9f6): case (O32 | 0xd9f6): case (A32 | O32 | 0xd9f6):
	case 0xd9f7: case (A32 | 0xd9f7): case (O32 | 0xd9f7): case (A32 | O32 | 0xd9f7):
	case 0xd9f8: case (A32 | 0xd9f8): case (O32 | 0xd9f8): case (A32 | O32 | 0xd9f8):
	case 0xd9f9: case (A32 | 0xd9f9): case (O32 | 0xd9f9): case (A32 | O32 | 0xd9f9):
	case 0xd9fa: case (A32 | 0xd9fa): case (O32 | 0xd9fa): case (A32 | O32 | 0xd9fa):
	case 0xd9fb: case (A32 | 0xd9fb): case (O32 | 0xd9fb): case (A32 | O32 | 0xd9fb):
	case 0xd9fc: case (A32 | 0xd9fc): case (O32 | 0xd9fc): case (A32 | O32 | 0xd9fc):
	case 0xd9fd: case (A32 | 0xd9fd): case (O32 | 0xd9fd): case (A32 | O32 | 0xd9fd):
	case 0xd9fe: case (A32 | 0xd9fe): case (O32 | 0xd9fe): case (A32 | O32 | 0xd9fe):
	case 0xd9ff: case (A32 | 0xd9ff): case (O32 | 0xd9ff): case (A32 | O32 | 0xd9ff):

	case 0xdae9: case (A32 | 0xdae9): case (O32 | 0xdae9): case (A32 | O32 | 0xdae9):

	case 0xdbe0: case (A32 | 0xdbe0): case (O32 | 0xdbe0): case (A32 | O32 | 0xdbe0):
	case 0xdbe1: case (A32 | 0xdbe1): case (O32 | 0xdbe1): case (A32 | O32 | 0xdbe1):
	case 0xdbe2: case (A32 | 0xdbe2): case (O32 | 0xdbe2): case (A32 | O32 | 0xdbe2):
	case 0xdbe3: case (A32 | 0xdbe3): case (O32 | 0xdbe3): case (A32 | O32 | 0xdbe3):
	case 0xdbe4: case (A32 | 0xdbe4): case (O32 | 0xdbe4): case (A32 | O32 | 0xdbe4):

	case 0xdcc0: case (A32 | 0xdcc0): case (O32 | 0xdcc0): case (A32 | O32 | 0xdcc0):
	case 0xdcc1: case (A32 | 0xdcc1): case (O32 | 0xdcc1): case (A32 | O32 | 0xdcc1):
	case 0xdcc2: case (A32 | 0xdcc2): case (O32 | 0xdcc2): case (A32 | O32 | 0xdcc2):
	case 0xdcc3: case (A32 | 0xdcc3): case (O32 | 0xdcc3): case (A32 | O32 | 0xdcc3):
	case 0xdcc4: case (A32 | 0xdcc4): case (O32 | 0xdcc4): case (A32 | O32 | 0xdcc4):
	case 0xdcc5: case (A32 | 0xdcc5): case (O32 | 0xdcc5): case (A32 | O32 | 0xdcc5):
	case 0xdcc6: case (A32 | 0xdcc6): case (O32 | 0xdcc6): case (A32 | O32 | 0xdcc6):
	case 0xdcc7: case (A32 | 0xdcc7): case (O32 | 0xdcc7): case (A32 | O32 | 0xdcc7):
	case 0xdcc8: case (A32 | 0xdcc8): case (O32 | 0xdcc8): case (A32 | O32 | 0xdcc8):
	case 0xdcc9: case (A32 | 0xdcc9): case (O32 | 0xdcc9): case (A32 | O32 | 0xdcc9):
	case 0xdcca: case (A32 | 0xdcca): case (O32 | 0xdcca): case (A32 | O32 | 0xdcca):
	case 0xdccb: case (A32 | 0xdccb): case (O32 | 0xdccb): case (A32 | O32 | 0xdccb):
	case 0xdccc: case (A32 | 0xdccc): case (O32 | 0xdccc): case (A32 | O32 | 0xdccc):
	case 0xdccd: case (A32 | 0xdccd): case (O32 | 0xdccd): case (A32 | O32 | 0xdccd):
	case 0xdcce: case (A32 | 0xdcce): case (O32 | 0xdcce): case (A32 | O32 | 0xdcce):
	case 0xdccf: case (A32 | 0xdccf): case (O32 | 0xdccf): case (A32 | O32 | 0xdccf):
	case 0xdce0: case (A32 | 0xdce0): case (O32 | 0xdce0): case (A32 | O32 | 0xdce0):
	case 0xdce1: case (A32 | 0xdce1): case (O32 | 0xdce1): case (A32 | O32 | 0xdce1):
	case 0xdce2: case (A32 | 0xdce2): case (O32 | 0xdce2): case (A32 | O32 | 0xdce2):
	case 0xdce3: case (A32 | 0xdce3): case (O32 | 0xdce3): case (A32 | O32 | 0xdce3):
	case 0xdce4: case (A32 | 0xdce4): case (O32 | 0xdce4): case (A32 | O32 | 0xdce4):
	case 0xdce5: case (A32 | 0xdce5): case (O32 | 0xdce5): case (A32 | O32 | 0xdce5):
	case 0xdce6: case (A32 | 0xdce6): case (O32 | 0xdce6): case (A32 | O32 | 0xdce6):
	case 0xdce7: case (A32 | 0xdce7): case (O32 | 0xdce7): case (A32 | O32 | 0xdce7):
	case 0xdce8: case (A32 | 0xdce8): case (O32 | 0xdce8): case (A32 | O32 | 0xdce8):
	case 0xdce9: case (A32 | 0xdce9): case (O32 | 0xdce9): case (A32 | O32 | 0xdce9):
	case 0xdcea: case (A32 | 0xdcea): case (O32 | 0xdcea): case (A32 | O32 | 0xdcea):
	case 0xdceb: case (A32 | 0xdceb): case (O32 | 0xdceb): case (A32 | O32 | 0xdceb):
	case 0xdcec: case (A32 | 0xdcec): case (O32 | 0xdcec): case (A32 | O32 | 0xdcec):
	case 0xdced: case (A32 | 0xdced): case (O32 | 0xdced): case (A32 | O32 | 0xdced):
	case 0xdcee: case (A32 | 0xdcee): case (O32 | 0xdcee): case (A32 | O32 | 0xdcee):
	case 0xdcef: case (A32 | 0xdcef): case (O32 | 0xdcef): case (A32 | O32 | 0xdcef):
	case 0xdcf0: case (A32 | 0xdcf0): case (O32 | 0xdcf0): case (A32 | O32 | 0xdcf0):
	case 0xdcf1: case (A32 | 0xdcf1): case (O32 | 0xdcf1): case (A32 | O32 | 0xdcf1):
	case 0xdcf2: case (A32 | 0xdcf2): case (O32 | 0xdcf2): case (A32 | O32 | 0xdcf2):
	case 0xdcf3: case (A32 | 0xdcf3): case (O32 | 0xdcf3): case (A32 | O32 | 0xdcf3):
	case 0xdcf4: case (A32 | 0xdcf4): case (O32 | 0xdcf4): case (A32 | O32 | 0xdcf4):
	case 0xdcf5: case (A32 | 0xdcf5): case (O32 | 0xdcf5): case (A32 | O32 | 0xdcf5):
	case 0xdcf6: case (A32 | 0xdcf6): case (O32 | 0xdcf6): case (A32 | O32 | 0xdcf6):
	case 0xdcf7: case (A32 | 0xdcf7): case (O32 | 0xdcf7): case (A32 | O32 | 0xdcf7):
	case 0xdcf8: case (A32 | 0xdcf8): case (O32 | 0xdcf8): case (A32 | O32 | 0xdcf8):
	case 0xdcf9: case (A32 | 0xdcf9): case (O32 | 0xdcf9): case (A32 | O32 | 0xdcf9):
	case 0xdcfa: case (A32 | 0xdcfa): case (O32 | 0xdcfa): case (A32 | O32 | 0xdcfa):
	case 0xdcfb: case (A32 | 0xdcfb): case (O32 | 0xdcfb): case (A32 | O32 | 0xdcfb):
	case 0xdcfc: case (A32 | 0xdcfc): case (O32 | 0xdcfc): case (A32 | O32 | 0xdcfc):
	case 0xdcfd: case (A32 | 0xdcfd): case (O32 | 0xdcfd): case (A32 | O32 | 0xdcfd):
	case 0xdcfe: case (A32 | 0xdcfe): case (O32 | 0xdcfe): case (A32 | O32 | 0xdcfe):
	case 0xdcff: case (A32 | 0xdcff): case (O32 | 0xdcff): case (A32 | O32 | 0xdcff):

	case 0xddc0: case (A32 | 0xddc0): case (O32 | 0xddc0): case (A32 | O32 | 0xddc0):
	case 0xddc1: case (A32 | 0xddc1): case (O32 | 0xddc1): case (A32 | O32 | 0xddc1):
	case 0xddc2: case (A32 | 0xddc2): case (O32 | 0xddc2): case (A32 | O32 | 0xddc2):
	case 0xddc3: case (A32 | 0xddc3): case (O32 | 0xddc3): case (A32 | O32 | 0xddc3):
	case 0xddc4: case (A32 | 0xddc4): case (O32 | 0xddc4): case (A32 | O32 | 0xddc4):
	case 0xddc5: case (A32 | 0xddc5): case (O32 | 0xddc5): case (A32 | O32 | 0xddc5):
	case 0xddc6: case (A32 | 0xddc6): case (O32 | 0xddc6): case (A32 | O32 | 0xddc6):
	case 0xddc7: case (A32 | 0xddc7): case (O32 | 0xddc7): case (A32 | O32 | 0xddc7):
	case 0xddd0: case (A32 | 0xddd0): case (O32 | 0xddd0): case (A32 | O32 | 0xddd0):
	case 0xddd1: case (A32 | 0xddd1): case (O32 | 0xddd1): case (A32 | O32 | 0xddd1):
	case 0xddd2: case (A32 | 0xddd2): case (O32 | 0xddd2): case (A32 | O32 | 0xddd2):
	case 0xddd3: case (A32 | 0xddd3): case (O32 | 0xddd3): case (A32 | O32 | 0xddd3):
	case 0xddd4: case (A32 | 0xddd4): case (O32 | 0xddd4): case (A32 | O32 | 0xddd4):
	case 0xddd5: case (A32 | 0xddd5): case (O32 | 0xddd5): case (A32 | O32 | 0xddd5):
	case 0xddd6: case (A32 | 0xddd6): case (O32 | 0xddd6): case (A32 | O32 | 0xddd6):
	case 0xddd7: case (A32 | 0xddd7): case (O32 | 0xddd7): case (A32 | O32 | 0xddd7):
	case 0xddd8: case (A32 | 0xddd8): case (O32 | 0xddd8): case (A32 | O32 | 0xddd8):
	case 0xddd9: case (A32 | 0xddd9): case (O32 | 0xddd9): case (A32 | O32 | 0xddd9):
	case 0xddda: case (A32 | 0xddda): case (O32 | 0xddda): case (A32 | O32 | 0xddda):
	case 0xdddb: case (A32 | 0xdddb): case (O32 | 0xdddb): case (A32 | O32 | 0xdddb):
	case 0xdddc: case (A32 | 0xdddc): case (O32 | 0xdddc): case (A32 | O32 | 0xdddc):
	case 0xdddd: case (A32 | 0xdddd): case (O32 | 0xdddd): case (A32 | O32 | 0xdddd):
	case 0xddde: case (A32 | 0xddde): case (O32 | 0xddde): case (A32 | O32 | 0xddde):
	case 0xdddf: case (A32 | 0xdddf): case (O32 | 0xdddf): case (A32 | O32 | 0xdddf):
	case 0xdde0: case (A32 | 0xdde0): case (O32 | 0xdde0): case (A32 | O32 | 0xdde0):
	case 0xdde1: case (A32 | 0xdde1): case (O32 | 0xdde1): case (A32 | O32 | 0xdde1):
	case 0xdde2: case (A32 | 0xdde2): case (O32 | 0xdde2): case (A32 | O32 | 0xdde2):
	case 0xdde3: case (A32 | 0xdde3): case (O32 | 0xdde3): case (A32 | O32 | 0xdde3):
	case 0xdde4: case (A32 | 0xdde4): case (O32 | 0xdde4): case (A32 | O32 | 0xdde4):
	case 0xdde5: case (A32 | 0xdde5): case (O32 | 0xdde5): case (A32 | O32 | 0xdde5):
	case 0xdde6: case (A32 | 0xdde6): case (O32 | 0xdde6): case (A32 | O32 | 0xdde6):
	case 0xdde7: case (A32 | 0xdde7): case (O32 | 0xdde7): case (A32 | O32 | 0xdde7):
	case 0xdde8: case (A32 | 0xdde8): case (O32 | 0xdde8): case (A32 | O32 | 0xdde8):
	case 0xdde9: case (A32 | 0xdde9): case (O32 | 0xdde9): case (A32 | O32 | 0xdde9):
	case 0xddea: case (A32 | 0xddea): case (O32 | 0xddea): case (A32 | O32 | 0xddea):
	case 0xddeb: case (A32 | 0xddeb): case (O32 | 0xddeb): case (A32 | O32 | 0xddeb):
	case 0xddec: case (A32 | 0xddec): case (O32 | 0xddec): case (A32 | O32 | 0xddec):
	case 0xdded: case (A32 | 0xdded): case (O32 | 0xdded): case (A32 | O32 | 0xdded):
	case 0xddee: case (A32 | 0xddee): case (O32 | 0xddee): case (A32 | O32 | 0xddee):
	case 0xddef: case (A32 | 0xddef): case (O32 | 0xddef): case (A32 | O32 | 0xddef):

	case 0xdec0: case (A32 | 0xdec0): case (O32 | 0xdec0): case (A32 | O32 | 0xdec0):
	case 0xdec1: case (A32 | 0xdec1): case (O32 | 0xdec1): case (A32 | O32 | 0xdec1):
	case 0xdec2: case (A32 | 0xdec2): case (O32 | 0xdec2): case (A32 | O32 | 0xdec2):
	case 0xdec3: case (A32 | 0xdec3): case (O32 | 0xdec3): case (A32 | O32 | 0xdec3):
	case 0xdec4: case (A32 | 0xdec4): case (O32 | 0xdec4): case (A32 | O32 | 0xdec4):
	case 0xdec5: case (A32 | 0xdec5): case (O32 | 0xdec5): case (A32 | O32 | 0xdec5):
	case 0xdec6: case (A32 | 0xdec6): case (O32 | 0xdec6): case (A32 | O32 | 0xdec6):
	case 0xdec7: case (A32 | 0xdec7): case (O32 | 0xdec7): case (A32 | O32 | 0xdec7):
	case 0xdec8: case (A32 | 0xdec8): case (O32 | 0xdec8): case (A32 | O32 | 0xdec8):
	case 0xdec9: case (A32 | 0xdec9): case (O32 | 0xdec9): case (A32 | O32 | 0xdec9):
	case 0xdeca: case (A32 | 0xdeca): case (O32 | 0xdeca): case (A32 | O32 | 0xdeca):
	case 0xdecb: case (A32 | 0xdecb): case (O32 | 0xdecb): case (A32 | O32 | 0xdecb):
	case 0xdecc: case (A32 | 0xdecc): case (O32 | 0xdecc): case (A32 | O32 | 0xdecc):
	case 0xdecd: case (A32 | 0xdecd): case (O32 | 0xdecd): case (A32 | O32 | 0xdecd):
	case 0xdece: case (A32 | 0xdece): case (O32 | 0xdece): case (A32 | O32 | 0xdece):
	case 0xdecf: case (A32 | 0xdecf): case (O32 | 0xdecf): case (A32 | O32 | 0xdecf):
	case 0xded9: case (A32 | 0xded9): case (O32 | 0xded9): case (A32 | O32 | 0xded9):
	case 0xdee0: case (A32 | 0xdee0): case (O32 | 0xdee0): case (A32 | O32 | 0xdee0):
	case 0xdee1: case (A32 | 0xdee1): case (O32 | 0xdee1): case (A32 | O32 | 0xdee1):
	case 0xdee2: case (A32 | 0xdee2): case (O32 | 0xdee2): case (A32 | O32 | 0xdee2):
	case 0xdee3: case (A32 | 0xdee3): case (O32 | 0xdee3): case (A32 | O32 | 0xdee3):
	case 0xdee4: case (A32 | 0xdee4): case (O32 | 0xdee4): case (A32 | O32 | 0xdee4):
	case 0xdee5: case (A32 | 0xdee5): case (O32 | 0xdee5): case (A32 | O32 | 0xdee5):
	case 0xdee6: case (A32 | 0xdee6): case (O32 | 0xdee6): case (A32 | O32 | 0xdee6):
	case 0xdee7: case (A32 | 0xdee7): case (O32 | 0xdee7): case (A32 | O32 | 0xdee7):
	case 0xdee8: case (A32 | 0xdee8): case (O32 | 0xdee8): case (A32 | O32 | 0xdee8):
	case 0xdee9: case (A32 | 0xdee9): case (O32 | 0xdee9): case (A32 | O32 | 0xdee9):
	case 0xdeea: case (A32 | 0xdeea): case (O32 | 0xdeea): case (A32 | O32 | 0xdeea):
	case 0xdeeb: case (A32 | 0xdeeb): case (O32 | 0xdeeb): case (A32 | O32 | 0xdeeb):
	case 0xdeec: case (A32 | 0xdeec): case (O32 | 0xdeec): case (A32 | O32 | 0xdeec):
	case 0xdeed: case (A32 | 0xdeed): case (O32 | 0xdeed): case (A32 | O32 | 0xdeed):
	case 0xdeee: case (A32 | 0xdeee): case (O32 | 0xdeee): case (A32 | O32 | 0xdeee):
	case 0xdeef: case (A32 | 0xdeef): case (O32 | 0xdeef): case (A32 | O32 | 0xdeef):
	case 0xdef0: case (A32 | 0xdef0): case (O32 | 0xdef0): case (A32 | O32 | 0xdef0):
	case 0xdef1: case (A32 | 0xdef1): case (O32 | 0xdef1): case (A32 | O32 | 0xdef1):
	case 0xdef2: case (A32 | 0xdef2): case (O32 | 0xdef2): case (A32 | O32 | 0xdef2):
	case 0xdef3: case (A32 | 0xdef3): case (O32 | 0xdef3): case (A32 | O32 | 0xdef3):
	case 0xdef4: case (A32 | 0xdef4): case (O32 | 0xdef4): case (A32 | O32 | 0xdef4):
	case 0xdef5: case (A32 | 0xdef5): case (O32 | 0xdef5): case (A32 | O32 | 0xdef5):
	case 0xdef6: case (A32 | 0xdef6): case (O32 | 0xdef6): case (A32 | O32 | 0xdef6):
	case 0xdef7: case (A32 | 0xdef7): case (O32 | 0xdef7): case (A32 | O32 | 0xdef7):
	case 0xdef8: case (A32 | 0xdef8): case (O32 | 0xdef8): case (A32 | O32 | 0xdef8):
	case 0xdef9: case (A32 | 0xdef9): case (O32 | 0xdef9): case (A32 | O32 | 0xdef9):
	case 0xdefa: case (A32 | 0xdefa): case (O32 | 0xdefa): case (A32 | O32 | 0xdefa):
	case 0xdefb: case (A32 | 0xdefb): case (O32 | 0xdefb): case (A32 | O32 | 0xdefb):
	case 0xdefc: case (A32 | 0xdefc): case (O32 | 0xdefc): case (A32 | O32 | 0xdefc):
	case 0xdefd: case (A32 | 0xdefd): case (O32 | 0xdefd): case (A32 | O32 | 0xdefd):
	case 0xdefe: case (A32 | 0xdefe): case (O32 | 0xdefe): case (A32 | O32 | 0xdefe):
	case 0xdeff: case (A32 | 0xdeff): case (O32 | 0xdeff): case (A32 | O32 | 0xdeff):
		{
			//Move r_eip over the ModR/M byte
			++r_eip;

			AddOp(TYPE_REGISTER_FPU, opcode & 0x7ff));

			break;
		}

	//FSTSW AX
	case 0xdfe0: case (A32 | 0xdfe0): case (O32 | 0xdfe0): case (A32 | O32 | 0xdfe0):
		{
			++r_eip;

			AddOp(TYPE_GET, HOST_32, REG_EAX, GUEST, REG_EAX));		//Get EAX, gEAX
			AddOp(TYPE_REGISTER_FPU, opcode & 0x7ff));				//FSTSW
			AddOp(TYPE_PUT, GUEST, REG_EAX, HOST_32, REG_EAX));		//Put gEAX, EAX

			break;
		}

	default:
		{
#ifdef INVALID_OPCODES_FATAL
			std::stringstream ss;
			ss << "CPU Error: Unimplemented Instruction, opcode = 0x" << std::hex << opcode;
			throw Ex(ss.str());
#else
			readCodeException = EXCEPTION_INVALID_OPCODE;
			return false;
#endif
		}
	}

	return true;
}
