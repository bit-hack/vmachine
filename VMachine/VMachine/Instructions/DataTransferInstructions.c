//	VMachine
//	Implementations of data transfer instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define PUSH_Regw(reg)			\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		WriteWord(registers->r_ss.base + registers->r_esp - 2, registers->r_##reg);	\
		registers->r_esp -= 2;	\
	}							\
	else						\
	{							\
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), registers->r_##reg);	\
		registers->r_sp -= 2;	\
	}


#define PUSH_Regd(reg)			\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		WriteDword(registers->r_ss.base + registers->r_esp - 4, registers->r_##reg);	\
		registers->r_esp -= 4;	\
	}							\
	else						\
	{							\
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), registers->r_##reg);	\
		registers->r_sp -= 4;	\
	}

#define POP_Regw(reg)			\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		registers->r_##reg = ReadWord(registers->r_ss.base + registers->r_esp);	\
		registers->r_esp += 2;	\
	}							\
	else						\
	{							\
		registers->r_##reg = ReadWord(registers->r_ss.base + registers->r_sp);	\
		registers->r_sp += 2;	\
	}

#define POP_Regd(reg)			\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		registers->r_##reg = ReadDword(registers->r_ss.base + registers->r_esp);	\
		registers->r_esp += 4;	\
	}							\
	else						\
	{							\
		registers->r_##reg = ReadDword(registers->r_ss.base + registers->r_sp);	\
		registers->r_sp += 4;	\
	}

#define PUSH_SegReg_O16(sreg)	\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		WriteWord(registers->r_ss.base + registers->r_esp - 2, registers->r_##sreg.selector);	\
		registers->r_esp -= 2;	\
	}							\
	else						\
	{							\
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), registers->r_##sreg.selector);	\
		registers->r_sp -= 2;	\
	}

#define PUSH_SegReg_O32(sreg)	\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
	{							\
		WriteDword(registers->r_ss.base + registers->r_esp - 4, registers->r_##sreg.selector);	\
		registers->r_esp -= 4;	\
	}							\
	else						\
	{							\
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), registers->r_##sreg.selector);	\
		registers->r_sp -= 4;	\
	}

#define POP_SegReg_O16(sreg)	\
	Word selector;				\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
		selector = ReadWord(registers->r_ss.base + registers->r_esp);	\
	else						\
		selector = ReadWord(registers->r_ss.base + registers->r_sp);	\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
		registers->r_esp += 2;	\
	else						\
		registers->r_sp += 2;

#define POP_SegReg_O32(sreg)	\
	Word selector;				\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
		selector = ReadWord(registers->r_ss.base + registers->r_esp);	\
	else						\
		selector = ReadWord(registers->r_ss.base + registers->r_sp);	\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)	\
		registers->r_esp += 4;	\
	else						\
		registers->r_sp += 4;

#define XCHG_AX_Regw(reg)		\
	Word temp = registers->r_ax;	\
	registers->r_ax = registers->r_##reg;	\
	registers->r_##reg = temp;

#define XCHG_EAX_Regd(reg)		\
	Dword temp = registers->r_eax;	\
	registers->r_eax = registers->r_##reg;	\
	registers->r_##reg = temp;

#define MOV_Regb_Ib(reg)		\
	registers->r_##reg = immByte;

#define MOV_Regw_Iw(reg)		\
	registers->r_##reg = immWord;

#define MOV_Regd_Id(reg)		\
	registers->r_##reg = immDword;

void OP_PUSH_AX(void) { PUSH_Regw(ax) asm (".global _OP_PUSH_AX_END\n _OP_PUSH_AX_END:"); }
void OP_PUSH_CX(void) { PUSH_Regw(cx) asm (".global _OP_PUSH_CX_END\n _OP_PUSH_CX_END:"); }
void OP_PUSH_DX(void) { PUSH_Regw(dx) asm (".global _OP_PUSH_DX_END\n _OP_PUSH_DX_END:"); }
void OP_PUSH_BX(void) { PUSH_Regw(bx) asm (".global _OP_PUSH_BX_END\n _OP_PUSH_BX_END:"); }
void OP_PUSH_SP(void) { PUSH_Regw(sp) asm (".global _OP_PUSH_SP_END\n _OP_PUSH_SP_END:"); }
void OP_PUSH_BP(void) { PUSH_Regw(bp) asm (".global _OP_PUSH_BP_END\n _OP_PUSH_BP_END:"); }
void OP_PUSH_SI(void) { PUSH_Regw(si) asm (".global _OP_PUSH_SI_END\n _OP_PUSH_SI_END:"); }
void OP_PUSH_DI(void) { PUSH_Regw(di) asm (".global _OP_PUSH_DI_END\n _OP_PUSH_DI_END:"); }

void OP_PUSH_EAX(void) { PUSH_Regd(eax) asm (".global _OP_PUSH_EAX_END\n _OP_PUSH_EAX_END:"); }
void OP_PUSH_ECX(void) { PUSH_Regd(ecx) asm (".global _OP_PUSH_ECX_END\n _OP_PUSH_ECX_END:"); }
void OP_PUSH_EDX(void) { PUSH_Regd(edx) asm (".global _OP_PUSH_EDX_END\n _OP_PUSH_EDX_END:"); }
void OP_PUSH_EBX(void) { PUSH_Regd(ebx) asm (".global _OP_PUSH_EBX_END\n _OP_PUSH_EBX_END:"); }
void OP_PUSH_ESP(void) { PUSH_Regd(esp) asm (".global _OP_PUSH_ESP_END\n _OP_PUSH_ESP_END:"); }
void OP_PUSH_EBP(void) { PUSH_Regd(ebp) asm (".global _OP_PUSH_EBP_END\n _OP_PUSH_EBP_END:"); }
void OP_PUSH_ESI(void) { PUSH_Regd(esi) asm (".global _OP_PUSH_ESI_END\n _OP_PUSH_ESI_END:"); }
void OP_PUSH_EDI(void) { PUSH_Regd(edi) asm (".global _OP_PUSH_EDI_END\n _OP_PUSH_EDI_END:"); }

void OP_POP_AX(void) { POP_Regw(ax) asm (".global _OP_POP_AX_END\n _OP_POP_AX_END:"); }
void OP_POP_CX(void) { POP_Regw(cx) asm (".global _OP_POP_CX_END\n _OP_POP_CX_END:"); }
void OP_POP_DX(void) { POP_Regw(dx) asm (".global _OP_POP_DX_END\n _OP_POP_DX_END:"); }
void OP_POP_BX(void) { POP_Regw(bx) asm (".global _OP_POP_BX_END\n _OP_POP_BX_END:"); }
void OP_POP_BP(void) { POP_Regw(bp) asm (".global _OP_POP_BP_END\n _OP_POP_BP_END:"); }
void OP_POP_SI(void) { POP_Regw(si) asm (".global _OP_POP_SI_END\n _OP_POP_SI_END:"); }
void OP_POP_DI(void) { POP_Regw(di) asm (".global _OP_POP_DI_END\n _OP_POP_DI_END:"); }

void OP_POP_SP(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Word r_sp;
		r_sp = ReadWord(registers->r_ss.base + registers->r_esp);
		
		registers->r_esp += 2;
		registers->r_sp = r_sp;
	}
	else
		registers->r_sp = ReadWord(registers->r_ss.base + registers->r_sp);
	
	asm (".global _OP_POP_SP_END\n _OP_POP_SP_END:");
}

void OP_POP_EAX(void) { POP_Regd(eax) asm (".global _OP_POP_EAX_END\n _OP_POP_EAX_END:"); }
void OP_POP_ECX(void) { POP_Regd(ecx) asm (".global _OP_POP_ECX_END\n _OP_POP_ECX_END:"); }
void OP_POP_EDX(void) { POP_Regd(edx) asm (".global _OP_POP_EDX_END\n _OP_POP_EDX_END:"); }
void OP_POP_EBX(void) { POP_Regd(ebx) asm (".global _OP_POP_EBX_END\n _OP_POP_EBX_END:"); }
void OP_POP_EBP(void) { POP_Regd(ebp) asm (".global _OP_POP_EBP_END\n _OP_POP_EBP_END:"); }
void OP_POP_ESI(void) { POP_Regd(esi) asm (".global _OP_POP_ESI_END\n _OP_POP_ESI_END:"); }
void OP_POP_EDI(void) { POP_Regd(edi) asm (".global _OP_POP_EDI_END\n _OP_POP_EDI_END:"); }

