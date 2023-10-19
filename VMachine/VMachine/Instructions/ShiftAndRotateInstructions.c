//	VMachine
//	Implementations of shift and rotate instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define SHIFTOP_Rb_1(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "1" (Rb)	\
	);

#define SHIFTOP_Rw_1(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "1" (Rw)	\
	);

#define SHIFTOP_Rd_1(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "1" (Rd)	\
	);

#define SHIFTOP_Mb_1_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_1_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_1_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTOP_Mb_1_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_1_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_1_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l $1, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTOP_Rb_CL(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (Rb)	\
	);

#define SHIFTOP_Rw_CL(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (Rw)	\
	);

#define SHIFTOP_Rd_CL(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (Rd)	\
	);

#define SHIFTOP_Mb_CL_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_CL_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_CL_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTOP_Mb_CL_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_CL_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_CL_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (registers->r_cl), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTOP_Rb_Ib(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (Rb)	\
	);

#define SHIFTOP_Rw_Ib(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (Rw)	\
	);

#define SHIFTOP_Rd_Ib(op)		\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (Rd)	\
	);

#define SHIFTOP_Mb_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTOP_Mb_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteByte(address, dest);

#define SHIFTOP_Mw_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTOP_Md_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "c" (immByte), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTDOP_Rw_Gw_CL(op)	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (Rw)	\
	);

#define SHIFTDOP_Mw_Gw_CL_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTDOP_Mw_Gw_CL_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTDOP_Rw_Gw_Ib(op)	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (Rw)	\
	);

#define SHIFTDOP_Mw_Gw_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTDOP_Mw_Gw_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gw), "c" (count), "1" (dest)	\
	);							\
	WriteWord(address, dest);

#define SHIFTDOP_Rd_Gd_CL(op)	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (Rd)	\
	);

#define SHIFTDOP_Md_Gd_CL_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTDOP_Md_Gd_CL_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	Byte count = registers->r_cl & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTDOP_Rd_Gd_Ib(op)	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (Rd)	\
	);

#define SHIFTDOP_Md_Gd_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define SHIFTDOP_Md_Gd_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	Byte count = immByte & 0x1f;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %4, %3, %1	\n"	\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (Gd), "c" (count), "1" (dest)	\
	);							\
	WriteDword(address, dest);

#define BSWAP_Regd(reg)			\
	asm volatile				\
	(							\
		"bswapl %0		\n"		\
		: "=r" (registers->r_##reg)	\
		: "0" (registers->r_##reg)	\
	);	

void OP_ROL_Rb_1(void) { SHIFTOP_Rb_1(rol) asm (".global _OP_ROL_Rb_1_END\n _OP_ROL_Rb_1_END:"); }
void OP_ROR_Rb_1(void) { SHIFTOP_Rb_1(ror) asm (".global _OP_ROR_Rb_1_END\n _OP_ROR_Rb_1_END:"); }
void OP_RCL_Rb_1(void) { SHIFTOP_Rb_1(rcl) asm (".global _OP_RCL_Rb_1_END\n _OP_RCL_Rb_1_END:"); }
void OP_RCR_Rb_1(void) { SHIFTOP_Rb_1(rcr) asm (".global _OP_RCR_Rb_1_END\n _OP_RCR_Rb_1_END:"); }
void OP_SHL_Rb_1(void) { SHIFTOP_Rb_1(shl) asm (".global _OP_SHL_Rb_1_END\n _OP_SHL_Rb_1_END:"); }
void OP_SHR_Rb_1(void) { SHIFTOP_Rb_1(shr) asm (".global _OP_SHR_Rb_1_END\n _OP_SHR_Rb_1_END:"); }
void OP_SAR_Rb_1(void) { SHIFTOP_Rb_1(sar) asm (".global _OP_SAR_Rb_1_END\n _OP_SAR_Rb_1_END:"); }

void OP_ROL_Rw_1(void) { SHIFTOP_Rw_1(rol) asm (".global _OP_ROL_Rw_1_END\n _OP_ROL_Rw_1_END:"); }
void OP_ROR_Rw_1(void) { SHIFTOP_Rw_1(ror) asm (".global _OP_ROR_Rw_1_END\n _OP_ROR_Rw_1_END:"); }
void OP_RCL_Rw_1(void) { SHIFTOP_Rw_1(rcl) asm (".global _OP_RCL_Rw_1_END\n _OP_RCL_Rw_1_END:"); }
void OP_RCR_Rw_1(void) { SHIFTOP_Rw_1(rcr) asm (".global _OP_RCR_Rw_1_END\n _OP_RCR_Rw_1_END:"); }
void OP_SHL_Rw_1(void) { SHIFTOP_Rw_1(shl) asm (".global _OP_SHL_Rw_1_END\n _OP_SHL_Rw_1_END:"); }
void OP_SHR_Rw_1(void) { SHIFTOP_Rw_1(shr) asm (".global _OP_SHR_Rw_1_END\n _OP_SHR_Rw_1_END:"); }
void OP_SAR_Rw_1(void) { SHIFTOP_Rw_1(sar) asm (".global _OP_SAR_Rw_1_END\n _OP_SAR_Rw_1_END:"); }

void OP_ROL_Rd_1(void) { SHIFTOP_Rd_1(rol) asm (".global _OP_ROL_Rd_1_END\n _OP_ROL_Rd_1_END:"); }
void OP_ROR_Rd_1(void) { SHIFTOP_Rd_1(ror) asm (".global _OP_ROR_Rd_1_END\n _OP_ROR_Rd_1_END:"); }
void OP_RCL_Rd_1(void) { SHIFTOP_Rd_1(rcl) asm (".global _OP_RCL_Rd_1_END\n _OP_RCL_Rd_1_END:"); }
void OP_RCR_Rd_1(void) { SHIFTOP_Rd_1(rcr) asm (".global _OP_RCR_Rd_1_END\n _OP_RCR_Rd_1_END:"); }
void OP_SHL_Rd_1(void) { SHIFTOP_Rd_1(shl) asm (".global _OP_SHL_Rd_1_END\n _OP_SHL_Rd_1_END:"); }
void OP_SHR_Rd_1(void) { SHIFTOP_Rd_1(shr) asm (".global _OP_SHR_Rd_1_END\n _OP_SHR_Rd_1_END:"); }
void OP_SAR_Rd_1(void) { SHIFTOP_Rd_1(sar) asm (".global _OP_SAR_Rd_1_END\n _OP_SAR_Rd_1_END:"); }

void OP_ROL_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(rol) asm (".global _OP_ROL_Mb_1_A16_END\n _OP_ROL_Mb_1_A16_END:"); }
void OP_ROR_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(ror) asm (".global _OP_ROR_Mb_1_A16_END\n _OP_ROR_Mb_1_A16_END:"); }
void OP_RCL_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(rcl) asm (".global _OP_RCL_Mb_1_A16_END\n _OP_RCL_Mb_1_A16_END:"); }
void OP_RCR_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(rcr) asm (".global _OP_RCR_Mb_1_A16_END\n _OP_RCR_Mb_1_A16_END:"); }
void OP_SHL_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(shl) asm (".global _OP_SHL_Mb_1_A16_END\n _OP_SHL_Mb_1_A16_END:"); }
void OP_SHR_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(shr) asm (".global _OP_SHR_Mb_1_A16_END\n _OP_SHR_Mb_1_A16_END:"); }
void OP_SAR_Mb_1_A16(void) { SHIFTOP_Mb_1_A16(sar) asm (".global _OP_SAR_Mb_1_A16_END\n _OP_SAR_Mb_1_A16_END:"); }

