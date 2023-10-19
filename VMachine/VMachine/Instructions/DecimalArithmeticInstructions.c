//	VMachine
//	Implementations of decimal arithmetic instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define DECIMALOP(op)			\
	asm volatile				\
	(							\
		#op"			\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
	);

void OP_DAA(void) { DECIMALOP(daa) asm (".global _OP_DAA_END\n _OP_DAA_END:"); }
void OP_DAS(void) { DECIMALOP(das) asm (".global _OP_DAS_END\n _OP_DAS_END:"); }
void OP_AAA(void) { DECIMALOP(aaa) asm (".global _OP_AAA_END\n _OP_AAA_END:"); }
void OP_AAS(void) { DECIMALOP(aas) asm (".global _OP_AAS_END\n _OP_AAS_END:"); }