void OP_POP_ESP(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp = ReadDword(registers->r_ss.base + registers->r_esp);
	else
		registers->r_esp = ReadDword(registers->r_ss.base + registers->r_sp);

	asm (".global _OP_POP_ESP_END\n _OP_POP_ESP_END:");
}

void OP_PUSH_CS_O16(void) { PUSH_SegReg_O16(cs) asm (".global _OP_PUSH_CS_O16_END\n _OP_PUSH_CS_O16_END:"); }
void OP_PUSH_DS_O16(void) { PUSH_SegReg_O16(ds) asm (".global _OP_PUSH_DS_O16_END\n _OP_PUSH_DS_O16_END:"); }
void OP_PUSH_ES_O16(void) { PUSH_SegReg_O16(es) asm (".global _OP_PUSH_ES_O16_END\n _OP_PUSH_ES_O16_END:"); }
void OP_PUSH_FS_O16(void) { PUSH_SegReg_O16(fs) asm (".global _OP_PUSH_FS_O16_END\n _OP_PUSH_FS_O16_END:"); }
void OP_PUSH_GS_O16(void) { PUSH_SegReg_O16(gs) asm (".global _OP_PUSH_GS_O16_END\n _OP_PUSH_GS_O16_END:"); }
void OP_PUSH_SS_O16(void) { PUSH_SegReg_O16(ss) asm (".global _OP_PUSH_SS_O16_END\n _OP_PUSH_SS_O16_END:"); }

void OP_PUSH_CS_O32(void) { PUSH_SegReg_O32(cs) asm (".global _OP_PUSH_CS_O32_END\n _OP_PUSH_CS_O32_END:"); }
void OP_PUSH_DS_O32(void) { PUSH_SegReg_O32(ds) asm (".global _OP_PUSH_DS_O32_END\n _OP_PUSH_DS_O32_END:"); }
void OP_PUSH_ES_O32(void) { PUSH_SegReg_O32(es) asm (".global _OP_PUSH_ES_O32_END\n _OP_PUSH_ES_O32_END:"); }
void OP_PUSH_FS_O32(void) { PUSH_SegReg_O32(fs) asm (".global _OP_PUSH_FS_O32_END\n _OP_PUSH_FS_O32_END:"); }
void OP_PUSH_GS_O32(void) { PUSH_SegReg_O32(gs) asm (".global _OP_PUSH_GS_O32_END\n _OP_PUSH_GS_O32_END:"); }
void OP_PUSH_SS_O32(void) { PUSH_SegReg_O32(ss) asm (".global _OP_PUSH_SS_O32_END\n _OP_PUSH_SS_O32_END:"); }

void OP_POP_DS_O16(void) { POP_SegReg_O16(ds) asm (".global _OP_POP_DS_O16_END\n _OP_POP_DS_O16_END:"); }
void OP_POP_ES_O16(void) { POP_SegReg_O16(es) asm (".global _OP_POP_ES_O16_END\n _OP_POP_ES_O16_END:"); }
void OP_POP_FS_O16(void) { POP_SegReg_O16(fs) asm (".global _OP_POP_FS_O16_END\n _OP_POP_FS_O16_END:"); }
void OP_POP_GS_O16(void) { POP_SegReg_O16(gs) asm (".global _OP_POP_GS_O16_END\n _OP_POP_GS_O16_END:"); }

void OP_POP_DS_O32(void) { POP_SegReg_O32(ds) asm (".global _OP_POP_DS_O32_END\n _OP_POP_DS_O32_END:"); }
void OP_POP_ES_O32(void) { POP_SegReg_O32(es) asm (".global _OP_POP_ES_O32_END\n _OP_POP_ES_O32_END:"); }
void OP_POP_FS_O32(void) { POP_SegReg_O32(fs) asm (".global _OP_POP_FS_O32_END\n _OP_POP_FS_O32_END:"); }
void OP_POP_GS_O32(void) { POP_SegReg_O32(gs) asm (".global _OP_POP_GS_O32_END\n _OP_POP_GS_O32_END:"); }

void OP_PUSHF_O16(void)
{
	//If in V8086 mode with IOPL != 3, #GP(0)
	if(	(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0 &&
		(registers->r_systemFlags & EFLAGS_IOPL_FLAG) != EFLAGS_IOPL_FLAG)
	{
		registers->exception = EXCEPTION_GENERAL_PROTECTION;
		registers->errorCode = 0;
		ReturnImmediately();
	}

	Word data = (registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK) | registers->r_systemFlags;
	
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, data);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), data);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSHF_O16_END\n _OP_PUSHF_O16_END:");
}

void OP_PUSHF_O32(void)
{
	//If in V8086 mode with IOPL != 3, #GP(0)
	if(	(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0 &&
		(registers->r_systemFlags & EFLAGS_IOPL_FLAG) != EFLAGS_IOPL_FLAG)
	{
		registers->exception = EXCEPTION_GENERAL_PROTECTION;
		registers->errorCode = 0;
		ReturnImmediately();
	}

	Dword data = (registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK) | registers->r_systemFlags;
	data &= 0x00fcffff;
	
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, data);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), data);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSHF_O32_END\n _OP_PUSHF_O32_END:");
}

void OP_PUSH_Rw(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, Rw);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), Rw);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSH_Rw_END\n _OP_PUSH_Rw_END:");
}

void OP_PUSH_Rd(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, Rd);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), Rd);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSH_Rd_END\n _OP_PUSH_Rd_END:");
}

void OP_POP_Rw(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Rw = ReadWord(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 2;
	}
	else
	{
		Rw = ReadWord(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 2;
	}

	asm (".global _OP_POP_Rw_END\n _OP_POP_Rw_END:");
}

void OP_POP_Rd(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		Rd = ReadDword(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 4;
	}
	else
	{
		Rd = ReadDword(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 4;
	}

	asm (".global _OP_POP_Rd_END\n _OP_POP_Rd_END:");
}

void OP_PUSH_Mw_A16(void)
{
	Dword address = CalculateAddressA16();
	Word data = ReadWord(address);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, data);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), data);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSH_Mw_A16_END\n _OP_PUSH_Mw_A16_END:");
}

void OP_PUSH_Md_A16(void)
{
	Dword address = CalculateAddressA16();
	Dword data = ReadDword(address);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, data);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), data);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSH_Md_A16_END\n _OP_PUSH_Md_A16_END:");
}

void OP_PUSH_Mw_A32(void)
{
	Dword address = CalculateAddressA32();
	Word data = ReadWord(address);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, data);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), data);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSH_Mw_A32_END\n _OP_PUSH_Mw_A32_END:");
}

void OP_PUSH_Md_A32(void)
{
	Dword address = CalculateAddressA32();
	Dword data = ReadDword(address);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, data);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), data);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSH_Md_A32_END\n _OP_PUSH_Md_A32_END:");
}

void OP_POP_Mw_A16(void)
{
	Word data;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		data = ReadWord(registers->r_ss.base + registers->r_esp);
	else
		data = ReadWord(registers->r_ss.base + registers->r_sp);

	//The address calculation uses the value of esp _after_ the pop
	Dword tempEsp = registers->r_esp;

	//In the A16 case, no need to worry about the top word of esp
	registers->r_sp += 2;

	Dword address = CalculateAddressA16();

	registers->r_esp = tempEsp;

	WriteWord(address, data);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 2;
	else
		registers->r_sp += 2;

	asm (".global _OP_POP_Mw_A16_END\n _OP_POP_Mw_A16_END:");
}