void OP_ROL_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(rol) asm (".global _OP_ROL_Mw_1_A16_END\n _OP_ROL_Mw_1_A16_END:"); }
void OP_ROR_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(ror) asm (".global _OP_ROR_Mw_1_A16_END\n _OP_ROR_Mw_1_A16_END:"); }
void OP_RCL_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(rcl) asm (".global _OP_RCL_Mw_1_A16_END\n _OP_RCL_Mw_1_A16_END:"); }
void OP_RCR_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(rcr) asm (".global _OP_RCR_Mw_1_A16_END\n _OP_RCR_Mw_1_A16_END:"); }
void OP_SHL_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(shl) asm (".global _OP_SHL_Mw_1_A16_END\n _OP_SHL_Mw_1_A16_END:"); }
void OP_SHR_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(shr) asm (".global _OP_SHR_Mw_1_A16_END\n _OP_SHR_Mw_1_A16_END:"); }
void OP_SAR_Mw_1_A16(void) { SHIFTOP_Mw_1_A16(sar) asm (".global _OP_SAR_Mw_1_A16_END\n _OP_SAR_Mw_1_A16_END:"); }

void OP_ROL_Md_1_A16(void) { SHIFTOP_Md_1_A16(rol) asm (".global _OP_ROL_Md_1_A16_END\n _OP_ROL_Md_1_A16_END:"); }
void OP_ROR_Md_1_A16(void) { SHIFTOP_Md_1_A16(ror) asm (".global _OP_ROR_Md_1_A16_END\n _OP_ROR_Md_1_A16_END:"); }
void OP_RCL_Md_1_A16(void) { SHIFTOP_Md_1_A16(rcl) asm (".global _OP_RCL_Md_1_A16_END\n _OP_RCL_Md_1_A16_END:"); }
void OP_RCR_Md_1_A16(void) { SHIFTOP_Md_1_A16(rcr) asm (".global _OP_RCR_Md_1_A16_END\n _OP_RCR_Md_1_A16_END:"); }
void OP_SHL_Md_1_A16(void) { SHIFTOP_Md_1_A16(shl) asm (".global _OP_SHL_Md_1_A16_END\n _OP_SHL_Md_1_A16_END:"); }
void OP_SHR_Md_1_A16(void) { SHIFTOP_Md_1_A16(shr) asm (".global _OP_SHR_Md_1_A16_END\n _OP_SHR_Md_1_A16_END:"); }
void OP_SAR_Md_1_A16(void) { SHIFTOP_Md_1_A16(sar) asm (".global _OP_SAR_Md_1_A16_END\n _OP_SAR_Md_1_A16_END:"); }

void OP_ROL_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(rol) asm (".global _OP_ROL_Mb_1_A32_END\n _OP_ROL_Mb_1_A32_END:"); }
void OP_ROR_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(ror) asm (".global _OP_ROR_Mb_1_A32_END\n _OP_ROR_Mb_1_A32_END:"); }
void OP_RCL_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(rcl) asm (".global _OP_RCL_Mb_1_A32_END\n _OP_RCL_Mb_1_A32_END:"); }
void OP_RCR_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(rcr) asm (".global _OP_RCR_Mb_1_A32_END\n _OP_RCR_Mb_1_A32_END:"); }
void OP_SHL_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(shl) asm (".global _OP_SHL_Mb_1_A32_END\n _OP_SHL_Mb_1_A32_END:"); }
void OP_SHR_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(shr) asm (".global _OP_SHR_Mb_1_A32_END\n _OP_SHR_Mb_1_A32_END:"); }
void OP_SAR_Mb_1_A32(void) { SHIFTOP_Mb_1_A32(sar) asm (".global _OP_SAR_Mb_1_A32_END\n _OP_SAR_Mb_1_A32_END:"); }

