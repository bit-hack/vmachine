//	VMachine
//	Implementations of near control transfer instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

void OP_JMP_Jb_O16(void)
{
	registers->r_eip += instrLength + (SignedByte)immByte;
	registers->r_eip &= 0xffff;

	asm (".global _OP_JMP_Jb_O16_END\n _OP_JMP_Jb_O16_END:");
}

void OP_JMP_Jb_O32(void)
{
	registers->r_eip += instrLength + (SignedByte)immByte;

	asm (".global _OP_JMP_Jb_O32_END\n _OP_JMP_Jb_O32_END:");
}

void OP_JMP_Jw(void)
{
	registers->r_eip += instrLength + immWord;
	registers->r_eip &= 0xffff;

	asm (".global _OP_JMP_Jw_END\n _OP_JMP_Jw_END:");
}

void OP_JMP_Jd(void)
{
	registers->r_eip += instrLength + immDword;
	
	asm (".global _OP_JMP_Jd_END\n _OP_JMP_Jd_END:");
}

void OP_JO_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JO_Jb_O16_END\n _OP_JO_Jb_O16_END:");
}

void OP_JNO_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNO_Jb_O16_END\n _OP_JNO_Jb_O16_END:");
}

void OP_JB_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JB_Jb_O16_END\n _OP_JB_Jb_O16_END:");
}

void OP_JNB_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNB_Jb_O16_END\n _OP_JNB_Jb_O16_END:");
}

void OP_JZ_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JZ_Jb_O16_END\n _OP_JZ_Jb_O16_END:");
}

void OP_JNZ_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNZ_Jb_O16_END\n _OP_JNZ_Jb_O16_END:");
}

void OP_JBE_Jb_O16(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JBE_Jb_O16_END\n _OP_JBE_Jb_O16_END:");
}

void OP_JNBE_Jb_O16(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNBE_Jb_O16_END\n _OP_JNBE_Jb_O16_END:");
}

void OP_JS_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JS_Jb_O16_END\n _OP_JS_Jb_O16_END:");
}

void OP_JNS_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNS_Jb_O16_END\n _OP_JNS_Jb_O16_END:");
}

void OP_JP_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JP_Jb_O16_END\n _OP_JP_Jb_O16_END:");
}

void OP_JNP_Jb_O16(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNP_Jb_O16_END\n _OP_JNP_Jb_O16_END:");
}

void OP_JL_Jb_O16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JL_Jb_O16_END\n _OP_JL_Jb_O16_END:");
}

void OP_JNL_Jb_O16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNL_Jb_O16_END\n _OP_JNL_Jb_O16_END:");
}

void OP_JLE_Jb_O16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JLE_Jb_O16_END\n _OP_JLE_Jb_O16_END:");
}

void OP_JNLE_Jb_O16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNLE_Jb_O16_END\n _OP_JNLE_Jb_O16_END:");
}

void OP_JO_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JO_Jb_O32_END\n _OP_JO_Jb_O32_END:");
}

void OP_JNO_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNO_Jb_O32_END\n _OP_JNO_Jb_O32_END:");
}

void OP_JB_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JB_Jb_O32_END\n _OP_JB_Jb_O32_END:");
}

void OP_JNB_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNB_Jb_O32_END\n _OP_JNB_Jb_O32_END:");
}

void OP_JZ_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JZ_Jb_O32_END\n _OP_JZ_Jb_O32_END:");
}

void OP_JNZ_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNZ_Jb_O32_END\n _OP_JNZ_Jb_O32_END:");
}

void OP_JBE_Jb_O32(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JBE_Jb_O32_END\n _OP_JBE_Jb_O32_END:");
}

void OP_JNBE_Jb_O32(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNBE_Jb_O32_END\n _OP_JNBE_Jb_O32_END:");
}

void OP_JS_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JS_Jb_O32_END\n _OP_JS_Jb_O32_END:");
}

void OP_JNS_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNS_Jb_O32_END\n _OP_JNS_Jb_O32_END:");
}

void OP_JP_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JP_Jb_O32_END\n _OP_JP_Jb_O32_END:");
}

void OP_JNP_Jb_O32(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNP_Jb_O32_END\n _OP_JNP_Jb_O32_END:");
}