void OP_POP_Md_A16(void)
{
	Dword data;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		data = ReadDword(registers->r_ss.base + registers->r_esp);
	else
		data = ReadDword(registers->r_ss.base + registers->r_sp);

	//The address calculation uses the value of esp _after_ the pop
	Dword tempEsp = registers->r_esp;

	//In the A16 case, no need to worry about the top word of esp
	registers->r_sp += 2;

	Dword address = CalculateAddressA16();

	registers->r_esp = tempEsp;

	WriteDword(address, data);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 4;
	else
		registers->r_sp += 4;

	asm (".global _OP_POP_Md_A16_END\n _OP_POP_Md_A16_END:");
}

void OP_POP_Mw_A32(void)
{
	Word data;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		data = ReadWord(registers->r_ss.base + registers->r_esp);
	else
		data = ReadWord(registers->r_ss.base + registers->r_sp);

	//The address calculation uses the value of esp _after_ the pop
	Dword tempEsp = registers->r_esp;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 2;
	else
		registers->r_sp += 2;

	Dword address = CalculateAddressA32();

	registers->r_esp = tempEsp;

	WriteWord(address, data);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 2;
	else
		registers->r_sp += 2;

	asm (".global _OP_POP_Mw_A32_END\n _OP_POP_Mw_A32_END:");
}

void OP_POP_Md_A32(void)
{
	Dword data;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		data = ReadDword(registers->r_ss.base + registers->r_esp);
	else
		data = ReadDword(registers->r_ss.base + registers->r_sp);

	//The address calculation uses the value of esp _after_ the pop
	Dword tempEsp = registers->r_esp;

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 4;
	else
		registers->r_sp += 4;

	Dword address = CalculateAddressA32();

	registers->r_esp = tempEsp;

	WriteDword(address, data);

	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
		registers->r_esp += 4;
	else
		registers->r_sp += 4;

	asm (".global _OP_POP_Md_A32_END\n _OP_POP_Md_A32_END:");
}

void OP_PUSH_Iw(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, immWord);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), immWord);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSH_Iw_END\n _OP_PUSH_Iw_END:");
}

void OP_PUSH_Id(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, immDword);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), immDword);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSH_Id_END\n _OP_PUSH_Id_END:");
}

void OP_PUSH_Ib_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, (SignedByte)immByte);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), (SignedByte)immByte);
		registers->r_sp -= 2;
	}

	asm (".global _OP_PUSH_Ib_O16_END\n _OP_PUSH_Ib_O16_END:");
}

void OP_PUSH_Ib_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, (SignedByte)immByte);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), (SignedByte)immByte);
		registers->r_sp -= 4;
	}

	asm (".global _OP_PUSH_Ib_O32_END\n _OP_PUSH_Ib_O32_END:");
}

void OP_PUSHA_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, registers->r_ax);
		WriteWord(registers->r_ss.base + registers->r_esp - 4, registers->r_cx);
		WriteWord(registers->r_ss.base + registers->r_esp - 6, registers->r_dx);
		WriteWord(registers->r_ss.base + registers->r_esp - 8, registers->r_bx);
		WriteWord(registers->r_ss.base + registers->r_esp - 10, registers->r_sp);
		WriteWord(registers->r_ss.base + registers->r_esp - 12, registers->r_bp);
		WriteWord(registers->r_ss.base + registers->r_esp - 14, registers->r_si);
		WriteWord(registers->r_ss.base + registers->r_esp - 16, registers->r_di);
		registers->r_esp -= 16;
	}
	else
	{
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 2), registers->r_ax);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 4), registers->r_cx);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 6), registers->r_dx);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 8), registers->r_bx);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 10), registers->r_sp);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 12), registers->r_bp);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 14), registers->r_si);
		WriteWord(registers->r_ss.base + ((Word)registers->r_sp - 16), registers->r_di);
		registers->r_sp -= 16;
	}

	asm (".global _OP_PUSHA_O16_END\n _OP_PUSHA_O16_END:");
}

void OP_PUSHA_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, registers->r_eax);
		WriteDword(registers->r_ss.base + registers->r_esp - 8, registers->r_ecx);
		WriteDword(registers->r_ss.base + registers->r_esp - 12, registers->r_edx);
		WriteDword(registers->r_ss.base + registers->r_esp - 16, registers->r_ebx);
		WriteDword(registers->r_ss.base + registers->r_esp - 20, registers->r_esp);
		WriteDword(registers->r_ss.base + registers->r_esp - 24, registers->r_ebp);
		WriteDword(registers->r_ss.base + registers->r_esp - 28, registers->r_esi);
		WriteDword(registers->r_ss.base + registers->r_esp - 32, registers->r_edi);
		registers->r_esp -= 32;
	}
	else
	{
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 4), registers->r_eax);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 8), registers->r_ecx);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 12), registers->r_edx);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 16), registers->r_ebx);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 20), registers->r_esp);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 24), registers->r_ebp);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 28), registers->r_esi);
		WriteDword(registers->r_ss.base + ((Word)registers->r_sp - 32), registers->r_edi);
		registers->r_sp -= 32;
	}

	asm (".global _OP_PUSHA_O32_END\n _OP_PUSHA_O32_END:");
}

void OP_POPA_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_di = ReadWord(registers->r_ss.base + registers->r_esp);
		registers->r_si = ReadWord(registers->r_ss.base + registers->r_esp + 2);
		registers->r_bp = ReadWord(registers->r_ss.base + registers->r_esp + 4);
		registers->r_bx = ReadWord(registers->r_ss.base + registers->r_esp + 8);
		registers->r_dx = ReadWord(registers->r_ss.base + registers->r_esp + 10);
		registers->r_cx = ReadWord(registers->r_ss.base + registers->r_esp + 12);
		registers->r_ax = ReadWord(registers->r_ss.base + registers->r_esp + 14);
		registers->r_esp += 16;
	}
	else
	{
		registers->r_di = ReadWord(registers->r_ss.base + ((Word)registers->r_sp));
		registers->r_si = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 2));
		registers->r_bp = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 4));
		registers->r_bx = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 8));
		registers->r_dx = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 10));
		registers->r_cx = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 12));
		registers->r_ax = ReadWord(registers->r_ss.base + ((Word)registers->r_sp + 14));
		registers->r_sp += 16;
	}

	asm (".global _OP_POPA_O16_END\n _OP_POPA_O16_END:");
}

void OP_POPA_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_edi = ReadDword(registers->r_ss.base + registers->r_esp);
		registers->r_esi = ReadDword(registers->r_ss.base + registers->r_esp + 4);
		registers->r_ebp = ReadDword(registers->r_ss.base + registers->r_esp + 8);
		registers->r_ebx = ReadDword(registers->r_ss.base + registers->r_esp + 16);
		registers->r_edx = ReadDword(registers->r_ss.base + registers->r_esp + 20);
		registers->r_ecx = ReadDword(registers->r_ss.base + registers->r_esp + 24);
		registers->r_eax = ReadDword(registers->r_ss.base + registers->r_esp + 28);
		registers->r_esp += 32;
	}
	else
	{
		registers->r_edi = ReadDword(registers->r_ss.base + ((Word)registers->r_sp));
		registers->r_esi = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 4));
		registers->r_ebp = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 8));
		registers->r_ebx = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 16));
		registers->r_edx = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 20));
		registers->r_ecx = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 24));
		registers->r_eax = ReadDword(registers->r_ss.base + ((Word)registers->r_sp + 28));
		registers->r_sp += 32;
	}

	asm (".global _OP_POPA_O32_END\n _OP_POPA_O32_END:");
}

void OP_XCHG_AX_CX(void) { XCHG_AX_Regw(cx) asm (".global _OP_XCHG_AX_CX_END\n _OP_XCHG_AX_CX_END:"); }
void OP_XCHG_AX_DX(void) { XCHG_AX_Regw(dx) asm (".global _OP_XCHG_AX_DX_END\n _OP_XCHG_AX_DX_END:"); }
void OP_XCHG_AX_BX(void) { XCHG_AX_Regw(bx) asm (".global _OP_XCHG_AX_BX_END\n _OP_XCHG_AX_BX_END:"); }
void OP_XCHG_AX_SP(void) { XCHG_AX_Regw(sp) asm (".global _OP_XCHG_AX_SP_END\n _OP_XCHG_AX_SP_END:"); }
void OP_XCHG_AX_BP(void) { XCHG_AX_Regw(bp) asm (".global _OP_XCHG_AX_BP_END\n _OP_XCHG_AX_BP_END:"); }
void OP_XCHG_AX_SI(void) { XCHG_AX_Regw(si) asm (".global _OP_XCHG_AX_SI_END\n _OP_XCHG_AX_SI_END:"); }
void OP_XCHG_AX_DI(void) { XCHG_AX_Regw(di) asm (".global _OP_XCHG_AX_DI_END\n _OP_XCHG_AX_DI_END:"); }