void OP_ROL_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(rol) asm (".global _OP_ROL_Mw_1_A32_END\n _OP_ROL_Mw_1_A32_END:"); }
void OP_ROR_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(ror) asm (".global _OP_ROR_Mw_1_A32_END\n _OP_ROR_Mw_1_A32_END:"); }
void OP_RCL_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(rcl) asm (".global _OP_RCL_Mw_1_A32_END\n _OP_RCL_Mw_1_A32_END:"); }
void OP_RCR_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(rcr) asm (".global _OP_RCR_Mw_1_A32_END\n _OP_RCR_Mw_1_A32_END:"); }
void OP_SHL_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(shl) asm (".global _OP_SHL_Mw_1_A32_END\n _OP_SHL_Mw_1_A32_END:"); }
void OP_SHR_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(shr) asm (".global _OP_SHR_Mw_1_A32_END\n _OP_SHR_Mw_1_A32_END:"); }
void OP_SAR_Mw_1_A32(void) { SHIFTOP_Mw_1_A32(sar) asm (".global _OP_SAR_Mw_1_A32_END\n _OP_SAR_Mw_1_A32_END:"); }

void OP_ROL_Md_1_A32(void) { SHIFTOP_Md_1_A32(rol) asm (".global _OP_ROL_Md_1_A32_END\n _OP_ROL_Md_1_A32_END:"); }
void OP_ROR_Md_1_A32(void) { SHIFTOP_Md_1_A32(ror) asm (".global _OP_ROR_Md_1_A32_END\n _OP_ROR_Md_1_A32_END:"); }
void OP_RCL_Md_1_A32(void) { SHIFTOP_Md_1_A32(rcl) asm (".global _OP_RCL_Md_1_A32_END\n _OP_RCL_Md_1_A32_END:"); }
void OP_RCR_Md_1_A32(void) { SHIFTOP_Md_1_A32(rcr) asm (".global _OP_RCR_Md_1_A32_END\n _OP_RCR_Md_1_A32_END:"); }
void OP_SHL_Md_1_A32(void) { SHIFTOP_Md_1_A32(shl) asm (".global _OP_SHL_Md_1_A32_END\n _OP_SHL_Md_1_A32_END:"); }
void OP_SHR_Md_1_A32(void) { SHIFTOP_Md_1_A32(shr) asm (".global _OP_SHR_Md_1_A32_END\n _OP_SHR_Md_1_A32_END:"); }
void OP_SAR_Md_1_A32(void) { SHIFTOP_Md_1_A32(sar) asm (".global _OP_SAR_Md_1_A32_END\n _OP_SAR_Md_1_A32_END:"); }

void OP_ROL_Rb_CL(void) { SHIFTOP_Rb_CL(rol) asm (".global _OP_ROL_Rb_CL_END\n _OP_ROL_Rb_CL_END:"); }
void OP_ROR_Rb_CL(void) { SHIFTOP_Rb_CL(ror) asm (".global _OP_ROR_Rb_CL_END\n _OP_ROR_Rb_CL_END:"); }
void OP_RCL_Rb_CL(void) { SHIFTOP_Rb_CL(rcl) asm (".global _OP_RCL_Rb_CL_END\n _OP_RCL_Rb_CL_END:"); }
void OP_RCR_Rb_CL(void) { SHIFTOP_Rb_CL(rcr) asm (".global _OP_RCR_Rb_CL_END\n _OP_RCR_Rb_CL_END:"); }
void OP_SHL_Rb_CL(void) { SHIFTOP_Rb_CL(shl) asm (".global _OP_SHL_Rb_CL_END\n _OP_SHL_Rb_CL_END:"); }
void OP_SHR_Rb_CL(void) { SHIFTOP_Rb_CL(shr) asm (".global _OP_SHR_Rb_CL_END\n _OP_SHR_Rb_CL_END:"); }
void OP_SAR_Rb_CL(void) { SHIFTOP_Rb_CL(sar) asm (".global _OP_SAR_Rb_CL_END\n _OP_SAR_Rb_CL_END:"); }

void OP_ROL_Rw_CL(void) { SHIFTOP_Rw_CL(rol) asm (".global _OP_ROL_Rw_CL_END\n _OP_ROL_Rw_CL_END:"); }
void OP_ROR_Rw_CL(void) { SHIFTOP_Rw_CL(ror) asm (".global _OP_ROR_Rw_CL_END\n _OP_ROR_Rw_CL_END:"); }
void OP_RCL_Rw_CL(void) { SHIFTOP_Rw_CL(rcl) asm (".global _OP_RCL_Rw_CL_END\n _OP_RCL_Rw_CL_END:"); }
void OP_RCR_Rw_CL(void) { SHIFTOP_Rw_CL(rcr) asm (".global _OP_RCR_Rw_CL_END\n _OP_RCR_Rw_CL_END:"); }
void OP_SHL_Rw_CL(void) { SHIFTOP_Rw_CL(shl) asm (".global _OP_SHL_Rw_CL_END\n _OP_SHL_Rw_CL_END:"); }
void OP_SHR_Rw_CL(void) { SHIFTOP_Rw_CL(shr) asm (".global _OP_SHR_Rw_CL_END\n _OP_SHR_Rw_CL_END:"); }
void OP_SAR_Rw_CL(void) { SHIFTOP_Rw_CL(sar) asm (".global _OP_SAR_Rw_CL_END\n _OP_SAR_Rw_CL_END:"); }

