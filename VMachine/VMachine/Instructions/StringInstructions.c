//	VMachine
//	Implementations of string instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define MOVS(opSize, regOffset, srcReg, destReg)	\
	opSize data = Read##opSize(segRegBase + registers->r_##srcReg);	\
	Write##opSize(registers->r_es.base + registers->r_##destReg, data);	\
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)	\
	{							\
		registers->r_##srcReg += regOffset;		\
		registers->r_##destReg += regOffset;	\
	}							\
	else						\
	{							\
		registers->r_##srcReg -= regOffset;		\
		registers->r_##destReg -= regOffset;	\
	}

#define CMPS(opSize, opSizeCode, regOffset, srcReg, destReg)	\
	opSize source = Read##opSize(segRegBase + registers->r_##srcReg);	\
	opSize dest = Read##opSize(registers->r_es.base + registers->r_##destReg);	\
	asm volatile				\
	(							\
		"cmp"#opSizeCode" %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (dest), "r" (source)	\
	);							\
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)	\
	{							\
		registers->r_##srcReg += regOffset;		\
		registers->r_##destReg += regOffset;	\
	}							\
	else						\
	{							\
		registers->r_##srcReg -= regOffset;		\
		registers->r_##destReg -= regOffset;	\
	}

#define STOS(opSize, regOffset, srcReg, destReg)	\
	Write##opSize(registers->r_es.base + registers->r_##destReg, registers->r_##srcReg);	\
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)	\
		registers->r_##destReg += regOffset;	\
	else						\
		registers->r_##destReg -= regOffset;

#define LODS(opSize, regOffset, srcReg, destReg)	\
	registers->r_##destReg = Read##opSize(segRegBase + registers->r_##srcReg);	\
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)	\
		registers->r_##srcReg += regOffset;	\
	else						\
		registers->r_##srcReg -= regOffset;

#define SCAS(opSize, opSizeCode, regOffset, srcReg, destReg)	\
	opSize dest = Read##opSize(registers->r_es.base + registers->r_##destReg);	\
	asm volatile				\
	(							\
		"cmp"#opSizeCode" %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (dest), "r" (registers->r_##srcReg)	\
	);							\
	if((registers->r_systemFlags & EFLAGS_D_FLAG) == 0)	\
		registers->r_##destReg += regOffset;	\
	else						\
		registers->r_##destReg -= regOffset;

void OP_MOVS_Yb_Xb_A16(void)
{
	MOVS(Byte, 1, si, di)

	asm (".global _OP_MOVS_Yb_Xb_A16_END\n _OP_MOVS_Yb_Xb_A16_END:");
}

void OP_REP_MOVS_Yb_Xb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Byte, 1, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yb_Xb_A16_END\n _OP_REP_MOVS_Yb_Xb_A16_END:");
}

void OP_MOVS_Yb_Xb_A32(void)
{
	MOVS(Byte, 1, esi, edi)

	asm (".global _OP_MOVS_Yb_Xb_A32_END\n _OP_MOVS_Yb_Xb_A32_END:");
}

void OP_REP_MOVS_Yb_Xb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Byte, 1, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yb_Xb_A32_END\n _OP_REP_MOVS_Yb_Xb_A32_END:");
}

void OP_MOVS_Yw_Xw_A16(void)
{
	MOVS(Word, 2, si, di)

	asm (".global _OP_MOVS_Yw_Xw_A16_END\n _OP_MOVS_Yw_Xw_A16_END:");
}

void OP_REP_MOVS_Yw_Xw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Word, 2, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yw_Xw_A16_END\n _OP_REP_MOVS_Yw_Xw_A16_END:");
}

void OP_MOVS_Yw_Xw_A32(void)
{
	MOVS(Word, 2, esi, edi)

	asm (".global _OP_MOVS_Yw_Xw_A32_END\n _OP_MOVS_Yw_Xw_A32_END:");
}

void OP_REP_MOVS_Yw_Xw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Word, 2, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yw_Xw_A32_END\n _OP_REP_MOVS_Yw_Xw_A32_END:");
}

void OP_MOVS_Yd_Xd_A16(void)
{
	MOVS(Dword, 4, si, di)

	asm (".global _OP_MOVS_Yd_Xd_A16_END\n _OP_MOVS_Yd_Xd_A16_END:");
}

void OP_REP_MOVS_Yd_Xd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Dword, 4, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yd_Xd_A16_END\n _OP_REP_MOVS_Yd_Xd_A16_END:");
}

