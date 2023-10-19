//	VMachine
//	Helper functions called from translated code
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#ifndef HELPER_FUNCTIONS_H
#define HELPER_FUNCTIONS_H

typedef unsigned char Byte;
typedef unsigned short Word;
typedef unsigned long Dword;
typedef unsigned long long Qword;
typedef struct { Qword low, high; } __attribute__((packed)) DQword;

typedef signed char SignedByte;
typedef signed short SignedWord;
typedef signed long SignedDword;
typedef signed long long SignedQword;

typedef Dword BOOL;
#define FALSE 0
#define TRUE 1

#define LIKELY(x)	__builtin_expect(!!(x), TRUE)
#define UNLIKELY(x)	__builtin_expect((x), FALSE)

#include "../x86Constants.h"

//Include the Registers structure definition
#include "../Registers.h"

//Translated code is run with ebp pointing to the guest's registers
register struct Registers * registers asm ("ebp");

//Return immediately from the translated code
static inline void ReturnImmediately(void)
{
	asm volatile
	(
		"movl "REGISTERS_RETURN_IMMEDIATELY_ESP_OFFSET_STRING"(%ebp), %esp		\n"
		"ret		\n"
	);
}

#endif