void OP_ROL_Rd_CL(void) { SHIFTOP_Rd_CL(rol) asm (".global _OP_ROL_Rd_CL_END\n _OP_ROL_Rd_CL_END:"); }
void OP_ROR_Rd_CL(void) { SHIFTOP_Rd_CL(ror) asm (".global _OP_ROR_Rd_CL_END\n _OP_ROR_Rd_CL_END:"); }
void OP_RCL_Rd_CL(void) { SHIFTOP_Rd_CL(rcl) asm (".global _OP_RCL_Rd_CL_END\n _OP_RCL_Rd_CL_END:"); }
void OP_RCR_Rd_CL(void) { SHIFTOP_Rd_CL(rcr) asm (".global _OP_RCR_Rd_CL_END\n _OP_RCR_Rd_CL_END:"); }
void OP_SHL_Rd_CL(void) { SHIFTOP_Rd_CL(shl) asm (".global _OP_SHL_Rd_CL_END\n _OP_SHL_Rd_CL_END:"); }
void OP_SHR_Rd_CL(void) { SHIFTOP_Rd_CL(shr) asm (".global _OP_SHR_Rd_CL_END\n _OP_SHR_Rd_CL_END:"); }
void OP_SAR_Rd_CL(void) { SHIFTOP_Rd_CL(sar) asm (".global _OP_SAR_Rd_CL_END\n _OP_SAR_Rd_CL_END:"); }

void OP_ROL_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(rol) asm (".global _OP_ROL_Mb_CL_A16_END\n _OP_ROL_Mb_CL_A16_END:"); }
void OP_ROR_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(ror) asm (".global _OP_ROR_Mb_CL_A16_END\n _OP_ROR_Mb_CL_A16_END:"); }
void OP_RCL_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(rcl) asm (".global _OP_RCL_Mb_CL_A16_END\n _OP_RCL_Mb_CL_A16_END:"); }
void OP_RCR_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(rcr) asm (".global _OP_RCR_Mb_CL_A16_END\n _OP_RCR_Mb_CL_A16_END:"); }
void OP_SHL_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(shl) asm (".global _OP_SHL_Mb_CL_A16_END\n _OP_SHL_Mb_CL_A16_END:"); }
void OP_SHR_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(shr) asm (".global _OP_SHR_Mb_CL_A16_END\n _OP_SHR_Mb_CL_A16_END:"); }
void OP_SAR_Mb_CL_A16(void) { SHIFTOP_Mb_CL_A16(sar) asm (".global _OP_SAR_Mb_CL_A16_END\n _OP_SAR_Mb_CL_A16_END:"); }

void OP_ROL_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(rol) asm (".global _OP_ROL_Mw_CL_A16_END\n _OP_ROL_Mw_CL_A16_END:"); }
void OP_ROR_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(ror) asm (".global _OP_ROR_Mw_CL_A16_END\n _OP_ROR_Mw_CL_A16_END:"); }
void OP_RCL_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(rcl) asm (".global _OP_RCL_Mw_CL_A16_END\n _OP_RCL_Mw_CL_A16_END:"); }
void OP_RCR_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(rcr) asm (".global _OP_RCR_Mw_CL_A16_END\n _OP_RCR_Mw_CL_A16_END:"); }
void OP_SHL_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(shl) asm (".global _OP_SHL_Mw_CL_A16_END\n _OP_SHL_Mw_CL_A16_END:"); }
void OP_SHR_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(shr) asm (".global _OP_SHR_Mw_CL_A16_END\n _OP_SHR_Mw_CL_A16_END:"); }
void OP_SAR_Mw_CL_A16(void) { SHIFTOP_Mw_CL_A16(sar) asm (".global _OP_SAR_Mw_CL_A16_END\n _OP_SAR_Mw_CL_A16_END:"); }

void OP_ROL_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(rol) asm (".global _OP_ROL_Md_CL_A16_END\n _OP_ROL_Md_CL_A16_END:"); }
void OP_ROR_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(ror) asm (".global _OP_ROR_Md_CL_A16_END\n _OP_ROR_Md_CL_A16_END:"); }
void OP_RCL_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(rcl) asm (".global _OP_RCL_Md_CL_A16_END\n _OP_RCL_Md_CL_A16_END:"); }
void OP_RCR_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(rcr) asm (".global _OP_RCR_Md_CL_A16_END\n _OP_RCR_Md_CL_A16_END:"); }
void OP_SHL_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(shl) asm (".global _OP_SHL_Md_CL_A16_END\n _OP_SHL_Md_CL_A16_END:"); }
void OP_SHR_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(shr) asm (".global _OP_SHR_Md_CL_A16_END\n _OP_SHR_Md_CL_A16_END:"); }
void OP_SAR_Md_CL_A16(void) { SHIFTOP_Md_CL_A16(sar) asm (".global _OP_SAR_Md_CL_A16_END\n _OP_SAR_Md_CL_A16_END:"); }

void OP_ROL_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(rol) asm (".global _OP_ROL_Mb_CL_A32_END\n _OP_ROL_Mb_CL_A32_END:"); }
void OP_ROR_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(ror) asm (".global _OP_ROR_Mb_CL_A32_END\n _OP_ROR_Mb_CL_A32_END:"); }
void OP_RCL_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(rcl) asm (".global _OP_RCL_Mb_CL_A32_END\n _OP_RCL_Mb_CL_A32_END:"); }
void OP_RCR_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(rcr) asm (".global _OP_RCR_Mb_CL_A32_END\n _OP_RCR_Mb_CL_A32_END:"); }
void OP_SHL_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(shl) asm (".global _OP_SHL_Mb_CL_A32_END\n _OP_SHL_Mb_CL_A32_END:"); }
void OP_SHR_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(shr) asm (".global _OP_SHR_Mb_CL_A32_END\n _OP_SHR_Mb_CL_A32_END:"); }
void OP_SAR_Mb_CL_A32(void) { SHIFTOP_Mb_CL_A32(sar) asm (".global _OP_SAR_Mb_CL_A32_END\n _OP_SAR_Mb_CL_A32_END:"); }