void OP_JL_Jb_O32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JL_Jb_O32_END\n _OP_JL_Jb_O32_END:");
}

void OP_JNL_Jb_O32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNL_Jb_O32_END\n _OP_JNL_Jb_O32_END:");
}

void OP_JLE_Jb_O32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JLE_Jb_O32_END\n _OP_JLE_Jb_O32_END:");
}

void OP_JNLE_Jb_O32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNLE_Jb_O32_END\n _OP_JNLE_Jb_O32_END:");
}

void OP_JO_Jw(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JO_Jw_END\n _OP_JO_Jw_END:");
}

void OP_JNO_Jw(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNO_Jw_END\n _OP_JNO_Jw_END:");
}

void OP_JB_Jw(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JB_Jw_END\n _OP_JB_Jw_END:");
}

void OP_JNB_Jw(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNB_Jw_END\n _OP_JNB_Jw_END:");
}

void OP_JZ_Jw(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JZ_Jw_END\n _OP_JZ_Jw_END:");
}

void OP_JNZ_Jw(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNZ_Jw_END\n _OP_JNZ_Jw_END:");
}

void OP_JBE_Jw(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JBE_Jw_END\n _OP_JBE_Jw_END:");
}

void OP_JNBE_Jw(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNBE_Jw_END\n _OP_JNBE_Jw_END:");
}

void OP_JS_Jw(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JS_Jw_END\n _OP_JS_Jw_END:");
}

void OP_JNS_Jw(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNS_Jw_END\n _OP_JNS_Jw_END:");
}

void OP_JP_Jw(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JP_Jw_END\n _OP_JP_Jw_END:");
}

void OP_JNP_Jw(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNP_Jw_END\n _OP_JNP_Jw_END:");
}

void OP_JL_Jw(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JL_Jw_END\n _OP_JL_Jw_END:");
}

void OP_JNL_Jw(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNL_Jw_END\n _OP_JNL_Jw_END:");
}

void OP_JLE_Jw(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JLE_Jw_END\n _OP_JLE_Jw_END:");
}

void OP_JNLE_Jw(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + immWord;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNLE_Jw_END\n _OP_JNLE_Jw_END:");
}

void OP_JO_Jd(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JO_Jd_END\n _OP_JO_Jd_END:");
}

void OP_JNO_Jd(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNO_Jd_END\n _OP_JNO_Jd_END:");
}

void OP_JB_Jd(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JB_Jd_END\n _OP_JB_Jd_END:");
}

void OP_JNB_Jd(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNB_Jd_END\n _OP_JNB_Jd_END:");
}

void OP_JZ_Jd(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JZ_Jd_END\n _OP_JZ_Jd_END:");
}

void OP_JNZ_Jd(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNZ_Jd_END\n _OP_JNZ_Jd_END:");
}

void OP_JBE_Jd(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JBE_Jd_END\n _OP_JBE_Jd_END:");
}

void OP_JNBE_Jd(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNBE_Jd_END\n _OP_JNBE_Jd_END:");
}

void OP_JS_Jd(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JS_Jd_END\n _OP_JS_Jd_END:");
}

void OP_JNS_Jd(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNS_Jd_END\n _OP_JNS_Jd_END:");
}

void OP_JP_Jd(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JP_Jd_END\n _OP_JP_Jd_END:");
}

void OP_JNP_Jd(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
		registers->r_eip += instrLength + immDword;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNP_Jd_END\n _OP_JNP_Jd_END:");
}

void OP_JL_Jd(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + immDword;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JL_Jd_END\n _OP_JL_Jd_END:");
}

void OP_JNL_Jd(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		registers->r_eip += instrLength + immDword;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNL_Jd_END\n _OP_JNL_Jd_END:");
}

void OP_JLE_Jd(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + immDword;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JLE_Jd_END\n _OP_JLE_Jd_END:");
}

void OP_JNLE_Jd(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + immDword;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JNLE_Jd_END\n _OP_JNLE_Jd_END:");
}