void OP_XCHG_EAX_ECX(void) { XCHG_EAX_Regd(ecx) asm (".global _OP_XCHG_EAX_ECX_END\n _OP_XCHG_EAX_ECX_END:"); }
void OP_XCHG_EAX_EDX(void) { XCHG_EAX_Regd(edx) asm (".global _OP_XCHG_EAX_EDX_END\n _OP_XCHG_EAX_EDX_END:"); }
void OP_XCHG_EAX_EBX(void) { XCHG_EAX_Regd(ebx) asm (".global _OP_XCHG_EAX_EBX_END\n _OP_XCHG_EAX_EBX_END:"); }
void OP_XCHG_EAX_ESP(void) { XCHG_EAX_Regd(esp) asm (".global _OP_XCHG_EAX_ESP_END\n _OP_XCHG_EAX_ESP_END:"); }
void OP_XCHG_EAX_EBP(void) { XCHG_EAX_Regd(ebp) asm (".global _OP_XCHG_EAX_EBP_END\n _OP_XCHG_EAX_EBP_END:"); }
void OP_XCHG_EAX_ESI(void) { XCHG_EAX_Regd(esi) asm (".global _OP_XCHG_EAX_ESI_END\n _OP_XCHG_EAX_ESI_END:"); }
void OP_XCHG_EAX_EDI(void) { XCHG_EAX_Regd(edi) asm (".global _OP_XCHG_EAX_EDI_END\n _OP_XCHG_EAX_EDI_END:"); }

void OP_XCHG_Rb_Gb(void)
{
	Byte temp = Rb;
	Rb = Gb;
	Gb = temp;

	asm (".global _OP_XCHG_Rb_Gb_END\n _OP_XCHG_Rb_Gb_END:");
}

void OP_XCHG_Mb_Gb_A16(void)
{
	Dword address = CalculateAddressA16();
	
	Byte temp = ReadByte(address);
	WriteByte(address, Gb);
	Gb = temp;

	asm (".global _OP_XCHG_Mb_Gb_A16_END\n _OP_XCHG_Mb_Gb_A16_END:");
}

void OP_XCHG_Mb_Gb_A32(void)
{
	Dword address = CalculateAddressA32();
	
	Byte temp = ReadByte(address);
	WriteByte(address, Gb);
	Gb = temp;

	asm (".global _OP_XCHG_Mb_Gb_A32_END\n _OP_XCHG_Mb_Gb_A32_END:");
}

void OP_XCHG_Rw_Gw(void)
{
	Word temp = Rw;
	Rw = Gw;
	Gw = temp;

	asm (".global _OP_XCHG_Rw_Gw_END\n _OP_XCHG_Rw_Gw_END:");
}

void OP_XCHG_Mw_Gw_A16(void)
{
	Dword address = CalculateAddressA16();
	
	Word temp = ReadWord(address);
	WriteWord(address, Gw);
	Gw = temp;

	asm (".global _OP_XCHG_Mw_Gw_A16_END\n _OP_XCHG_Mw_Gw_A16_END:");
}

void OP_XCHG_Mw_Gw_A32(void)
{
	Dword address = CalculateAddressA32();
	
	Word temp = ReadWord(address);
	WriteWord(address, Gw);
	Gw = temp;

	asm (".global _OP_XCHG_Mw_Gw_A32_END\n _OP_XCHG_Mw_Gw_A32_END:");
}

void OP_XCHG_Rd_Gd(void)
{
	Dword temp = Rd;
	Rd = Gd;
	Gd = temp;

	asm (".global _OP_XCHG_Rd_Gd_END\n _OP_XCHG_Rd_Gd_END:");
}

void OP_XCHG_Md_Gd_A16(void)
{
	Dword address = CalculateAddressA16();
	
	Dword temp = ReadDword(address);
	WriteDword(address, Gd);
	Gd = temp;

	asm (".global _OP_XCHG_Md_Gd_A16_END\n _OP_XCHG_Md_Gd_A16_END:");
}

void OP_XCHG_Md_Gd_A32(void)
{
	Dword address = CalculateAddressA32();
	
	Dword temp = ReadDword(address);
	WriteDword(address, Gd);
	Gd = temp;

	asm (".global _OP_XCHG_Md_Gd_A32_END\n _OP_XCHG_Md_Gd_A32_END:");
}

void OP_MOV_AL_Ib(void) { MOV_Regb_Ib(al) asm (".global _OP_MOV_AL_Ib_END\n _OP_MOV_AL_Ib_END:"); }
void OP_MOV_CL_Ib(void) { MOV_Regb_Ib(cl) asm (".global _OP_MOV_CL_Ib_END\n _OP_MOV_CL_Ib_END:"); }
void OP_MOV_DL_Ib(void) { MOV_Regb_Ib(dl) asm (".global _OP_MOV_DL_Ib_END\n _OP_MOV_DL_Ib_END:"); }
void OP_MOV_BL_Ib(void) { MOV_Regb_Ib(bl) asm (".global _OP_MOV_BL_Ib_END\n _OP_MOV_BL_Ib_END:");; }
void OP_MOV_AH_Ib(void) { MOV_Regb_Ib(ah) asm (".global _OP_MOV_AH_Ib_END\n _OP_MOV_AH_Ib_END:"); }
void OP_MOV_CH_Ib(void) { MOV_Regb_Ib(ch) asm (".global _OP_MOV_CH_Ib_END\n _OP_MOV_CH_Ib_END:"); }
void OP_MOV_DH_Ib(void) { MOV_Regb_Ib(dh) asm (".global _OP_MOV_DH_Ib_END\n _OP_MOV_DH_Ib_END:"); }
void OP_MOV_BH_Ib(void) { MOV_Regb_Ib(bh) asm (".global _OP_MOV_BH_Ib_END\n _OP_MOV_BH_Ib_END:"); }

void OP_MOV_AX_Iw(void) { MOV_Regw_Iw(ax) asm (".global _OP_MOV_AX_Iw_END\n _OP_MOV_AX_Iw_END:"); }
void OP_MOV_CX_Iw(void) { MOV_Regw_Iw(cx) asm (".global _OP_MOV_CX_Iw_END\n _OP_MOV_CX_Iw_END:"); }
void OP_MOV_DX_Iw(void) { MOV_Regw_Iw(dx) asm (".global _OP_MOV_DX_Iw_END\n _OP_MOV_DX_Iw_END:"); }
void OP_MOV_BX_Iw(void) { MOV_Regw_Iw(bx) asm (".global _OP_MOV_BX_Iw_END\n _OP_MOV_BX_Iw_END:"); }
void OP_MOV_SP_Iw(void) { MOV_Regw_Iw(sp) asm (".global _OP_MOV_SP_Iw_END\n _OP_MOV_SP_Iw_END:"); }
void OP_MOV_BP_Iw(void) { MOV_Regw_Iw(bp) asm (".global _OP_MOV_BP_Iw_END\n _OP_MOV_BP_Iw_END:"); }
void OP_MOV_SI_Iw(void) { MOV_Regw_Iw(si) asm (".global _OP_MOV_SI_Iw_END\n _OP_MOV_SI_Iw_END:"); }
void OP_MOV_DI_Iw(void) { MOV_Regw_Iw(di) asm (".global _OP_MOV_DI_Iw_END\n _OP_MOV_DI_Iw_END:"); }