void OP_MOVS_Yd_Xd_A32(void)
{
	MOVS(Dword, 4, esi, edi)

	asm (".global _OP_MOVS_Yd_Xd_A32_END\n _OP_MOVS_Yd_Xd_A32_END:");
}

void OP_REP_MOVS_Yd_Xd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	MOVS(Dword, 4, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_MOVS_Yd_Xd_A32_END\n _OP_REP_MOVS_Yd_Xd_A32_END:");
}

void OP_CMPS_Yb_Xb_A16(void)
{
	CMPS(Byte, b, 1, si, di)

	asm (".global _OP_CMPS_Yb_Xb_A16_END\n _OP_CMPS_Yb_Xb_A16_END:");
}

void OP_REPE_CMPS_Yb_Xb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Byte, b, 1, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yb_Xb_A16_END\n _OP_REPE_CMPS_Yb_Xb_A16_END:");
}

void OP_REPNE_CMPS_Yb_Xb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Byte, b, 1, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yb_Xb_A16_END\n _OP_REPNE_CMPS_Yb_Xb_A16_END:");
}

void OP_CMPS_Yb_Xb_A32(void)
{
	CMPS(Byte, b, 1, esi, edi)

	asm (".global _OP_CMPS_Yb_Xb_A32_END\n _OP_CMPS_Yb_Xb_A32_END:");
}

void OP_REPE_CMPS_Yb_Xb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Byte, b, 1, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yb_Xb_A32_END\n _OP_REPE_CMPS_Yb_Xb_A32_END:");
}

void OP_REPNE_CMPS_Yb_Xb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Byte, b, 1, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yb_Xb_A32_END\n _OP_REPNE_CMPS_Yb_Xb_A32_END:");
}

void OP_CMPS_Yw_Xw_A16(void)
{
	CMPS(Word, w, 2, si, di)

	asm (".global _OP_CMPS_Yw_Xw_A16_END\n _OP_CMPS_Yw_Xw_A16_END:");
}

void OP_REPE_CMPS_Yw_Xw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Word, w, 2, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yw_Xw_A16_END\n _OP_REPE_CMPS_Yw_Xw_A16_END:");
}

void OP_REPNE_CMPS_Yw_Xw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Word, w, 2, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yw_Xw_A16_END\n _OP_REPNE_CMPS_Yw_Xw_A16_END:");
}

void OP_CMPS_Yw_Xw_A32(void)
{
	CMPS(Word, w, 2, esi, edi)

	asm (".global _OP_CMPS_Yw_Xw_A32_END\n _OP_CMPS_Yw_Xw_A32_END:");
}

void OP_REPE_CMPS_Yw_Xw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Word, w, 2, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yw_Xw_A32_END\n _OP_REPE_CMPS_Yw_Xw_A32_END:");
}

void OP_REPNE_CMPS_Yw_Xw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Word, w, 2, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yw_Xw_A32_END\n _OP_REPNE_CMPS_Yw_Xw_A32_END:");
}

void OP_CMPS_Yd_Xd_A16(void)
{
	CMPS(Dword, l, 4, si, di)

	asm (".global _OP_CMPS_Yd_Xd_A16_END\n _OP_CMPS_Yd_Xd_A16_END:");
}

void OP_REPE_CMPS_Yd_Xd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Dword, l, 4, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yd_Xd_A16_END\n _OP_REPE_CMPS_Yd_Xd_A16_END:");
}

void OP_REPNE_CMPS_Yd_Xd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Dword, l, 4, si, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yd_Xd_A16_END\n _OP_REPNE_CMPS_Yd_Xd_A16_END:");
}

void OP_CMPS_Yd_Xd_A32(void)
{
	CMPS(Dword, l, 4, esi, edi)

	asm (".global _OP_CMPS_Yd_Xd_A32_END\n _OP_CMPS_Yd_Xd_A32_END:");
}

void OP_REPE_CMPS_Yd_Xd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Dword, l, 4, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_CMPS_Yd_Xd_A32_END\n _OP_REPE_CMPS_Yd_Xd_A32_END:");
}

void OP_REPNE_CMPS_Yd_Xd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	CMPS(Dword, l, 4, esi, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_CMPS_Yd_Xd_A32_END\n _OP_REPNE_CMPS_Yd_Xd_A32_END:");
}

void OP_STOS_Yb_AL_A16(void)
{
	STOS(Byte, 1, al, di)

	asm (".global _OP_STOS_Yb_AL_A16_END\n _OP_STOS_Yb_AL_A16_END:");
}

