//	VMachine
//	Interpret an instruction
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

namespace
{
	const Dword A32 = OPCODE_A32;
	const Dword O32 = OPCODE_O32;
}

void VMachine::InterpretInstruction(Dword opcode, PrefixBitset prefixes)
{
	switch(opcode)
	{
	case 0x17:	case (A32 | 0x17):
		POP_SS_O16(prefixes);	break;

	case (O32 | 0x17):	case (A32 | O32 | 0x17):
		POP_SS_O32(prefixes);	break;


	case 0x6c:	case (O32 | 0x6c):
		INS_Yb_DX_A16(prefixes);	break;

	case (A32 | 0x6c):	case (A32 | O32 | 0x6c):
		INS_Yb_DX_A32(prefixes);	break;

	case 0x6d:
		INS_Yw_DX_A16(prefixes);	break;

	case (A32 | 0x6d):
		INS_Yw_DX_A32(prefixes);	break;

	case (O32 | 0x6d):
		INS_Yd_DX_A16(prefixes);	break;

	case (A32 | O32 | 0x6d):
		INS_Yd_DX_A32(prefixes);	break;


	case 0x6e:	case (O32 | 0x6e):
		OUTS_DX_Xb_A16(prefixes);	break;

	case (A32 | 0x6e):	case (A32 | O32 | 0x6e):
		OUTS_DX_Xb_A32(prefixes);	break;

	case 0x6f:
		OUTS_DX_Xw_A16(prefixes);	break;

	case (A32 | 0x6f):
		OUTS_DX_Xw_A32(prefixes);	break;

	case (O32 | 0x6f):
		OUTS_DX_Xd_A16(prefixes);	break;

	case (A32 | O32 | 0x6f):
		OUTS_DX_Xd_A32(prefixes);	break;


	//MOV SS, Mw
	case 0x8e10:	case (A32 | 0x8e10):	case (O32 | 0x8e10):	case (A32 | O32 | 0x8e10):
		MOV_Sw_Mw(opcode, prefixes);	break;

	//MOV SS, Rw
	case 0x8ed0:	case (A32 | 0x8ed0):	case (O32 | 0x8ed0):	case (A32 | O32 | 0x8ed0):
		MOV_Sw_Rw(prefixes);	break;

	case 0x9a:	case (A32 | 0x9a):	case (O32 | 0x9a):	case (A32 | O32 | 0x9a):
		CALL_FAR_Ap(opcode, prefixes);	break;

	case 0x9d:	case (A32 | 0x9d):
		POPF_O16(prefixes);		break;

	case (O32 | 0x9d):	case (A32 | O32 | 0x9d):
		POPF_O32(prefixes);		break;
	
	case 0xca:	case (A32 | 0xca):	case (O32 | 0xca):	case (A32 | O32 | 0xca):
		RETF_Iw(opcode, prefixes);		break;
	
	case 0xcb:	case (A32 | 0xcb):	case (O32 | 0xcb):	case (A32 | O32 | 0xcb):
		RETF(opcode, prefixes);			break;

	case 0xcd:	case (A32 | 0xcd):	case (O32 | 0xcd):	case (A32 | O32 | 0xcd):
		INT_Ib(prefixes);		break;

	case 0xcf:	case (A32 | 0xcf):	case (O32 | 0xcf):	case (A32 | O32 | 0xcf):
		IRET(opcode, prefixes);			break;
	
	case 0xea:	case (A32 | 0xea):	case (O32 | 0xea):	case (A32 | O32 | 0xea):
		JMP_FAR_Ap(opcode, prefixes);	break;


	case 0xe4:	case (A32 | 0xe4):	case (O32 | 0xe4):	case (A32 | O32 | 0xe4):
		IN_AL_Ib(prefixes);		break;

	case 0xe5:	case (A32 | 0xe5):
		IN_AX_Ib(prefixes);		break;

	case (O32 | 0xe5):	case (A32 | O32 | 0xe5):
		IN_EAX_Ib(prefixes);	break;

	case 0xe6:	case (A32 | 0xe6):	case (O32 | 0xe6):	case (A32 | O32 | 0xe6):
		OUT_Ib_AL(prefixes);	break;

	case 0xe7:	case (A32 | 0xe7):
		OUT_Ib_AX(prefixes);	break;

	case (O32 | 0xe7):	case (A32 | O32 | 0xe7):
		OUT_Ib_EAX(prefixes);	break;


	case 0xec:	case (A32 | 0xec):	case (O32 | 0xec):	case (A32 | O32 | 0xec):
		IN_AL_DX(prefixes);		break;

	case 0xed:	case (A32 | 0xed):
		IN_AX_DX(prefixes);		break;

	case (O32 | 0xed):	case (A32 | O32 | 0xed):
		IN_EAX_DX(prefixes);	break;

	case 0xee:	case (A32 | 0xee):	case (O32 | 0xee):	case (A32 | O32 | 0xee):
		OUT_DX_AL(prefixes);	break;

	case 0xef:	case (A32 | 0xef):
		OUT_DX_AX(prefixes);	break;

	case (O32 | 0xef):	case (A32 | O32 | 0xef):
		OUT_DX_EAX(prefixes);	break;


	case 0xf4:	case (A32 | 0xf4):	case (O32 | 0xf4):	case (A32 | O32 | 0xf4):
		//TODO: HLT
		break;


	case 0xfa:	case (A32 | 0xfa):	case (O32 | 0xfa):	case (A32 | O32 | 0xfa):
		CLI(prefixes);	break;
		break;

	case 0xfb:	case (A32 | 0xfb):	case (O32 | 0xfb):	case (A32 | O32 | 0xfb):
		STI(prefixes);	break;
		break;


	case 0xff10:	case (A32 | 0xff10):
		CALL_Mw(opcode, prefixes);		break;

	case (O32 | 0xff10):	case (A32 | O32 | 0xff10):
		CALL_Md(opcode, prefixes);		break;

	case 0xff18:	case (A32 | 0xff18):	case (O32 | 0xff18):	case (A32 | O32 | 0xff18):
		CALL_FAR_Mp(opcode, prefixes);	break;

	case 0xff20:	case (A32 | 0xff20):
		JMP_Mw(opcode, prefixes);		break;

	case (O32 | 0xff20):	case (A32 | O32 | 0xff20):
		JMP_Md(opcode, prefixes);		break;

	case 0xff28:	case (A32 | 0xff28):	case (O32 | 0xff28):	case (A32 | O32 | 0xff28):
		JMP_FAR_Mp(opcode, prefixes);	break;

	case 0xffd0:	case (A32 | 0xffd0):
		CALL_Rw(prefixes);		break;

	case (O32 | 0xffd0):	case (A32 | O32 | 0xffd0):
		CALL_Rd(prefixes);		break;

	case 0xffe0:	case (A32 | 0xffe0):
		JMP_Rw(prefixes);		break;

	case (O32 | 0xffe0):	case (A32 | O32 | 0xffe0):
		JMP_Rd(prefixes);		break;


	case 0x0f0000: case (A32 | 0x0f0000): case (O32 | 0x0f0000): case (A32 | O32 | 0x0f0000):
		SLDT_Mw(opcode, prefixes);		break;

	case 0x0f00c0: case (A32 | 0x0f00c0): case (O32 | 0x0f00c0): case (A32 | O32 | 0x0f00c0):
		SLDT_Rw(prefixes);		break;

	case 0x0f0008: case (A32 | 0x0f0008): case (O32 | 0x0f0008): case (A32 | O32 | 0x0f0008):
		STR_Mw(opcode, prefixes);		break;

	case 0x0f00c8: case (A32 | 0x0f00c8): case (O32 | 0x0f00c8): case (A32 | O32 | 0x0f00c8):
		STR_Rw(prefixes);		break;
	
	case 0x0f0010: case (A32 | 0x0f0010): case (O32 | 0x0f0010): case (A32 | O32 | 0x0f0010):
		LLDT_Mw(opcode, prefixes);		break;

	case 0x0f00d0: case (A32 | 0x0f00d0): case (O32 | 0x0f00d0): case (A32 | O32 | 0x0f00d0):
		LLDT_Rw(prefixes);		break;

	case 0x0f0018: case (A32 | 0x0f0018): case (O32 | 0x0f0018): case (A32 | O32 | 0x0f0018):
		LTR_Mw(opcode, prefixes);		break;

	case 0x0f00d8: case (A32 | 0x0f00d8): case (O32 | 0x0f00d8): case (A32 | O32 | 0x0f00d8):
		LTR_Rw(prefixes);		break;

	case 0x0f0020: case (A32 | 0x0f0020): case (O32 | 0x0f0020): case (A32 | O32 | 0x0f0020):
		VERR_Mw(opcode, prefixes);		break;

	case 0x0f00e0: case (A32 | 0x0f00e0): case (O32 | 0x0f00e0): case (A32 | O32 | 0x0f00e0):
		VERR_Rw(prefixes);		break;

	case 0x0f0028: case (A32 | 0x0f0028): case (O32 | 0x0f0028): case (A32 | O32 | 0x0f0028):
		VERW_Mw(opcode, prefixes);		break;

	case 0x0f00e8: case (A32 | 0x0f00e8): case (O32 | 0x0f00e8): case (A32 | O32 | 0x0f00e8):
		VERW_Rw(prefixes);		break;


	case 0x0f0100:	case (A32 | 0x0f0100):
		SGDT_Ms_O16(opcode, prefixes);	break;

	case (O32 | 0x0f0100):	case (A32 | O32 | 0x0f0100):
		SGDT_Ms_O32(opcode, prefixes);	break;

	case 0x0f0108:	case (A32 | 0x0f0108):
		SIDT_Ms_O16(opcode, prefixes);	break;

	case (O32 | 0x0f0108):	case (A32 | O32 | 0x0f0108):
		SIDT_Ms_O32(opcode, prefixes);	break;
	
	case 0x0f0110:	case (A32 | 0x0f0110):
		LGDT_Ms_O16(opcode, prefixes);	break;

	case (O32 | 0x0f0110):	case (A32 | O32 | 0x0f0110):
		LGDT_Ms_O32(opcode, prefixes);	break;

	case 0x0f0118:	case (A32 | 0x0f0118):
		LIDT_Ms_O16(opcode, prefixes);	break;

	case (O32 | 0x0f0118):	case (A32 | O32 | 0x0f0118):
		LIDT_Ms_O32(opcode, prefixes);	break;

	case 0x0f0120: case (A32 | 0x0f0120): case (O32 | 0x0f0120): case (A32 | O32 | 0x0f0120):
		SMSW_Mw(opcode, prefixes);		break;

	case 0x0f01e0: case (A32 | 0x0f01e0): case (O32 | 0x0f01e0): case (A32 | O32 | 0x0f01e0):
		SMSW_Rw(prefixes);		break;

	case 0x0f0130: case (A32 | 0x0f0130): case (O32 | 0x0f0130): case (A32 | O32 | 0x0f0130):
		LMSW_Mw(opcode, prefixes);		break;

	case 0x0f01f0: case (A32 | 0x0f01f0): case (O32 | 0x0f01f0): case (A32 | O32 | 0x0f01f0):
		LMSW_Rw(prefixes);		break;

	case 0x0f0138: case (A32 | 0x0f0138): case (O32 | 0x0f0138): case (A32 | O32 | 0x0f0138):
		INVLPG_M(opcode, prefixes);		break;


	case 0x0f0200: case (A32 | 0x0f0200):
		LAR_Gw_Mw(opcode, prefixes);	break;

	case (O32 | 0x0f0200): case (A32 | O32 | 0x0f0200):
		LAR_Gd_Mw(opcode, prefixes);	break;

	case 0x0f02c0: case (A32 | 0x0f02c0):
		LAR_Gw_Rw(prefixes);	break;

	case (O32 | 0x0f02c0): case (A32 | O32 | 0x0f02c0):
		LAR_Gd_Rw(prefixes);	break;


	case 0x0f0300: case (A32 | 0x0f0300):
		LSL_Gw_Mw(opcode, prefixes);	break;

	case (O32 | 0x0f0300): case (A32 | O32 | 0x0f0300):
		LSL_Gd_Mw(opcode, prefixes);	break;

	case 0x0f03c0: case (A32 | 0x0f03c0):
		LSL_Gw_Rw(prefixes);	break;

	case (O32 | 0x0f03c0): case (A32 | O32 | 0x0f03c0):
		LSL_Gd_Rw(prefixes);	break;


	case 0x0f22c0: case (A32 | 0x0f22c0): case (O32 | 0x0f22c0): case (A32 | O32 | 0x0f22c0):
		MOV_Cd_Rd(prefixes);	break;

	case 0x0f23c0: case (A32 | 0x0f23c0): case (O32 | 0x0f23c0): case (A32 | O32 | 0x0f23c0):
		MOV_Dd_Rd(prefixes);	break;


	case 0x0fb200: case (A32 | 0x0fb200):
		LSS_Gw_Mp(opcode, prefixes);	break;

	case (O32 | 0x0fb200): case (A32 | O32 | 0x0fb200):
		LSS_Gd_Mp(opcode, prefixes);	break;


	case 0x0f06: case (A32 | 0x0f06): case (O32 | 0x0f06): case (A32 | O32 | 0x0f06): //CLTS
		registers->r_cr0 &= ~CR0_TS_FLAG;
		break;


	//Windows 95 legitimately uses this invalid opcode, so do not throw an exception
	//even if INVALID_OPCODES_FATAL is #defined
	case 0x0fff: case (A32 | 0x0fff): case (O32 | 0x0fff): case (A32 | O32 | 0x0fff):
		registers->exception = EXCEPTION_INVALID_OPCODE;
		break;

	default:
		{
#ifdef INVALID_OPCODES_FATAL
			std::stringstream ss;
			ss << "CPU Error: Unimplemented Instruction, opcode = 0x" << std::hex << opcode;
			throw Ex(ss.str());
#else
			registers->exception = EXCEPTION_INVALID_OPCODE;
#endif
		}
	}
}