void OP_MOV_EAX_Id(void) { MOV_Regd_Id(eax) asm (".global _OP_MOV_EAX_Id_END\n _OP_MOV_EAX_Id_END:"); }
void OP_MOV_ECX_Id(void) { MOV_Regd_Id(ecx) asm (".global _OP_MOV_ECX_Id_END\n _OP_MOV_ECX_Id_END:"); }
void OP_MOV_EDX_Id(void) { MOV_Regd_Id(edx) asm (".global _OP_MOV_EDX_Id_END\n _OP_MOV_EDX_Id_END:"); }
void OP_MOV_EBX_Id(void) { MOV_Regd_Id(ebx) asm (".global _OP_MOV_EBX_Id_END\n _OP_MOV_EBX_Id_END:"); }
void OP_MOV_ESP_Id(void) { MOV_Regd_Id(esp) asm (".global _OP_MOV_ESP_Id_END\n _OP_MOV_ESP_Id_END:"); }
void OP_MOV_EBP_Id(void) { MOV_Regd_Id(ebp) asm (".global _OP_MOV_EBP_Id_END\n _OP_MOV_EBP_Id_END:"); }
void OP_MOV_ESI_Id(void) { MOV_Regd_Id(esi) asm (".global _OP_MOV_ESI_Id_END\n _OP_MOV_ESI_Id_END:"); }
void OP_MOV_EDI_Id(void) { MOV_Regd_Id(edi) asm (".global _OP_MOV_EDI_Id_END\n _OP_MOV_EDI_Id_END:"); }

void OP_MOV_Sw_Rw(void)
{
	SetDataSegmentRegisterValuePtr(&Sw, Rw);

	asm (".global _OP_MOV_Sw_Rw_END\n _OP_MOV_Sw_Rw_END:");
}

void OP_MOV_Sw_Mw_A16(void)
{
	Dword address = CalculateAddressA16();
	Word selector = ReadWord(address);

	SetDataSegmentRegisterValuePtr(&Sw, selector);

	asm (".global _OP_MOV_Sw_Mw_A16_END\n _OP_MOV_Sw_Mw_A16_END:");
}

void OP_MOV_Sw_Mw_A32(void)
{
	Dword address = CalculateAddressA32();
	Word selector = ReadWord(address);

	SetDataSegmentRegisterValuePtr(&Sw, selector);

	asm (".global _OP_MOV_Sw_Mw_A32_END\n _OP_MOV_Sw_Mw_A32_END:");
}

void OP_MOV_Rw_Sw(void)
{
	Rw = Sw.selector;
	asm (".global _OP_MOV_Rw_Sw_END\n _OP_MOV_Rw_Sw_END:");
}

void OP_MOV_Mw_Sw_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteWord(address, Sw.selector);

	asm (".global _OP_MOV_Mw_Sw_A16_END\n _OP_MOV_Mw_Sw_A16_END:");
}

void OP_MOV_Mw_Sw_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteWord(address, Sw.selector);

	asm (".global _OP_MOV_Mw_Sw_A32_END\n _OP_MOV_Mw_Sw_A32_END:");
}

void OP_MOV_Rb_Gb(void)
{
	Rb = Gb;
	asm (".global _OP_MOV_Rb_Gb_END\n _OP_MOV_Rb_Gb_END:");
}

void OP_MOV_Mb_Gb_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteByte(address, Gb);
	asm (".global _OP_MOV_Mb_Gb_A16_END\n _OP_MOV_Mb_Gb_A16_END:");
}

void OP_MOV_Mb_Gb_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteByte(address, Gb);
	asm (".global _OP_MOV_Mb_Gb_A32_END\n _OP_MOV_Mb_Gb_A32_END:");
}

void OP_MOV_Rw_Gw(void)
{
	Rw = Gw;
	asm (".global _OP_MOV_Rw_Gw_END\n _OP_MOV_Rw_Gw_END:");
}

void OP_MOV_Mw_Gw_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteWord(address, Gw);
	asm (".global _OP_MOV_Mw_Gw_A16_END\n _OP_MOV_Mw_Gw_A16_END:");
}

void OP_MOV_Mw_Gw_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteWord(address, Gw);
	asm (".global _OP_MOV_Mw_Gw_A32_END\n _OP_MOV_Mw_Gw_A32_END:");
}

void OP_MOV_Rd_Gd(void)
{
	Rd = Gd;
	asm (".global _OP_MOV_Rd_Gd_END\n _OP_MOV_Rd_Gd_END:");
}

void OP_MOV_Md_Gd_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteDword(address, Gd);
	asm (".global _OP_MOV_Md_Gd_A16_END\n _OP_MOV_Md_Gd_A16_END:");
}

void OP_MOV_Md_Gd_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteDword(address, Gd);
	asm (".global _OP_MOV_Md_Gd_A32_END\n _OP_MOV_Md_Gd_A32_END:");
}

void OP_MOV_Gb_Rb(void)
{
	Gb = Rb;
	asm (".global _OP_MOV_Gb_Rb_END\n _OP_MOV_Gb_Rb_END:");
}

void OP_MOV_Gb_Mb_A16(void)
{
	Dword address = CalculateAddressA16();
	Gb = ReadByte(address);
	asm (".global _OP_MOV_Gb_Mb_A16_END\n _OP_MOV_Gb_Mb_A16_END:");
}

void OP_MOV_Gb_Mb_A32(void)
{
	Dword address = CalculateAddressA32();
	Gb = ReadByte(address);
	asm (".global _OP_MOV_Gb_Mb_A32_END\n _OP_MOV_Gb_Mb_A32_END:");
}

void OP_MOV_Gw_Rw(void)
{
	Gw = Rw;
	asm (".global _OP_MOV_Gw_Rw_END\n _OP_MOV_Gw_Rw_END:");
}

void OP_MOV_Gw_Mw_A16(void)
{
	Dword address = CalculateAddressA16();
	Gw = ReadWord(address);
	asm (".global _OP_MOV_Gw_Mw_A16_END\n _OP_MOV_Gw_Mw_A16_END:");
}

void OP_MOV_Gw_Mw_A32(void)
{
	Dword address = CalculateAddressA32();
	Gw = ReadWord(address);
	asm (".global _OP_MOV_Gw_Mw_A32_END\n _OP_MOV_Gw_Mw_A32_END:");
}

void OP_MOV_Gd_Rd(void)
{
	Gd = Rd;
	asm (".global _OP_MOV_Gd_Rd_END\n _OP_MOV_Gd_Rd_END:");
}

void OP_MOV_Gd_Md_A16(void)
{
	Dword address = CalculateAddressA16();
	Gd = ReadDword(address);
	asm (".global _OP_MOV_Gd_Md_A16_END\n _OP_MOV_Gd_Md_A16_END:");
}

void OP_MOV_Gd_Md_A32(void)
{
	Dword address = CalculateAddressA32();
	Gd = ReadDword(address);
	asm (".global _OP_MOV_Gd_Md_A32_END\n _OP_MOV_Gd_Md_A32_END:");
}

void OP_MOV_Rb_Ib(void)
{
	Rb = immByte;
	asm (".global _OP_MOV_Rb_Ib_END\n _OP_MOV_Rb_Ib_END:");
}

void OP_MOV_Mb_Ib_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteByte(address, immByte);
	asm (".global _OP_MOV_Mb_Ib_A16_END\n _OP_MOV_Mb_Ib_A16_END:");
}

void OP_MOV_Mb_Ib_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteByte(address, immByte);
	asm (".global _OP_MOV_Mb_Ib_A32_END\n _OP_MOV_Mb_Ib_A32_END:");
}


void OP_MOV_Rw_Iw(void)
{
	Rw = immWord;
	asm (".global _OP_MOV_Rw_Iw_END\n _OP_MOV_Rw_Iw_END:");
}

void OP_MOV_Mw_Iw_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteWord(address, immWord);
	asm (".global _OP_MOV_Mw_Iw_A16_END\n _OP_MOV_Mw_Iw_A16_END:");
}

void OP_MOV_Mw_Iw_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteWord(address, immWord);
	asm (".global _OP_MOV_Mw_Iw_A32_END\n _OP_MOV_Mw_Iw_A32_END:");
}