void OP_ROL_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(rol) asm (".global _OP_ROL_Mw_CL_A32_END\n _OP_ROL_Mw_CL_A32_END:"); }
void OP_ROR_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(ror) asm (".global _OP_ROR_Mw_CL_A32_END\n _OP_ROR_Mw_CL_A32_END:"); }
void OP_RCL_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(rcl) asm (".global _OP_RCL_Mw_CL_A32_END\n _OP_RCL_Mw_CL_A32_END:"); }
void OP_RCR_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(rcr) asm (".global _OP_RCR_Mw_CL_A32_END\n _OP_RCR_Mw_CL_A32_END:"); }
void OP_SHL_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(shl) asm (".global _OP_SHL_Mw_CL_A32_END\n _OP_SHL_Mw_CL_A32_END:"); }
void OP_SHR_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(shr) asm (".global _OP_SHR_Mw_CL_A32_END\n _OP_SHR_Mw_CL_A32_END:"); }
void OP_SAR_Mw_CL_A32(void) { SHIFTOP_Mw_CL_A32(sar) asm (".global _OP_SAR_Mw_CL_A32_END\n _OP_SAR_Mw_CL_A32_END:"); }

void OP_ROL_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(rol) asm (".global _OP_ROL_Md_CL_A32_END\n _OP_ROL_Md_CL_A32_END:"); }
void OP_ROR_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(ror) asm (".global _OP_ROR_Md_CL_A32_END\n _OP_ROR_Md_CL_A32_END:"); }
void OP_RCL_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(rcl) asm (".global _OP_RCL_Md_CL_A32_END\n _OP_RCL_Md_CL_A32_END:"); }
void OP_RCR_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(rcr) asm (".global _OP_RCR_Md_CL_A32_END\n _OP_RCR_Md_CL_A32_END:"); }
void OP_SHL_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(shl) asm (".global _OP_SHL_Md_CL_A32_END\n _OP_SHL_Md_CL_A32_END:"); }
void OP_SHR_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(shr) asm (".global _OP_SHR_Md_CL_A32_END\n _OP_SHR_Md_CL_A32_END:"); }
void OP_SAR_Md_CL_A32(void) { SHIFTOP_Md_CL_A32(sar) asm (".global _OP_SAR_Md_CL_A32_END\n _OP_SAR_Md_CL_A32_END:"); }

void OP_ROL_Rb_Ib(void) { SHIFTOP_Rb_Ib(rol) asm (".global _OP_ROL_Rb_Ib_END\n _OP_ROL_Rb_Ib_END:"); }
void OP_ROR_Rb_Ib(void) { SHIFTOP_Rb_Ib(ror) asm (".global _OP_ROR_Rb_Ib_END\n _OP_ROR_Rb_Ib_END:"); }
void OP_RCL_Rb_Ib(void) { SHIFTOP_Rb_Ib(rcl) asm (".global _OP_RCL_Rb_Ib_END\n _OP_RCL_Rb_Ib_END:"); }
void OP_RCR_Rb_Ib(void) { SHIFTOP_Rb_Ib(rcr) asm (".global _OP_RCR_Rb_Ib_END\n _OP_RCR_Rb_Ib_END:"); }
void OP_SHL_Rb_Ib(void) { SHIFTOP_Rb_Ib(shl) asm (".global _OP_SHL_Rb_Ib_END\n _OP_SHL_Rb_Ib_END:"); }
void OP_SHR_Rb_Ib(void) { SHIFTOP_Rb_Ib(shr) asm (".global _OP_SHR_Rb_Ib_END\n _OP_SHR_Rb_Ib_END:"); }
void OP_SAR_Rb_Ib(void) { SHIFTOP_Rb_Ib(sar) asm (".global _OP_SAR_Rb_Ib_END\n _OP_SAR_Rb_Ib_END:"); }

void OP_ROL_Rw_Ib(void) { SHIFTOP_Rw_Ib(rol) asm (".global _OP_ROL_Rw_Ib_END\n _OP_ROL_Rw_Ib_END:"); }
void OP_ROR_Rw_Ib(void) { SHIFTOP_Rw_Ib(ror) asm (".global _OP_ROR_Rw_Ib_END\n _OP_ROR_Rw_Ib_END:"); }
void OP_RCL_Rw_Ib(void) { SHIFTOP_Rw_Ib(rcl) asm (".global _OP_RCL_Rw_Ib_END\n _OP_RCL_Rw_Ib_END:"); }
void OP_RCR_Rw_Ib(void) { SHIFTOP_Rw_Ib(rcr) asm (".global _OP_RCR_Rw_Ib_END\n _OP_RCR_Rw_Ib_END:"); }
void OP_SHL_Rw_Ib(void) { SHIFTOP_Rw_Ib(shl) asm (".global _OP_SHL_Rw_Ib_END\n _OP_SHL_Rw_Ib_END:"); }
void OP_SHR_Rw_Ib(void) { SHIFTOP_Rw_Ib(shr) asm (".global _OP_SHR_Rw_Ib_END\n _OP_SHR_Rw_Ib_END:"); }
void OP_SAR_Rw_Ib(void) { SHIFTOP_Rw_Ib(sar) asm (".global _OP_SAR_Rw_Ib_END\n _OP_SAR_Rw_Ib_END:"); }