void OP_REP_STOS_Yb_AL_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Byte, 1, al, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yb_AL_A16_END\n _OP_REP_STOS_Yb_AL_A16_END:");
}

void OP_STOS_Yb_AL_A32(void)
{
	STOS(Byte, 1, al, edi)

	asm (".global _OP_STOS_Yb_AL_A32_END\n _OP_STOS_Yb_AL_A32_END:");
}

void OP_REP_STOS_Yb_AL_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Byte, 1, al, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yb_AL_A32_END\n _OP_REP_STOS_Yb_AL_A32_END:");
}

void OP_STOS_Yw_AX_A16(void)
{
	STOS(Word, 2, ax, di)

	asm (".global _OP_STOS_Yw_AX_A16_END\n _OP_STOS_Yw_AX_A16_END:");
}

void OP_REP_STOS_Yw_AX_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Word, 2, ax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yw_AX_A16_END\n _OP_REP_STOS_Yw_AX_A16_END:");
}

void OP_STOS_Yw_AX_A32(void)
{
	STOS(Word, 2, ax, edi)

	asm (".global _OP_STOS_Yw_AX_A32_END\n _OP_STOS_Yw_AX_A32_END:");
}

void OP_REP_STOS_Yw_AX_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Word, 2, ax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yw_AX_A32_END\n _OP_REP_STOS_Yw_AX_A32_END:");
}

void OP_STOS_Yd_EAX_A16(void)
{
	STOS(Dword, 4, eax, di)

	asm (".global _OP_STOS_Yd_EAX_A16_END\n _OP_STOS_Yd_EAX_A16_END:");
}

void OP_REP_STOS_Yd_EAX_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Dword, 4, eax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yd_EAX_A16_END\n _OP_REP_STOS_Yd_EAX_A16_END:");
}

void OP_STOS_Yd_EAX_A32(void)
{
	STOS(Dword, 4, eax, edi)

	asm (".global _OP_STOS_Yd_EAX_A32_END\n _OP_STOS_Yd_EAX_A32_END:");
}

void OP_REP_STOS_Yd_EAX_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	STOS(Dword, 4, eax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_STOS_Yd_EAX_A32_END\n _OP_REP_STOS_Yd_EAX_A32_END:");
}

void OP_LODS_AL_Xb_A16(void)
{
	LODS(Byte, 1, si, al)

	asm (".global _OP_LODS_AL_Xb_A16_END\n _OP_LODS_AL_Xb_A16_END:");
}

void OP_REP_LODS_AL_Xb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Byte, 1, si, al)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_AL_Xb_A16_END\n _OP_REP_LODS_AL_Xb_A16_END:");
}

void OP_LODS_AL_Xb_A32(void)
{
	LODS(Byte, 1, esi, al)

	asm (".global _OP_LODS_AL_Xb_A32_END\n _OP_LODS_AL_Xb_A32_END:");
}

void OP_REP_LODS_AL_Xb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Byte, 1, esi, al)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_AL_Xb_A32_END\n _OP_REP_LODS_AL_Xb_A32_END:");
}

void OP_LODS_AX_Xw_A16(void)
{
	LODS(Word, 2, si, ax)

	asm (".global _OP_LODS_AX_Xw_A16_END\n _OP_LODS_AX_Xw_A16_END:");
}

void OP_REP_LODS_AX_Xw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Word, 2, si, ax)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_AX_Xw_A16_END\n _OP_REP_LODS_AX_Xw_A16_END:");
}

void OP_LODS_AX_Xw_A32(void)
{
	LODS(Word, 2, esi, ax)

	asm (".global _OP_LODS_AX_Xw_A32_END\n _OP_LODS_AX_Xw_A32_END:");
}

void OP_REP_LODS_AX_Xw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Word, 2, esi, ax)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_AX_Xw_A32_END\n _OP_REP_LODS_AX_Xw_A32_END:");
}

void OP_LODS_EAX_Xd_A16(void)
{
	LODS(Dword, 4, si, eax)

	asm (".global _OP_LODS_EAX_Xd_A16_END\n _OP_LODS_EAX_Xd_A16_END:");
}

void OP_REP_LODS_EAX_Xd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Dword, 4, si, eax)

	//Repeat if necessary
	if(--registers->r_cx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_EAX_Xd_A16_END\n _OP_REP_LODS_EAX_Xd_A16_END:");
}

void OP_LODS_EAX_Xd_A32(void)
{
	LODS(Dword, 4, esi, eax)

	asm (".global _OP_LODS_EAX_Xd_A32_END\n _OP_LODS_EAX_Xd_A32_END:");
}