void OP_MOV_Rd_Id(void)
{
	Rd = immDword;
	asm (".global _OP_MOV_Rd_Id_END\n _OP_MOV_Rd_Id_END:");
}

void OP_MOV_Md_Id_A16(void)
{
	Dword address = CalculateAddressA16();
	WriteDword(address, immDword);
	asm (".global _OP_MOV_Md_Id_A16_END\n _OP_MOV_Md_Id_A16_END:");
}

void OP_MOV_Md_Id_A32(void)
{
	Dword address = CalculateAddressA32();
	WriteDword(address, immDword);
	asm (".global _OP_MOV_Md_Id_A32_END\n _OP_MOV_Md_Id_A32_END:");
}

void OP_MOV_AL_Ob_A16(void)
{
	registers->r_al = ReadByte(segRegBase + immWord);
	asm (".global _OP_MOV_AL_Ob_A16_END\n _OP_MOV_AL_Ob_A16_END:");
}

void OP_MOV_AX_Ow_A16(void)
{
	registers->r_ax = ReadWord(segRegBase + immWord);
	asm (".global _OP_MOV_AX_Ow_A16_END\n _OP_MOV_AX_Ow_A16_END:");
}

void OP_MOV_EAX_Od_A16(void)
{
	registers->r_eax = ReadDword(segRegBase + immWord);
	asm (".global _OP_MOV_EAX_Od_A16_END\n _OP_MOV_EAX_Od_A16_END:");
}

void OP_MOV_AL_Ob_A32(void)
{
	registers->r_al = ReadByte(segRegBase + immDword);
	asm (".global _OP_MOV_AL_Ob_A32_END\n _OP_MOV_AL_Ob_A32_END:");
}

void OP_MOV_AX_Ow_A32(void)
{
	registers->r_ax = ReadWord(segRegBase + immDword);
	asm (".global _OP_MOV_AX_Ow_A32_END\n _OP_MOV_AX_Ow_A32_END:");
}

void OP_MOV_EAX_Od_A32(void)
{
	registers->r_eax = ReadDword(segRegBase + immDword);
	asm (".global _OP_MOV_EAX_Od_A32_END\n _OP_MOV_EAX_Od_A32_END:");
}

void OP_MOV_Ob_AL_A16(void)
{
	WriteByte(segRegBase + immWord, registers->r_al);
	asm (".global _OP_MOV_Ob_AL_A16_END\n _OP_MOV_Ob_AL_A16_END:");
}

void OP_MOV_Ow_AX_A16(void)
{
	WriteWord(segRegBase + immWord, registers->r_ax);
	asm (".global _OP_MOV_Ow_AX_A16_END\n _OP_MOV_Ow_AX_A16_END:");
}

void OP_MOV_Od_EAX_A16(void)
{
	WriteDword(segRegBase + immWord, registers->r_eax);
	asm (".global _OP_MOV_Od_EAX_A16_END\n _OP_MOV_Od_EAX_A16_END:");
}

void OP_MOV_Ob_AL_A32(void)
{
	WriteByte(segRegBase + immDword, registers->r_al);
	asm (".global _OP_MOV_Ob_AL_A32_END\n _OP_MOV_Ob_AL_A32_END:");
}

void OP_MOV_Ow_AX_A32(void)
{
	WriteWord(segRegBase + immDword, registers->r_ax);
	asm (".global _OP_MOV_Ow_AX_A32_END\n _OP_MOV_Ow_AX_A32_END:");
}

void OP_MOV_Od_EAX_A32(void)
{
	WriteDword(segRegBase + immDword, registers->r_eax);
	asm (".global _OP_MOV_Od_EAX_A32_END\n _OP_MOV_Od_EAX_A32_END:");
}

void OP_MOVZX_Gw_Rb(void)
{
	Gw = Rb;
	asm (".global _OP_MOVZX_Gw_Rb_END\n _OP_MOVZX_Gw_Rb_END:");
}

void OP_MOVZX_Gw_Mb_A16(void)
{
	Gw = ReadByte(CalculateAddressA16());
	asm (".global _OP_MOVZX_Gw_Mb_A16_END\n _OP_MOVZX_Gw_Mb_A16_END:");
}

void OP_MOVZX_Gw_Mb_A32(void)
{
	Gw = ReadByte(CalculateAddressA32());
	asm (".global _OP_MOVZX_Gw_Mb_A32_END\n _OP_MOVZX_Gw_Mb_A32_END:");
}

void OP_MOVZX_Gd_Rb(void)
{
	Gd = Rb;
	asm (".global _OP_MOVZX_Gd_Rb_END\n _OP_MOVZX_Gd_Rb_END:");
}

void OP_MOVZX_Gd_Mb_A16(void)
{
	Gd = ReadByte(CalculateAddressA16());
	asm (".global _OP_MOVZX_Gd_Mb_A16_END\n _OP_MOVZX_Gd_Mb_A16_END:");
}

void OP_MOVZX_Gd_Mb_A32(void)
{
	Gd = ReadByte(CalculateAddressA32());
	asm (".global _OP_MOVZX_Gd_Mb_A32_END\n _OP_MOVZX_Gd_Mb_A32_END:");
}

void OP_MOVZX_Gw_Rw(void)
{
	Gw = Rw;
	asm (".global _OP_MOVZX_Gw_Rw_END\n _OP_MOVZX_Gw_Rw_END:");
}

void OP_MOVZX_Gw_Mw_A16(void)
{
	Gw = ReadWord(CalculateAddressA16());
	asm (".global _OP_MOVZX_Gw_Mw_A16_END\n _OP_MOVZX_Gw_Mw_A16_END:");
}

void OP_MOVZX_Gw_Mw_A32(void)
{
	Gw = ReadWord(CalculateAddressA32());
	asm (".global _OP_MOVZX_Gw_Mw_A32_END\n _OP_MOVZX_Gw_Mw_A32_END:");
}

void OP_MOVZX_Gd_Rw(void)
{
	Gd = Rw;
	asm (".global _OP_MOVZX_Gd_Rw_END\n _OP_MOVZX_Gd_Rw_END:");
}

void OP_MOVZX_Gd_Mw_A16(void)
{
	Gd = ReadWord(CalculateAddressA16());
	asm (".global _OP_MOVZX_Gd_Mw_A16_END\n _OP_MOVZX_Gd_Mw_A16_END:");
}

void OP_MOVZX_Gd_Mw_A32(void)
{
	Gd = ReadWord(CalculateAddressA32());
	asm (".global _OP_MOVZX_Gd_Mw_A32_END\n _OP_MOVZX_Gd_Mw_A32_END:");
}

void OP_MOVSX_Gw_Rb(void)
{
	Gw = (SignedByte)Rb;
	asm (".global _OP_MOVSX_Gw_Rb_END\n _OP_MOVSX_Gw_Rb_END:");
}

void OP_MOVSX_Gw_Mb_A16(void)
{
	Gw = (SignedByte)ReadByte(CalculateAddressA16());
	asm (".global _OP_MOVSX_Gw_Mb_A16_END\n _OP_MOVSX_Gw_Mb_A16_END:");
}

void OP_MOVSX_Gw_Mb_A32(void)
{
	Gw = (SignedByte)ReadByte(CalculateAddressA32());
	asm (".global _OP_MOVSX_Gw_Mb_A32_END\n _OP_MOVSX_Gw_Mb_A32_END:");
}

void OP_MOVSX_Gd_Rb(void)
{
	Gd = (SignedByte)Rb;
	asm (".global _OP_MOVSX_Gd_Rb_END\n _OP_MOVSX_Gd_Rb_END:");
}

void OP_MOVSX_Gd_Mb_A16(void)
{
	Gd = (SignedByte)ReadByte(CalculateAddressA16());
	asm (".global _OP_MOVSX_Gd_Mb_A16_END\n _OP_MOVSX_Gd_Mb_A16_END:");
}

void OP_MOVSX_Gd_Mb_A32(void)
{
	Gd = (SignedByte)ReadByte(CalculateAddressA32());
	asm (".global _OP_MOVSX_Gd_Mb_A32_END\n _OP_MOVSX_Gd_Mb_A32_END:");
}