void OP_ROL_Rd_Ib(void) { SHIFTOP_Rd_Ib(rol) asm (".global _OP_ROL_Rd_Ib_END\n _OP_ROL_Rd_Ib_END:"); }
void OP_ROR_Rd_Ib(void) { SHIFTOP_Rd_Ib(ror) asm (".global _OP_ROR_Rd_Ib_END\n _OP_ROR_Rd_Ib_END:"); }
void OP_RCL_Rd_Ib(void) { SHIFTOP_Rd_Ib(rcl) asm (".global _OP_RCL_Rd_Ib_END\n _OP_RCL_Rd_Ib_END:"); }
void OP_RCR_Rd_Ib(void) { SHIFTOP_Rd_Ib(rcr) asm (".global _OP_RCR_Rd_Ib_END\n _OP_RCR_Rd_Ib_END:"); }
void OP_SHL_Rd_Ib(void) { SHIFTOP_Rd_Ib(shl) asm (".global _OP_SHL_Rd_Ib_END\n _OP_SHL_Rd_Ib_END:"); }
void OP_SHR_Rd_Ib(void) { SHIFTOP_Rd_Ib(shr) asm (".global _OP_SHR_Rd_Ib_END\n _OP_SHR_Rd_Ib_END:"); }
void OP_SAR_Rd_Ib(void) { SHIFTOP_Rd_Ib(sar) asm (".global _OP_SAR_Rd_Ib_END\n _OP_SAR_Rd_Ib_END:"); }

void OP_ROL_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(rol) asm (".global _OP_ROL_Mb_Ib_A16_END\n _OP_ROL_Mb_Ib_A16_END:"); }
void OP_ROR_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(ror) asm (".global _OP_ROR_Mb_Ib_A16_END\n _OP_ROR_Mb_Ib_A16_END:"); }
void OP_RCL_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(rcl) asm (".global _OP_RCL_Mb_Ib_A16_END\n _OP_RCL_Mb_Ib_A16_END:"); }
void OP_RCR_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(rcr) asm (".global _OP_RCR_Mb_Ib_A16_END\n _OP_RCR_Mb_Ib_A16_END:"); }
void OP_SHL_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(shl) asm (".global _OP_SHL_Mb_Ib_A16_END\n _OP_SHL_Mb_Ib_A16_END:"); }
void OP_SHR_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(shr) asm (".global _OP_SHR_Mb_Ib_A16_END\n _OP_SHR_Mb_Ib_A16_END:"); }
void OP_SAR_Mb_Ib_A16(void) { SHIFTOP_Mb_Ib_A16(sar) asm (".global _OP_SAR_Mb_Ib_A16_END\n _OP_SAR_Mb_Ib_A16_END:"); }

void OP_ROL_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(rol) asm (".global _OP_ROL_Mw_Ib_A16_END\n _OP_ROL_Mw_Ib_A16_END:"); }
void OP_ROR_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(ror) asm (".global _OP_ROR_Mw_Ib_A16_END\n _OP_ROR_Mw_Ib_A16_END:"); }
void OP_RCL_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(rcl) asm (".global _OP_RCL_Mw_Ib_A16_END\n _OP_RCL_Mw_Ib_A16_END:"); }
void OP_RCR_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(rcr) asm (".global _OP_RCR_Mw_Ib_A16_END\n _OP_RCR_Mw_Ib_A16_END:"); }
void OP_SHL_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(shl) asm (".global _OP_SHL_Mw_Ib_A16_END\n _OP_SHL_Mw_Ib_A16_END:"); }
void OP_SHR_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(shr) asm (".global _OP_SHR_Mw_Ib_A16_END\n _OP_SHR_Mw_Ib_A16_END:"); }
void OP_SAR_Mw_Ib_A16(void) { SHIFTOP_Mw_Ib_A16(sar) asm (".global _OP_SAR_Mw_Ib_A16_END\n _OP_SAR_Mw_Ib_A16_END:"); }

void OP_ROL_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(rol) asm (".global _OP_ROL_Md_Ib_A16_END\n _OP_ROL_Md_Ib_A16_END:"); }
void OP_ROR_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(ror) asm (".global _OP_ROR_Md_Ib_A16_END\n _OP_ROR_Md_Ib_A16_END:"); }
void OP_RCL_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(rcl) asm (".global _OP_RCL_Md_Ib_A16_END\n _OP_RCL_Md_Ib_A16_END:"); }
void OP_RCR_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(rcr) asm (".global _OP_RCR_Md_Ib_A16_END\n _OP_RCR_Md_Ib_A16_END:"); }
void OP_SHL_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(shl) asm (".global _OP_SHL_Md_Ib_A16_END\n _OP_SHL_Md_Ib_A16_END:"); }
void OP_SHR_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(shr) asm (".global _OP_SHR_Md_Ib_A16_END\n _OP_SHR_Md_Ib_A16_END:"); }
void OP_SAR_Md_Ib_A16(void) { SHIFTOP_Md_Ib_A16(sar) asm (".global _OP_SAR_Md_Ib_A16_END\n _OP_SAR_Md_Ib_A16_END:"); }