void OP_REP_LODS_EAX_Xd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	LODS(Dword, 4, esi, eax)

	//Repeat if necessary
	if(--registers->r_ecx == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REP_LODS_EAX_Xd_A32_END\n _OP_REP_LODS_EAX_Xd_A32_END:");
}

void OP_SCAS_AL_Yb_A16(void)
{
	SCAS(Byte, b, 1, al, di)

	asm (".global _OP_SCAS_AL_Yb_A16_END\n _OP_SCAS_AL_Yb_A16_END:");
}

void OP_REPE_SCAS_AL_Yb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Byte, b, 1, al, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_AL_Yb_A16_END\n _OP_REPE_SCAS_AL_Yb_A16_END:");
}

void OP_REPNE_SCAS_AL_Yb_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Byte, b, 1, al, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_AL_Yb_A16_END\n _OP_REPNE_SCAS_AL_Yb_A16_END:");
}

void OP_SCAS_AL_Yb_A32(void)
{
	SCAS(Byte, b, 1, al, edi)

	asm (".global _OP_SCAS_AL_Yb_A32_END\n _OP_SCAS_AL_Yb_A32_END:");
}

void OP_REPE_SCAS_AL_Yb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Byte, b, 1, al, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_AL_Yb_A32_END\n _OP_REPE_SCAS_AL_Yb_A32_END:");
}

void OP_REPNE_SCAS_AL_Yb_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Byte, b, 1, al, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_AL_Yb_A32_END\n _OP_REPNE_SCAS_AL_Yb_A32_END:");
}

void OP_SCAS_AX_Yw_A16(void)
{
	SCAS(Word, w, 2, ax, di)

	asm (".global _OP_SCAS_AX_Yw_A16_END\n _OP_SCAS_AX_Yw_A16_END:");
}

void OP_REPE_SCAS_AX_Yw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Word, w, 2, ax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_AX_Yw_A16_END\n _OP_REPE_SCAS_AX_Yw_A16_END:");
}

void OP_REPNE_SCAS_AX_Yw_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Word, w, 2, ax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_AX_Yw_A16_END\n _OP_REPNE_SCAS_AX_Yw_A16_END:");
}

void OP_SCAS_AX_Yw_A32(void)
{
	SCAS(Word, w, 2, ax, edi)

	asm (".global _OP_SCAS_AX_Yw_A32_END\n _OP_SCAS_AX_Yw_A32_END:");
}

void OP_REPE_SCAS_AX_Yw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Word, w, 2, ax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_AX_Yw_A32_END\n _OP_REPE_SCAS_AX_Yw_A32_END:");
}

void OP_REPNE_SCAS_AX_Yw_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Word, w, 2, ax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_AX_Yw_A32_END\n _OP_REPNE_SCAS_AX_Yw_A32_END:");
}

void OP_SCAS_EAX_Yd_A16(void)
{
	SCAS(Dword, l, 4, eax, di)

	asm (".global _OP_SCAS_EAX_Yd_A16_END\n _OP_SCAS_EAX_Yd_A16_END:");
}

void OP_REPE_SCAS_EAX_Yd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Dword, l, 4, eax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_EAX_Yd_A16_END\n _OP_REPE_SCAS_EAX_Yd_A16_END:");
}

void OP_REPNE_SCAS_EAX_Yd_A16(void)
{
	//If the count register is zero, no operation
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Dword, l, 4, eax, di)

	//Repeat if necessary
	if(--registers->r_cx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_EAX_Yd_A16_END\n _OP_REPNE_SCAS_EAX_Yd_A16_END:");
}

void OP_SCAS_EAX_Yd_A32(void)
{
	SCAS(Dword, l, 4, eax, edi)

	asm (".global _OP_SCAS_EAX_Yd_A32_END\n _OP_SCAS_EAX_Yd_A32_END:");
}

void OP_REPE_SCAS_EAX_Yd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Dword, l, 4, eax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPE_SCAS_EAX_Yd_A32_END\n _OP_REPE_SCAS_EAX_Yd_A32_END:");
}

void OP_REPNE_SCAS_EAX_Yd_A32(void)
{
	//If the count register is zero, no operation
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength;
		return;
	}

	SCAS(Dword, l, 4, eax, edi)

	//Repeat if necessary
	if(--registers->r_ecx == 0 || (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength;

	asm (".global _OP_REPNE_SCAS_EAX_Yd_A32_END\n _OP_REPNE_SCAS_EAX_Yd_A32_END:");
}