void OP_JCXZ_Jb_O16(void)
{
	if(registers->r_cx == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JCXZ_Jb_O16_END\n _OP_JCXZ_Jb_O16_END:");
}

void OP_JCXZ_Jb_O32(void)
{
	if(registers->r_cx == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JCXZ_Jb_O32_END\n _OP_JCXZ_Jb_O32_END:");
}

void OP_JECXZ_Jb_O16(void)
{
	if(registers->r_ecx == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JECXZ_Jb_O16_END\n _OP_JECXZ_Jb_O16_END:");
}

void OP_JECXZ_Jb_O32(void)
{
	if(registers->r_ecx == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_JECXZ_Jb_O32_END\n _OP_JECXZ_Jb_O32_END:");
}

void OP_LOOPNZ_Jb_O16(void)
{
	if(--registers->r_cx != 0 && (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOPNZ_Jb_O16_END\n _OP_LOOPNZ_Jb_O16_END:");
}

void OP_LOOPNZ_Jb_O32(void)
{
	if(--registers->r_cx != 0 && (registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOPNZ_Jb_O32_END\n _OP_LOOPNZ_Jb_O32_END:");
}


void OP_LOOPZ_Jb_O16(void)
{
	if(--registers->r_cx != 0 && (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOPZ_Jb_O16_END\n _OP_LOOPZ_Jb_O16_END:");
}

void OP_LOOPZ_Jb_O32(void)
{
	if(--registers->r_cx != 0 && (registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOPZ_Jb_O32_END\n _OP_LOOPZ_Jb_O32_END:");
}

void OP_LOOP_Jb_O16(void)
{
	if(--registers->r_cx != 0)
	{
		registers->r_eip += instrLength + (SignedByte)immByte;
		registers->r_eip &= 0xffff;
	}
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOP_Jb_O16_END\n _OP_LOOP_Jb_O16_END:");
}

void OP_LOOP_Jb_O32(void)
{
	if(--registers->r_cx != 0)
		registers->r_eip += instrLength + (SignedByte)immByte;
	else
		registers->r_eip += instrLength;

	asm (".global _OP_LOOP_Jb_O32_END\n _OP_LOOP_Jb_O32_END:");
}

void OP_CALL_Jw(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteWord(registers->r_ss.base + registers->r_esp - 2, registers->r_ip + instrLength);
		registers->r_esp -= 2;
	}
	else
	{
		WriteWord(registers->r_ss.base + (Word)(registers->r_sp - 2), registers->r_ip + instrLength);
		registers->r_sp -= 2;
	}

	registers->r_eip += instrLength + immWord;
	registers->r_eip &= 0xffff;

	asm (".global _OP_CALL_Jw_END\n _OP_CALL_Jw_END:");
}

void OP_CALL_Jd(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		WriteDword(registers->r_ss.base + registers->r_esp - 4, registers->r_eip + instrLength);
		registers->r_esp -= 4;
	}
	else
	{
		WriteDword(registers->r_ss.base + (Word)(registers->r_sp - 4), registers->r_eip + instrLength);
		registers->r_sp -= 4;
	}

	registers->r_eip += instrLength + immDword;
	
	asm (".global _OP_CALL_Jd_END\n _OP_CALL_Jd_END:");
}

void OP_RET_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_eip = ReadWord(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 2;
	}
	else
	{
		registers->r_eip = ReadWord(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 2;
	}

	asm (".global _OP_RET_O16_END\n _OP_RET_O16_END:");
}

void OP_RET_Iw_O16(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_eip = ReadWord(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 2 + immWord;
	}
	else
	{
		registers->r_eip = ReadWord(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 2 + immWord;
	}

	asm (".global _OP_RET_Iw_O16_END\n _OP_RET_Iw_O16_END:");
}

void OP_RET_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_eip = ReadDword(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 4;
	}
	else
	{
		registers->r_eip = ReadDword(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 4;
	}

	asm (".global _OP_RET_O32_END\n _OP_RET_O32_END:");
}

void OP_RET_Iw_O32(void)
{
	if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
	{
		registers->r_eip = ReadDword(registers->r_ss.base + registers->r_esp);
		registers->r_esp += 4 + immWord;
	}
	else
	{
		registers->r_eip = ReadDword(registers->r_ss.base + registers->r_sp);
		registers->r_sp += 4 + immWord;
	}

	asm (".global _OP_RET_Iw_O32_END\n _OP_RET_Iw_O32_END:");
}