void OP_MOVSX_Gw_Rw(void)
{
	Gw = (SignedWord)Rw;
	asm (".global _OP_MOVSX_Gw_Rw_END\n _OP_MOVSX_Gw_Rw_END:");
}

void OP_MOVSX_Gw_Mw_A16(void)
{
	Gw = (SignedWord)ReadWord(CalculateAddressA16());
	asm (".global _OP_MOVSX_Gw_Mw_A16_END\n _OP_MOVSX_Gw_Mw_A16_END:");
}

void OP_MOVSX_Gw_Mw_A32(void)
{
	Gw = (SignedWord)ReadWord(CalculateAddressA32());
	asm (".global _OP_MOVSX_Gw_Mw_A32_END\n _OP_MOVSX_Gw_Mw_A32_END:");
}

void OP_MOVSX_Gd_Rw(void)
{
	Gd = (SignedWord)Rw;
	asm (".global _OP_MOVSX_Gd_Rw_END\n _OP_MOVSX_Gd_Rw_END:");
}

void OP_MOVSX_Gd_Mw_A16(void)
{
	Gd = (SignedWord)ReadWord(CalculateAddressA16());
	asm (".global _OP_MOVSX_Gd_Mw_A16_END\n _OP_MOVSX_Gd_Mw_A16_END:");
}

void OP_MOVSX_Gd_Mw_A32(void)
{
	Gd = (SignedWord)ReadWord(CalculateAddressA32());
	asm (".global _OP_MOVSX_Gd_Mw_A32_END\n _OP_MOVSX_Gd_Mw_A32_END:");
}

void OP_MOV_Rd_CR0(void)
{
	Rd = registers->r_cr0;
	asm (".global _OP_MOV_Rd_CR0_END\n _OP_MOV_Rd_CR0_END:");
}

void OP_MOV_Rd_CR2(void)
{
	Rd = registers->r_cr2;
	asm (".global _OP_MOV_Rd_CR2_END\n _OP_MOV_Rd_CR2_END:");
}

void OP_MOV_Rd_CR3(void)
{
	Rd = registers->r_cr3;
	asm (".global _OP_MOV_Rd_CR3_END\n _OP_MOV_Rd_CR3_END:");
}

void OP_MOV_Rd_CR4(void)
{
	Rd = registers->r_cr4;
	asm (".global _OP_MOV_Rd_CR4_END\n _OP_MOV_Rd_CR4_END:");
}

void OP_MOV_Rd_DR0(void) { Rd = registers->r_dr[0]; asm (".global _OP_MOV_Rd_DR0_END\n _OP_MOV_Rd_DR0_END:"); }
void OP_MOV_Rd_DR1(void) { Rd = registers->r_dr[1]; asm (".global _OP_MOV_Rd_DR1_END\n _OP_MOV_Rd_DR1_END:"); }
void OP_MOV_Rd_DR2(void) { Rd = registers->r_dr[2]; asm (".global _OP_MOV_Rd_DR2_END\n _OP_MOV_Rd_DR2_END:"); }
void OP_MOV_Rd_DR3(void) { Rd = registers->r_dr[3]; asm (".global _OP_MOV_Rd_DR3_END\n _OP_MOV_Rd_DR3_END:"); }
void OP_MOV_Rd_DR4(void) { Rd = registers->r_dr[4]; asm (".global _OP_MOV_Rd_DR4_END\n _OP_MOV_Rd_DR4_END:"); }
void OP_MOV_Rd_DR5(void) { Rd = registers->r_dr[5]; asm (".global _OP_MOV_Rd_DR5_END\n _OP_MOV_Rd_DR5_END:"); }
void OP_MOV_Rd_DR6(void) { Rd = registers->r_dr[6]; asm (".global _OP_MOV_Rd_DR6_END\n _OP_MOV_Rd_DR6_END:"); }
void OP_MOV_Rd_DR7(void) { Rd = registers->r_dr[7]; asm (".global _OP_MOV_Rd_DR7_END\n _OP_MOV_Rd_DR7_END:"); }

void OP_CMOVO_Gw_Rw(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gw = Rw;

	asm (".global _OP_CMOVO_Gw_Rw_END\n _OP_CMOVO_Gw_Rw_END:");
}

void OP_CMOVO_Gw_Mw_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gw = ReadWord(CalculateAddressA16());

	asm (".global _OP_CMOVO_Gw_Mw_A16_END\n _OP_CMOVO_Gw_Mw_A16_END:");
}

void OP_CMOVO_Gw_Mw_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gw = ReadWord(CalculateAddressA32());

	asm (".global _OP_CMOVO_Gw_Mw_A32_END\n _OP_CMOVO_Gw_Mw_A32_END:");
}

void OP_CMOVO_Gd_Rd(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gd = Rd;

	asm (".global _OP_CMOVO_Gd_Rd_END\n _OP_CMOVO_Gd_Rd_END:");
}

void OP_CMOVO_Gd_Md_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gw = ReadDword(CalculateAddressA16());

	asm (".global _OP_CMOVO_Gd_Md_A16_END\n _OP_CMOVO_Gd_Md_A16_END:");
}

void OP_CMOVO_Gd_Md_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gd = ReadDword(CalculateAddressA32());

	asm (".global _OP_CMOVO_Gd_Md_A32_END\n _OP_CMOVO_Gd_Md_A32_END:");
}

void OP_CMOVNO_Gw_Rw(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gw = Rw;

	asm (".global _OP_CMOVNO_Gw_Rw_END\n _OP_CMOVNO_Gw_Rw_END:");
}

void OP_CMOVNO_Gw_Mw_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gw = ReadWord(CalculateAddressA16());

	asm (".global _OP_CMOVNO_Gw_Mw_A16_END\n _OP_CMOVNO_Gw_Mw_A16_END:");
}

void OP_CMOVNO_Gw_Mw_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gw = ReadWord(CalculateAddressA32());

	asm (".global _OP_CMOVNO_Gw_Mw_A32_END\n _OP_CMOVNO_Gw_Mw_A32_END:");
}

void OP_CMOVNO_Gd_Rd(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gd = Rd;

	asm (".global _OP_CMOVNO_Gd_Rd_END\n _OP_CMOVNO_Gd_Rd_END:");
}

void OP_CMOVNO_Gd_Md_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gw = ReadDword(CalculateAddressA16());

	asm (".global _OP_CMOVNO_Gd_Md_A16_END\n _OP_CMOVNO_Gd_Md_A16_END:");
}

void OP_CMOVNO_Gd_Md_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Gd = ReadDword(CalculateAddressA32());

	asm (".global _OP_CMOVNO_Gd_Md_A32_END\n _OP_CMOVNO_Gd_Md_A32_END:");
}

void OP_CMOVL_Gw_Rw(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Gw = Rw;

	asm (".global _OP_CMOVL_Gw_Rw_END\n _OP_CMOVL_Gw_Rw_END:");
}

void OP_CMOVL_Gw_Mw_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadWord(CalculateAddressA16());
	}

	asm (".global _OP_CMOVL_Gw_Mw_A16_END\n _OP_CMOVL_Gw_Mw_A16_END:");
}

void OP_CMOVL_Gw_Mw_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadWord(CalculateAddressA32());
	}

	asm (".global _OP_CMOVL_Gw_Mw_A32_END\n _OP_CMOVL_Gw_Mw_A32_END:");
}

void OP_CMOVL_Gd_Rd(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gd = Rd;
	}

	asm (".global _OP_CMOVL_Gd_Rd_END\n _OP_CMOVL_Gd_Rd_END:");
}

void OP_CMOVL_Gd_Md_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadDword(CalculateAddressA16());
	}

	asm (".global _OP_CMOVL_Gd_Md_A16_END\n _OP_CMOVL_Gd_Md_A16_END:");
}

void OP_CMOVL_Gd_Md_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gd = ReadDword(CalculateAddressA32());
	}

	asm (".global _OP_CMOVL_Gd_Md_A32_END\n _OP_CMOVL_Gd_Md_A32_END:");
}

