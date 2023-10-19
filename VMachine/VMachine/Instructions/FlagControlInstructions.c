//	VMachine
//	Implementations of flag control instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

void OP_CMC(void)
{
	registers->r_eflags ^= EFLAGS_C_FLAG;
	asm (".global _OP_CMC_END\n _OP_CMC_END:");
}

void OP_CLC(void)
{
	registers->r_eflags &= ~EFLAGS_C_FLAG;
	asm (".global _OP_CLC_END\n _OP_CLC_END:");
}

void OP_STC(void)
{
	registers->r_eflags |= EFLAGS_C_FLAG;
	asm (".global _OP_STC_END\n _OP_STC_END:");
}

void OP_CLD(void)
{
	registers->r_systemFlags &= ~EFLAGS_D_FLAG;
	asm (".global _OP_CLD_END\n _OP_CLD_END:");
}

void OP_STD(void)
{
	registers->r_systemFlags |= EFLAGS_D_FLAG;
	asm (".global _OP_STD_END\n _OP_STD_END:");
}

void OP_SAHF(void)
{
	registers->r_eflags &= 0xffffff00;
	registers->r_eflags |= (registers->r_ah & EFLAGS_STATUS_FLAGS_MASK) | 0x02;
	asm (".global _OP_SAHF_END\n _OP_SAHF_END:");
}

void OP_LAHF(void)
{
	registers->r_ah = ((registers->r_eflags & EFLAGS_STATUS_FLAGS_MASK) & 0xff) | 0x02;
	asm (".global _OP_LAHF_END\n _OP_LAHF_END:");
}