void OP_ROL_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(rol) asm (".global _OP_ROL_Mb_Ib_A32_END\n _OP_ROL_Mb_Ib_A32_END:"); }
void OP_ROR_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(ror) asm (".global _OP_ROR_Mb_Ib_A32_END\n _OP_ROR_Mb_Ib_A32_END:"); }
void OP_RCL_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(rcl) asm (".global _OP_RCL_Mb_Ib_A32_END\n _OP_RCL_Mb_Ib_A32_END:"); }
void OP_RCR_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(rcr) asm (".global _OP_RCR_Mb_Ib_A32_END\n _OP_RCR_Mb_Ib_A32_END:"); }
void OP_SHL_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(shl) asm (".global _OP_SHL_Mb_Ib_A32_END\n _OP_SHL_Mb_Ib_A32_END:"); }
void OP_SHR_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(shr) asm (".global _OP_SHR_Mb_Ib_A32_END\n _OP_SHR_Mb_Ib_A32_END:"); }
void OP_SAR_Mb_Ib_A32(void) { SHIFTOP_Mb_Ib_A32(sar) asm (".global _OP_SAR_Mb_Ib_A32_END\n _OP_SAR_Mb_Ib_A32_END:"); }

void OP_ROL_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(rol) asm (".global _OP_ROL_Mw_Ib_A32_END\n _OP_ROL_Mw_Ib_A32_END:"); }
void OP_ROR_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(ror) asm (".global _OP_ROR_Mw_Ib_A32_END\n _OP_ROR_Mw_Ib_A32_END:"); }
void OP_RCL_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(rcl) asm (".global _OP_RCL_Mw_Ib_A32_END\n _OP_RCL_Mw_Ib_A32_END:"); }
void OP_RCR_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(rcr) asm (".global _OP_RCR_Mw_Ib_A32_END\n _OP_RCR_Mw_Ib_A32_END:"); }
void OP_SHL_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(shl) asm (".global _OP_SHL_Mw_Ib_A32_END\n _OP_SHL_Mw_Ib_A32_END:"); }
void OP_SHR_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(shr) asm (".global _OP_SHR_Mw_Ib_A32_END\n _OP_SHR_Mw_Ib_A32_END:"); }
void OP_SAR_Mw_Ib_A32(void) { SHIFTOP_Mw_Ib_A32(sar) asm (".global _OP_SAR_Mw_Ib_A32_END\n _OP_SAR_Mw_Ib_A32_END:"); }

void OP_ROL_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(rol) asm (".global _OP_ROL_Md_Ib_A32_END\n _OP_ROL_Md_Ib_A32_END:"); }
void OP_ROR_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(ror) asm (".global _OP_ROR_Md_Ib_A32_END\n _OP_ROR_Md_Ib_A32_END:"); }
void OP_RCL_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(rcl) asm (".global _OP_RCL_Md_Ib_A32_END\n _OP_RCL_Md_Ib_A32_END:"); }
void OP_RCR_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(rcr) asm (".global _OP_RCR_Md_Ib_A32_END\n _OP_RCR_Md_Ib_A32_END:"); }
void OP_SHL_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(shl) asm (".global _OP_SHL_Md_Ib_A32_END\n _OP_SHL_Md_Ib_A32_END:"); }
void OP_SHR_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(shr) asm (".global _OP_SHR_Md_Ib_A32_END\n _OP_SHR_Md_Ib_A32_END:"); }
void OP_SAR_Md_Ib_A32(void) { SHIFTOP_Md_Ib_A32(sar) asm (".global _OP_SAR_Md_Ib_A32_END\n _OP_SAR_Md_Ib_A32_END:"); }

void OP_SHLD_Rw_Gw_CL(void) { SHIFTDOP_Rw_Gw_CL(shld) asm (".global _OP_SHLD_Rw_Gw_CL_END\n _OP_SHLD_Rw_Gw_CL_END:"); }
void OP_SHRD_Rw_Gw_CL(void) { SHIFTDOP_Rw_Gw_CL(shrd) asm (".global _OP_SHRD_Rw_Gw_CL_END\n _OP_SHRD_Rw_Gw_CL_END:"); }

void OP_SHLD_Mw_Gw_CL_A16(void) { SHIFTDOP_Mw_Gw_CL_A16(shld) asm (".global _OP_SHLD_Mw_Gw_CL_A16_END\n _OP_SHLD_Mw_Gw_CL_A16_END:"); }
void OP_SHRD_Mw_Gw_CL_A16(void) { SHIFTDOP_Mw_Gw_CL_A16(shrd) asm (".global _OP_SHRD_Mw_Gw_CL_A16_END\n _OP_SHRD_Mw_Gw_CL_A16_END:"); }

void OP_SHLD_Mw_Gw_CL_A32(void) { SHIFTDOP_Mw_Gw_CL_A32(shld) asm (".global _OP_SHLD_Mw_Gw_CL_A32_END\n _OP_SHLD_Mw_Gw_CL_A32_END:"); }
void OP_SHRD_Mw_Gw_CL_A32(void) { SHIFTDOP_Mw_Gw_CL_A32(shrd) asm (".global _OP_SHRD_Mw_Gw_CL_A32_END\n _OP_SHRD_Mw_Gw_CL_A32_END:"); }

void OP_SHLD_Rw_Gw_Ib(void) { SHIFTDOP_Rw_Gw_Ib(shld) asm (".global _OP_SHLD_Rw_Gw_Ib_END\n _OP_SHLD_Rw_Gw_Ib_END:"); }
void OP_SHRD_Rw_Gw_Ib(void) { SHIFTDOP_Rw_Gw_Ib(shrd) asm (".global _OP_SHRD_Rw_Gw_Ib_END\n _OP_SHRD_Rw_Gw_Ib_END:"); }