void OP_CMOVNL_Gw_Rw(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = Rw;
	}

	asm (".global _OP_CMOVNL_Gw_Rw_END\n _OP_CMOVNL_Gw_Rw_END:");
}

void OP_CMOVNL_Gw_Mw_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadWord(CalculateAddressA16());
	}

	asm (".global _OP_CMOVNL_Gw_Mw_A16_END\n _OP_CMOVNL_Gw_Mw_A16_END:");
}

void OP_CMOVNL_Gw_Mw_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadWord(CalculateAddressA32());
	}

	asm (".global _OP_CMOVNL_Gw_Mw_A32_END\n _OP_CMOVNL_Gw_Mw_A32_END:");
}

void OP_CMOVNL_Gd_Rd(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gd = Rd;
	}

	asm (".global _OP_CMOVNL_Gd_Rd_END\n _OP_CMOVNL_Gd_Rd_END:");
}

void OP_CMOVNL_Gd_Md_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gw = ReadDword(CalculateAddressA16());
	}

	asm (".global _OP_CMOVNL_Gd_Md_A16_END\n _OP_CMOVNL_Gd_Md_A16_END:");
}

void OP_CMOVNL_Gd_Md_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Gd = ReadDword(CalculateAddressA32());
	}

	asm (".global _OP_CMOVNL_Gd_Md_A32_END\n _OP_CMOVNL_Gd_Md_A32_END:");
}

void OP_CMOVLE_Gw_Rw(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gw = Rw;
	}

	asm (".global _OP_CMOVLE_Gw_Rw_END\n _OP_CMOVLE_Gw_Rw_END:");
}

void OP_CMOVLE_Gw_Mw_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gw = ReadWord(CalculateAddressA16());
	}

	asm (".global _OP_CMOVLE_Gw_Mw_A16_END\n _OP_CMOVLE_Gw_Mw_A16_END:");
}

void OP_CMOVLE_Gw_Mw_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gw = ReadWord(CalculateAddressA32());
	}

	asm (".global _OP_CMOVLE_Gw_Mw_A32_END\n _OP_CMOVLE_Gw_Mw_A32_END:");
}

void OP_CMOVLE_Gd_Rd(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gd = Rd;
	}

	asm (".global _OP_CMOVLE_Gd_Rd_END\n _OP_CMOVLE_Gd_Rd_END:");
}

void OP_CMOVLE_Gd_Md_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gw = ReadDword(CalculateAddressA16());
	}

	asm (".global _OP_CMOVLE_Gd_Md_A16_END\n _OP_CMOVLE_Gd_Md_A16_END:");
}

void OP_CMOVLE_Gd_Md_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Gd = ReadDword(CalculateAddressA32());
	}

	asm (".global _OP_CMOVLE_Gd_Md_A32_END\n _OP_CMOVLE_Gd_Md_A32_END:");
}

void OP_CMOVNLE_Gw_Rw(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gw = Rw;
	}

	asm (".global _OP_CMOVNLE_Gw_Rw_END\n _OP_CMOVNLE_Gw_Rw_END:");
}

void OP_CMOVNLE_Gw_Mw_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gw = ReadWord(CalculateAddressA16());
	}

	asm (".global _OP_CMOVNLE_Gw_Mw_A16_END\n _OP_CMOVNLE_Gw_Mw_A16_END:");
}

void OP_CMOVNLE_Gw_Mw_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gw = ReadWord(CalculateAddressA32());
	}

	asm (".global _OP_CMOVNLE_Gw_Mw_A32_END\n _OP_CMOVNLE_Gw_Mw_A32_END:");
}

void OP_CMOVNLE_Gd_Rd(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gd = Rd;
	}

	asm (".global _OP_CMOVNLE_Gd_Rd_END\n _OP_CMOVNLE_Gd_Rd_END:");
}

void OP_CMOVNLE_Gd_Md_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gw = ReadDword(CalculateAddressA16());
	}

	asm (".global _OP_CMOVNLE_Gd_Md_A16_END\n _OP_CMOVNLE_Gd_Md_A16_END:");
}

void OP_CMOVNLE_Gd_Md_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Gd = ReadDword(CalculateAddressA32());
	}

	asm (".global _OP_CMOVNLE_Gd_Md_A32_END\n _OP_CMOVNLE_Gd_Md_A32_END:");
}

void OP_CMPXCHG_Rb_Gb(void)
{
	//Perform the comparison
	asm volatile
	(
		"cmpb %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "q" (registers->r_al), "q" (Rb)
	);

	//If equal, Rb = Gb. Otherwise, AL = Rb
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		Rb = Gb;
	else
		registers->r_al = Rb;

	asm (".global _OP_CMPXCHG_Rb_Gb_END\n _OP_CMPXCHG_Rb_Gb_END:");
}

void OP_CMPXCHG_Mb_Gb_A16(void)
{
	Dword address = CalculateAddressA16();
	Byte dest = ReadByte(address);

	asm volatile
	(
		"cmpb %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "q" (registers->r_al), "q" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteByte(address, Gb);
	else
		registers->r_al = dest;

	asm (".global _OP_CMPXCHG_Mb_Gb_A16_END\n _OP_CMPXCHG_Mb_Gb_A16_END:");
}

void OP_CMPXCHG_Mb_Gb_A32(void)
{
	Dword address = CalculateAddressA32();
	Byte dest = ReadByte(address);

	asm volatile
	(
		"cmpb %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "q" (registers->r_al), "q" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteByte(address, Gb);
	else
		registers->r_al = dest;

	asm (".global _OP_CMPXCHG_Mb_Gb_A32_END\n _OP_CMPXCHG_Mb_Gb_A32_END:");
}

void OP_CMPXCHG_Rw_Gw(void)
{
	asm volatile
	(
		"cmpw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_ax), "r" (Rw)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		Rw = Gw;
	else
		registers->r_ax = Rw;

	asm (".global _OP_CMPXCHG_Rw_Gw_END\n _OP_CMPXCHG_Rw_Gw_END:");
}

void OP_CMPXCHG_Mw_Gw_A16(void)
{
	Dword address = CalculateAddressA16();
	Word dest = ReadWord(address);

	asm volatile
	(
		"cmpw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_ax), "r" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteWord(address, Gw);
	else
		registers->r_ax = dest;

	asm (".global _OP_CMPXCHG_Mw_Gw_A16_END\n _OP_CMPXCHG_Mw_Gw_A16_END:");
}

void OP_CMPXCHG_Mw_Gw_A32(void)
{
	Dword address = CalculateAddressA32();
	Word dest = ReadWord(address);

	asm volatile
	(
		"cmpw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_ax), "r" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteWord(address, Gw);
	else
		registers->r_ax = dest;

	asm (".global _OP_CMPXCHG_Mw_Gw_A32_END\n _OP_CMPXCHG_Mw_Gw_A32_END:");
}

void OP_CMPXCHG_Rd_Gd(void)
{
	asm volatile
	(
		"cmpl %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_eax), "r" (Rd)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		Rd = Gd;
	else
		registers->r_eax = Rd;

	asm (".global _OP_CMPXCHG_Rd_Gd_END\n _OP_CMPXCHG_Rd_Gd_END:");
}

void OP_CMPXCHG_Md_Gd_A16(void)
{
	Dword address = CalculateAddressA16();
	Dword dest = ReadDword(address);

	asm volatile
	(
		"cmpl %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_eax), "r" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteDword(address, Gd);
	else
		registers->r_eax = dest;

	asm (".global _OP_CMPXCHG_Md_Gd_A16_END\n _OP_CMPXCHG_Md_Gd_A16_END:");
}

void OP_CMPXCHG_Md_Gd_A32(void)
{
	Dword address = CalculateAddressA32();
	Dword dest = ReadDword(address);

	asm volatile
	(
		"cmpl %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags)
		: "r" (registers->r_eax), "r" (dest)
	);

	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteDword(address, Gd);
	else
		registers->r_eax = dest;

	asm (".global _OP_CMPXCHG_Md_Gd_A32_END\n _OP_CMPXCHG_Md_Gd_A32_END:");
}