void OP_SHLD_Mw_Gw_Ib_A16(void) { SHIFTDOP_Mw_Gw_Ib_A16(shld) asm (".global _OP_SHLD_Mw_Gw_Ib_A16_END\n _OP_SHLD_Mw_Gw_Ib_A16_END:"); }
void OP_SHRD_Mw_Gw_Ib_A16(void) { SHIFTDOP_Mw_Gw_Ib_A16(shrd) asm (".global _OP_SHRD_Mw_Gw_Ib_A16_END\n _OP_SHRD_Mw_Gw_Ib_A16_END:"); }

void OP_SHLD_Mw_Gw_Ib_A32(void) { SHIFTDOP_Mw_Gw_Ib_A32(shld) asm (".global _OP_SHLD_Mw_Gw_Ib_A32_END\n _OP_SHLD_Mw_Gw_Ib_A32_END:"); }
void OP_SHRD_Mw_Gw_Ib_A32(void) { SHIFTDOP_Mw_Gw_Ib_A32(shrd) asm (".global _OP_SHRD_Mw_Gw_Ib_A32_END\n _OP_SHRD_Mw_Gw_Ib_A32_END:"); }

void OP_SHLD_Rd_Gd_CL(void) { SHIFTDOP_Rd_Gd_CL(shld) asm (".global _OP_SHLD_Rd_Gd_CL_END\n _OP_SHLD_Rd_Gd_CL_END:"); }
void OP_SHRD_Rd_Gd_CL(void) { SHIFTDOP_Rd_Gd_CL(shrd) asm (".global _OP_SHRD_Rd_Gd_CL_END\n _OP_SHRD_Rd_Gd_CL_END:"); }

void OP_SHLD_Md_Gd_CL_A16(void) { SHIFTDOP_Md_Gd_CL_A16(shld) asm (".global _OP_SHLD_Md_Gd_CL_A16_END\n _OP_SHLD_Md_Gd_CL_A16_END:"); }
void OP_SHRD_Md_Gd_CL_A16(void) { SHIFTDOP_Md_Gd_CL_A16(shrd) asm (".global _OP_SHRD_Md_Gd_CL_A16_END\n _OP_SHRD_Md_Gd_CL_A16_END:"); }

void OP_SHLD_Md_Gd_CL_A32(void) { SHIFTDOP_Md_Gd_CL_A32(shld) asm (".global _OP_SHLD_Md_Gd_CL_A32_END\n _OP_SHLD_Md_Gd_CL_A32_END:"); }
void OP_SHRD_Md_Gd_CL_A32(void) { SHIFTDOP_Md_Gd_CL_A32(shrd) asm (".global _OP_SHRD_Md_Gd_CL_A32_END\n _OP_SHRD_Md_Gd_CL_A32_END:"); }

void OP_SHLD_Rd_Gd_Ib(void) { SHIFTDOP_Rd_Gd_Ib(shld) asm (".global _OP_SHLD_Rd_Gd_Ib_END\n _OP_SHLD_Rd_Gd_Ib_END:"); }
void OP_SHRD_Rd_Gd_Ib(void) { SHIFTDOP_Rd_Gd_Ib(shrd) asm (".global _OP_SHRD_Rd_Gd_Ib_END\n _OP_SHRD_Rd_Gd_Ib_END:"); }

void OP_SHLD_Md_Gd_Ib_A16(void) { SHIFTDOP_Md_Gd_Ib_A16(shld) asm (".global _OP_SHLD_Md_Gd_Ib_A16_END\n _OP_SHLD_Md_Gd_Ib_A16_END:"); }
void OP_SHRD_Md_Gd_Ib_A16(void) { SHIFTDOP_Md_Gd_Ib_A16(shrd) asm (".global _OP_SHRD_Md_Gd_Ib_A16_END\n _OP_SHRD_Md_Gd_Ib_A16_END:"); }

void OP_SHLD_Md_Gd_Ib_A32(void) { SHIFTDOP_Md_Gd_Ib_A32(shld) asm (".global _OP_SHLD_Md_Gd_Ib_A32_END\n _OP_SHLD_Md_Gd_Ib_A32_END:"); }
void OP_SHRD_Md_Gd_Ib_A32(void) { SHIFTDOP_Md_Gd_Ib_A32(shrd) asm (".global _OP_SHRD_Md_Gd_Ib_A32_END\n _OP_SHRD_Md_Gd_Ib_A32_END:"); }

void OP_BSWAP_EAX(void) { BSWAP_Regd(eax) asm (".global _OP_BSWAP_EAX_END\n _OP_BSWAP_EAX_END:"); }
void OP_BSWAP_ECX(void) { BSWAP_Regd(ecx) asm (".global _OP_BSWAP_ECX_END\n _OP_BSWAP_ECX_END:"); }
void OP_BSWAP_EDX(void) { BSWAP_Regd(edx) asm (".global _OP_BSWAP_EDX_END\n _OP_BSWAP_EDX_END:"); }
void OP_BSWAP_EBX(void) { BSWAP_Regd(ebx) asm (".global _OP_BSWAP_EBX_END\n _OP_BSWAP_EBX_END:"); }
void OP_BSWAP_ESP(void) { BSWAP_Regd(esp) asm (".global _OP_BSWAP_ESP_END\n _OP_BSWAP_ESP_END:"); }
void OP_BSWAP_EBP(void) { BSWAP_Regd(ebp) asm (".global _OP_BSWAP_EBP_END\n _OP_BSWAP_EBP_END:"); }
void OP_BSWAP_ESI(void) { BSWAP_Regd(esi) asm (".global _OP_BSWAP_ESI_END\n _OP_BSWAP_ESI_END:"); }
void OP_BSWAP_EDI(void) { BSWAP_Regd(edi) asm (".global _OP_BSWAP_EDI_END\n _OP_BSWAP_EDI_END:"); }
