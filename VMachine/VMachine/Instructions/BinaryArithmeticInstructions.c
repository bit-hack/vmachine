//	VMachine
//	Implementations of binary arithmetic instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define ALUOP_Rb_Gb(op)			\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (Gb), "1" (Rb)	\
	);

#define READFALUOP_Rb_Gb(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "r" (Gb), "1" (Rb)	\
	);

#define ALUOP_Rw_Gw(op)			\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (Gw), "1" (Rw)	\
	);

#define READFALUOP_Rw_Gw(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "r" (Gw), "1" (Rw)	\
	);

#define ALUOP_Rd_Gd(op)			\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (Gd), "1" (Rd)	\
	);

#define READFALUOP_Rd_Gd(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "r" (Gd), "1" (Rd)	\
	);

#define ALUOP_Mb_Gb_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gb), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define READFALUOP_Mb_Gb_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (Gb), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define CMPALUOP_Mb_Gb_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gb), "r" (dest)	\
	);

#define ALUOP_Mw_Gw_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gw), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Gw_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (Gw), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Gw_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gw), "r" (dest)	\
	);

#define ALUOP_Md_Gd_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gd), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Gd_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (Gd), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Gd_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gd), "r" (dest)	\
	);

#define ALUOP_Mb_Gb_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gb), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define READFALUOP_Mb_Gb_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (Gb), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define CMPALUOP_Mb_Gb_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gb), "r" (dest)	\
	);

#define ALUOP_Mw_Gw_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gw), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Gw_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (Gw), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Gw_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gw), "r" (dest)	\
	);

#define ALUOP_Md_Gd_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (Gd), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Gd_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (Gd), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Gd_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (Gd), "r" (dest)	\
	);

#define ALUOP_Gb_Rb(op)			\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (Rb), "1" (Gb)	\
	);

#define READFALUOP_Gb_Rb(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (registers->r_eflags), "r" (Rb), "1" (Gb)	\
	);

#define ALUOP_Gw_Rw(op)			\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (Rw), "1" (Gw)	\
	);

#define READFALUOP_Gw_Rw(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (registers->r_eflags), "r" (Rw), "1" (Gw)	\
	);

#define ALUOP_Gd_Rd(op)			\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (Rd), "1" (Gd)	\
	);

#define READFALUOP_Gd_Rd(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (registers->r_eflags), "r" (Rd), "1" (Gd)	\
	);

#define ALUOP_Gb_Mb_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (source), "1" (Gb)	\
	);

#define READFALUOP_Gb_Mb_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gb)	\
	);

#define ALUOP_Gw_Mw_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (source), "1" (Gw)	\
	);

#define READFALUOP_Gw_Mw_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gw)	\
	);

#define ALUOP_Gd_Md_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (source), "1" (Gd)	\
	);

#define READFALUOP_Gd_Md_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gd)	\
	);

#define ALUOP_Gb_Mb_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (source), "1" (Gb)	\
	);

#define READFALUOP_Gb_Mb_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gb)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gb)	\
	);

#define ALUOP_Gw_Mw_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (source), "1" (Gw)	\
	);

#define READFALUOP_Gw_Mw_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gw)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gw)	\
	);

#define ALUOP_Gd_Md_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (source), "1" (Gd)	\
	);

#define READFALUOP_Gd_Md_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Gd)	\
		: "r" (registers->r_eflags), "r" (source), "1" (Gd)	\
	);

#define ALUOP_Rb_Ib(op)			\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (immByte), "1" (Rb)	\
	);

#define READFALUOP_Rb_Ib(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "r" (immByte), "1" (Rb)	\
	);

#define ALUOP_Rw_Iw(op)			\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (immWord), "1" (Rw)	\
	);

#define READFALUOP_Rw_Iw(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "r" (immWord), "1" (Rw)	\
	);

#define ALUOP_Rd_Id(op)			\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (immDword), "1" (Rd)	\
	);

#define READFALUOP_Rd_Id(op)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "r" (immDword), "1" (Rd)	\
	);

#define ALUOP_Rw_Ib(op)			\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (temp), "1" (Rw)	\
	);

#define READFALUOP_Rw_Ib(op)	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (Rw)	\
	);

#define ALUOP_Rd_Ib(op)			\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (temp), "1" (Rd)	\
	);

#define READFALUOP_Rd_Ib(op)	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (Rd)	\
	);

#define ALUOP_Mb_Ib_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immByte), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define READFALUOP_Mb_Ib_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (immByte), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define CMPALUOP_Mb_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immByte), "r" (dest)		\
	);

#define ALUOP_Mw_Iw_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immWord), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Iw_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (immWord), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Iw_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immWord), "r" (dest)		\
	);

#define ALUOP_Md_Id_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immDword), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Id_A16(op)	\
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
		: "r" (registers->r_eflags), "r" (immDword), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Id_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immDword), "r" (dest)	\
	);

#define ALUOP_Mw_Ib_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (temp), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (temp), "r" (dest)	\
	);

#define ALUOP_Md_Ib_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (temp), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Ib_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (temp), "r" (dest)	\
	);

#define ALUOP_Mb_Ib_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immByte), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define READFALUOP_Mb_Ib_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (immByte), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define CMPALUOP_Mb_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immByte), "r" (dest)		\
	);

#define ALUOP_Mw_Iw_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immWord), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Iw_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (immWord), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Iw_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immWord), "r" (dest)		\
	);

#define ALUOP_Md_Id_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (immDword), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Id_A32(op)	\
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
		: "r" (registers->r_eflags), "r" (immDword), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Id_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immDword), "r" (dest)	\
	);

#define ALUOP_Mw_Ib_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"w %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (temp), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define READFALUOP_Mw_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define CMPALUOP_Mw_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	Word temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"w %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (temp), "r" (dest)	\
	);

#define ALUOP_Md_Ib_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"l %2, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (temp), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define READFALUOP_Md_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %3, %1	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "r" (temp), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define CMPALUOP_Md_Ib_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	Dword temp = (SignedByte)immByte;	\
	asm volatile				\
	(							\
		#op"l %1, %2	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (temp), "r" (dest)	\
	);

#define ALUOP_AL_Ib(op)			\
	Byte * dest = &registers->r_al;	\
	asm volatile				\
	(							\
		#op"b %1, (%2)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immByte), "r" (dest)	\
	);

#define READFALUOP_AL_Ib(op)	\
	Byte * dest = &registers->r_al;	\
	asm volatile				\
	(							\
		"pushl %1		\n"		\
		"popfl			\n"		\
		#op"b %2, (%3)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (registers->r_eflags), "r" (immByte), "r" (dest)	\
	);

#define ALUOP_AX_Iw(op)			\
	Word * dest = &registers->r_ax;	\
	asm volatile				\
	(							\
		#op"w %1, (%2)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immWord), "r" (dest)	\
	);

#define READFALUOP_AX_Iw(op)	\
	Word * dest = &registers->r_ax;	\
	asm volatile				\
	(							\
		"pushl %1		\n"		\
		"popfl			\n"		\
		#op"w %2, (%3)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (registers->r_eflags), "r" (immWord), "r" (dest)	\
	);

#define ALUOP_EAX_Id(op)		\
	Dword * dest = &registers->r_eax;	\
	asm volatile				\
	(							\
		#op"l %1, (%2)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (immDword), "r" (dest)	\
	);

#define READFALUOP_EAX_Id(op)	\
	Dword * dest = &registers->r_eax;	\
	asm volatile				\
	(							\
		"pushl %1		\n"		\
		"popfl			\n"		\
		#op"l %2, (%3)	\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags)	\
		: "r" (registers->r_eflags), "r" (immDword), "r" (dest)	\
	);

#define INCDEC_Regw(op, reg)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (registers->r_##reg)	\
		: "r" (registers->r_eflags), "1" (registers->r_##reg)	\
	);

#define INCDEC_Regd(op, reg)	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (registers->r_##reg)	\
		: "r" (registers->r_eflags), "1" (registers->r_##reg)	\
	);

#define UALUOP_Rb(op)			\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rb)	\
		: "r" (registers->r_eflags), "1" (Rb)	\
	);

#define UALUOP_Mb_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define UALUOP_Mb_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte dest = ReadByte(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"b %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteByte(address, dest);

#define UALUOP_Rw(op)			\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rw)	\
		: "r" (registers->r_eflags), "1" (Rw)	\
	);

#define UALUOP_Mw_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define UALUOP_Mw_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word dest = ReadWord(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"w %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteWord(address, dest);

#define UALUOP_Rd(op)			\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (Rd)	\
		: "r" (registers->r_eflags), "1" (Rd)	\
	);

#define UALUOP_Md_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define UALUOP_Md_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Dword dest = ReadDword(address);	\
	asm volatile				\
	(							\
		"pushl %2		\n"		\
		"popfl			\n"		\
		#op"l %1		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=r" (dest)	\
		: "r" (registers->r_eflags), "1" (dest)	\
	);							\
    WriteDword(address, dest);

#define MULOP_AL_Rb(op)			\
	asm volatile				\
	(							\
		#op"b %3		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax)	\
		: "a" (registers->r_al), "r" (Rb)	\
	);

#define MULOP_AX_Rw(op)			\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "r" (Rw)	\
	);

#define MULOP_EAX_Rd(op)			\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "r" (Rd)	\
	);

#define MULOP_AL_Mb_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %3		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax)	\
		: "a" (registers->r_al), "r" (source)	\
	);

#define MULOP_AX_Mw_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "r" (source)	\
	);

#define MULOP_EAX_Md_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "r" (source)	\
	);

#define MULOP_AL_Mb_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %3		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax)	\
		: "a" (registers->r_al), "r" (source)	\
	);

#define MULOP_AX_Mw_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "r" (source)	\
	);

#define MULOP_EAX_Md_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		"pushfl			\n"		\
		"popl %0		\n"		\
		: "=r" (registers->r_eflags), "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "r" (source)	\
	);

#define DIVOP_AX_Rb(op)			\
	asm volatile				\
	(							\
		#op"b %2		\n"		\
		: "=a" (registers->r_ax)	\
		: "a" (registers->r_ax), "r" (Rb)	\
	);

#define DIVOP_DX_AX_Rw(op)		\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		: "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "d" (registers->r_dx), "r" (Rw)	\
	);

#define DIVOP_EDX_EAX_Rd(op)	\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		: "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "d" (registers->r_edx), "r" (Rd)	\
	);

#define DIVOP_AX_Mb_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2		\n"		\
		: "=a" (registers->r_ax)	\
		: "a" (registers->r_ax), "r" (source)	\
	);

#define DIVOP_DX_AX_Mw_A16(op)	\
	Dword address = CalculateAddressA16();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		: "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "d" (registers->r_dx), "r" (source)	\
	);

#define DIVOP_EDX_EAX_Md_A16(op)\
	Dword address = CalculateAddressA16();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		: "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "d" (registers->r_edx), "r" (source)	\
	);

#define DIVOP_AX_Mb_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Byte source = ReadByte(address);	\
	asm volatile				\
	(							\
		#op"b %2		\n"		\
		: "=a" (registers->r_ax)	\
		: "a" (registers->r_ax), "r" (source)	\
	);

#define DIVOP_DX_AX_Mw_A32(op)	\
	Dword address = CalculateAddressA32();	\
	Word source = ReadWord(address);	\
	asm volatile				\
	(							\
		#op"w %4		\n"		\
		: "=a" (registers->r_ax), "=d" (registers->r_dx)	\
		: "a" (registers->r_ax), "d" (registers->r_dx), "r" (source)	\
	);

#define DIVOP_EDX_EAX_Md_A32(op)\
	Dword address = CalculateAddressA32();	\
	Dword source = ReadDword(address);	\
	asm volatile				\
	(							\
		#op"l %4		\n"		\
		: "=a" (registers->r_eax), "=d" (registers->r_edx)	\
		: "a" (registers->r_eax), "d" (registers->r_edx), "r" (source)	\
	);

void OP_ADD_Rb_Gb(void) { ALUOP_Rb_Gb(add) asm (".global _OP_ADD_Rb_Gb_END\n _OP_ADD_Rb_Gb_END:"); }
void OP_OR_Rb_Gb(void) { ALUOP_Rb_Gb(or) asm (".global _OP_OR_Rb_Gb_END\n _OP_OR_Rb_Gb_END:"); }
void OP_ADC_Rb_Gb(void) { READFALUOP_Rb_Gb(adc) asm (".global _OP_ADC_Rb_Gb_END\n _OP_ADC_Rb_Gb_END:"); }
void OP_SBB_Rb_Gb(void) { READFALUOP_Rb_Gb(sbb) asm (".global _OP_SBB_Rb_Gb_END\n _OP_SBB_Rb_Gb_END:"); }
void OP_AND_Rb_Gb(void) { ALUOP_Rb_Gb(and) asm (".global _OP_AND_Rb_Gb_END\n _OP_AND_Rb_Gb_END:"); }
void OP_SUB_Rb_Gb(void) { ALUOP_Rb_Gb(sub) asm (".global _OP_SUB_Rb_Gb_END\n _OP_SUB_Rb_Gb_END:"); }
void OP_XOR_Rb_Gb(void) { ALUOP_Rb_Gb(xor) asm (".global _OP_XOR_Rb_Gb_END\n _OP_XOR_Rb_Gb_END:"); }
void OP_CMP_Rb_Gb(void) { ALUOP_Rb_Gb(cmp) asm (".global _OP_CMP_Rb_Gb_END\n _OP_CMP_Rb_Gb_END:"); }
void OP_TEST_Rb_Gb(void) { ALUOP_Rb_Gb(test) asm (".global _OP_TEST_Rb_Gb_END\n _OP_TEST_Rb_Gb_END:"); }

void OP_ADD_Rw_Gw(void) { ALUOP_Rw_Gw(add) asm (".global _OP_ADD_Rw_Gw_END\n _OP_ADD_Rw_Gw_END:"); }
void OP_OR_Rw_Gw(void) { ALUOP_Rw_Gw(or) asm (".global _OP_OR_Rw_Gw_END\n _OP_OR_Rw_Gw_END:"); }
void OP_ADC_Rw_Gw(void) { READFALUOP_Rw_Gw(adc) asm (".global _OP_ADC_Rw_Gw_END\n _OP_ADC_Rw_Gw_END:"); }
void OP_SBB_Rw_Gw(void) { READFALUOP_Rw_Gw(sbb) asm (".global _OP_SBB_Rw_Gw_END\n _OP_SBB_Rw_Gw_END:"); }
void OP_AND_Rw_Gw(void) { ALUOP_Rw_Gw(and) asm (".global _OP_AND_Rw_Gw_END\n _OP_AND_Rw_Gw_END:"); }
void OP_SUB_Rw_Gw(void) { ALUOP_Rw_Gw(sub) asm (".global _OP_SUB_Rw_Gw_END\n _OP_SUB_Rw_Gw_END:"); }
void OP_XOR_Rw_Gw(void) { ALUOP_Rw_Gw(xor) asm (".global _OP_XOR_Rw_Gw_END\n _OP_XOR_Rw_Gw_END:"); }
void OP_CMP_Rw_Gw(void) { ALUOP_Rw_Gw(cmp) asm (".global _OP_CMP_Rw_Gw_END\n _OP_CMP_Rw_Gw_END:"); }
void OP_TEST_Rw_Gw(void) { ALUOP_Rw_Gw(test) asm (".global _OP_TEST_Rw_Gw_END\n _OP_TEST_Rw_Gw_END:"); }

void OP_ADD_Rd_Gd(void) { ALUOP_Rd_Gd(add) asm (".global _OP_ADD_Rd_Gd_END\n _OP_ADD_Rd_Gd_END:"); }
void OP_OR_Rd_Gd(void) { ALUOP_Rd_Gd(or) asm (".global _OP_OR_Rd_Gd_END\n _OP_OR_Rd_Gd_END:"); }
void OP_ADC_Rd_Gd(void) { READFALUOP_Rd_Gd(adc) asm (".global _OP_ADC_Rd_Gd_END\n _OP_ADC_Rd_Gd_END:"); }
void OP_SBB_Rd_Gd(void) { READFALUOP_Rd_Gd(sbb) asm (".global _OP_SBB_Rd_Gd_END\n _OP_SBB_Rd_Gd_END:"); }
void OP_AND_Rd_Gd(void) { ALUOP_Rd_Gd(and) asm (".global _OP_AND_Rd_Gd_END\n _OP_AND_Rd_Gd_END:"); }
void OP_SUB_Rd_Gd(void) { ALUOP_Rd_Gd(sub) asm (".global _OP_SUB_Rd_Gd_END\n _OP_SUB_Rd_Gd_END:"); }
void OP_XOR_Rd_Gd(void) { ALUOP_Rd_Gd(xor) asm (".global _OP_XOR_Rd_Gd_END\n _OP_XOR_Rd_Gd_END:"); }
void OP_CMP_Rd_Gd(void) { ALUOP_Rd_Gd(cmp) asm (".global _OP_CMP_Rd_Gd_END\n _OP_CMP_Rd_Gd_END:"); }
void OP_TEST_Rd_Gd(void) { ALUOP_Rd_Gd(test) asm (".global _OP_TEST_Rd_Gd_END\n _OP_TEST_Rd_Gd_END:"); }

void OP_ADD_Mb_Gb_A16(void) { ALUOP_Mb_Gb_A16(add) asm (".global _OP_ADD_Mb_Gb_A16_END\n _OP_ADD_Mb_Gb_A16_END:"); }
void OP_OR_Mb_Gb_A16(void) { ALUOP_Mb_Gb_A16(or) asm (".global _OP_OR_Mb_Gb_A16_END\n _OP_OR_Mb_Gb_A16_END:"); }
void OP_ADC_Mb_Gb_A16(void) { READFALUOP_Mb_Gb_A16(adc) asm (".global _OP_ADC_Mb_Gb_A16_END\n _OP_ADC_Mb_Gb_A16_END:"); }
void OP_SBB_Mb_Gb_A16(void) { READFALUOP_Mb_Gb_A16(sbb) asm (".global _OP_SBB_Mb_Gb_A16_END\n _OP_SBB_Mb_Gb_A16_END:"); }
void OP_AND_Mb_Gb_A16(void) { ALUOP_Mb_Gb_A16(and) asm (".global _OP_AND_Mb_Gb_A16_END\n _OP_AND_Mb_Gb_A16_END:"); }
void OP_SUB_Mb_Gb_A16(void) { ALUOP_Mb_Gb_A16(sub) asm (".global _OP_SUB_Mb_Gb_A16_END\n _OP_SUB_Mb_Gb_A16_END:"); }
void OP_XOR_Mb_Gb_A16(void) { ALUOP_Mb_Gb_A16(xor) asm (".global _OP_XOR_Mb_Gb_A16_END\n _OP_XOR_Mb_Gb_A16_END:"); }
void OP_CMP_Mb_Gb_A16(void) { CMPALUOP_Mb_Gb_A16(cmp) asm (".global _OP_CMP_Mb_Gb_A16_END\n _OP_CMP_Mb_Gb_A16_END:"); }
void OP_TEST_Mb_Gb_A16(void) { CMPALUOP_Mb_Gb_A16(test) asm (".global _OP_TEST_Mb_Gb_A16_END\n _OP_TEST_Mb_Gb_A16_END:"); }

void OP_ADD_Mw_Gw_A16(void) { ALUOP_Mw_Gw_A16(add) asm (".global _OP_ADD_Mw_Gw_A16_END\n _OP_ADD_Mw_Gw_A16_END:"); }
void OP_OR_Mw_Gw_A16(void) { ALUOP_Mw_Gw_A16(or) asm (".global _OP_OR_Mw_Gw_A16_END\n _OP_OR_Mw_Gw_A16_END:"); }
void OP_ADC_Mw_Gw_A16(void) { READFALUOP_Mw_Gw_A16(adc) asm (".global _OP_ADC_Mw_Gw_A16_END\n _OP_ADC_Mw_Gw_A16_END:"); }
void OP_SBB_Mw_Gw_A16(void) { READFALUOP_Mw_Gw_A16(sbb) asm (".global _OP_SBB_Mw_Gw_A16_END\n _OP_SBB_Mw_Gw_A16_END:"); }
void OP_AND_Mw_Gw_A16(void) { ALUOP_Mw_Gw_A16(and) asm (".global _OP_AND_Mw_Gw_A16_END\n _OP_AND_Mw_Gw_A16_END:"); }
void OP_SUB_Mw_Gw_A16(void) { ALUOP_Mw_Gw_A16(sub) asm (".global _OP_SUB_Mw_Gw_A16_END\n _OP_SUB_Mw_Gw_A16_END:"); }
void OP_XOR_Mw_Gw_A16(void) { ALUOP_Mw_Gw_A16(xor) asm (".global _OP_XOR_Mw_Gw_A16_END\n _OP_XOR_Mw_Gw_A16_END:"); }
void OP_CMP_Mw_Gw_A16(void) { CMPALUOP_Mw_Gw_A16(cmp) asm (".global _OP_CMP_Mw_Gw_A16_END\n _OP_CMP_Mw_Gw_A16_END:"); }
void OP_TEST_Mw_Gw_A16(void) { CMPALUOP_Mw_Gw_A16(test) asm (".global _OP_TEST_Mw_Gw_A16_END\n _OP_TEST_Mw_Gw_A16_END:"); }

void OP_ADD_Md_Gd_A16(void) { ALUOP_Md_Gd_A16(add) asm (".global _OP_ADD_Md_Gd_A16_END\n _OP_ADD_Md_Gd_A16_END:"); }
void OP_OR_Md_Gd_A16(void) { ALUOP_Md_Gd_A16(or) asm (".global _OP_OR_Md_Gd_A16_END\n _OP_OR_Md_Gd_A16_END:"); }
void OP_ADC_Md_Gd_A16(void) { READFALUOP_Md_Gd_A16(adc) asm (".global _OP_ADC_Md_Gd_A16_END\n _OP_ADC_Md_Gd_A16_END:"); }
void OP_SBB_Md_Gd_A16(void) { READFALUOP_Md_Gd_A16(sbb) asm (".global _OP_SBB_Md_Gd_A16_END\n _OP_SBB_Md_Gd_A16_END:"); }
void OP_AND_Md_Gd_A16(void) { ALUOP_Md_Gd_A16(and) asm (".global _OP_AND_Md_Gd_A16_END\n _OP_AND_Md_Gd_A16_END:"); }
void OP_SUB_Md_Gd_A16(void) { ALUOP_Md_Gd_A16(sub) asm (".global _OP_SUB_Md_Gd_A16_END\n _OP_SUB_Md_Gd_A16_END:"); }
void OP_XOR_Md_Gd_A16(void) { ALUOP_Md_Gd_A16(xor) asm (".global _OP_XOR_Md_Gd_A16_END\n _OP_XOR_Md_Gd_A16_END:"); }
void OP_CMP_Md_Gd_A16(void) { CMPALUOP_Md_Gd_A16(cmp) asm (".global _OP_CMP_Md_Gd_A16_END\n _OP_CMP_Md_Gd_A16_END:"); }
void OP_TEST_Md_Gd_A16(void) { CMPALUOP_Md_Gd_A16(test) asm (".global _OP_TEST_Md_Gd_A16_END\n _OP_TEST_Md_Gd_A16_END:"); }

void OP_ADD_Mb_Gb_A32(void) { ALUOP_Mb_Gb_A32(add) asm (".global _OP_ADD_Mb_Gb_A32_END\n _OP_ADD_Mb_Gb_A32_END:"); }
void OP_OR_Mb_Gb_A32(void) { ALUOP_Mb_Gb_A32(or) asm (".global _OP_OR_Mb_Gb_A32_END\n _OP_OR_Mb_Gb_A32_END:"); }
void OP_ADC_Mb_Gb_A32(void) { READFALUOP_Mb_Gb_A32(adc) asm (".global _OP_ADC_Mb_Gb_A32_END\n _OP_ADC_Mb_Gb_A32_END:"); }
void OP_SBB_Mb_Gb_A32(void) { READFALUOP_Mb_Gb_A32(sbb) asm (".global _OP_SBB_Mb_Gb_A32_END\n _OP_SBB_Mb_Gb_A32_END:"); }
void OP_AND_Mb_Gb_A32(void) { ALUOP_Mb_Gb_A32(and) asm (".global _OP_AND_Mb_Gb_A32_END\n _OP_AND_Mb_Gb_A32_END:"); }
void OP_SUB_Mb_Gb_A32(void) { ALUOP_Mb_Gb_A32(sub) asm (".global _OP_SUB_Mb_Gb_A32_END\n _OP_SUB_Mb_Gb_A32_END:"); }
void OP_XOR_Mb_Gb_A32(void) { ALUOP_Mb_Gb_A32(xor) asm (".global _OP_XOR_Mb_Gb_A32_END\n _OP_XOR_Mb_Gb_A32_END:"); }
void OP_CMP_Mb_Gb_A32(void) { CMPALUOP_Mb_Gb_A32(cmp) asm (".global _OP_CMP_Mb_Gb_A32_END\n _OP_CMP_Mb_Gb_A32_END:"); }
void OP_TEST_Mb_Gb_A32(void) { CMPALUOP_Mb_Gb_A32(test) asm (".global _OP_TEST_Mb_Gb_A32_END\n _OP_TEST_Mb_Gb_A32_END:"); }

void OP_ADD_Mw_Gw_A32(void) { ALUOP_Mw_Gw_A32(add) asm (".global _OP_ADD_Mw_Gw_A32_END\n _OP_ADD_Mw_Gw_A32_END:"); }
void OP_OR_Mw_Gw_A32(void) { ALUOP_Mw_Gw_A32(or) asm (".global _OP_OR_Mw_Gw_A32_END\n _OP_OR_Mw_Gw_A32_END:"); }
void OP_ADC_Mw_Gw_A32(void) { READFALUOP_Mw_Gw_A32(adc) asm (".global _OP_ADC_Mw_Gw_A32_END\n _OP_ADC_Mw_Gw_A32_END:"); }
void OP_SBB_Mw_Gw_A32(void) { READFALUOP_Mw_Gw_A32(sbb) asm (".global _OP_SBB_Mw_Gw_A32_END\n _OP_SBB_Mw_Gw_A32_END:"); }
void OP_AND_Mw_Gw_A32(void) { ALUOP_Mw_Gw_A32(and) asm (".global _OP_AND_Mw_Gw_A32_END\n _OP_AND_Mw_Gw_A32_END:"); }
void OP_SUB_Mw_Gw_A32(void) { ALUOP_Mw_Gw_A32(sub) asm (".global _OP_SUB_Mw_Gw_A32_END\n _OP_SUB_Mw_Gw_A32_END:"); }
void OP_XOR_Mw_Gw_A32(void) { ALUOP_Mw_Gw_A32(xor) asm (".global _OP_XOR_Mw_Gw_A32_END\n _OP_XOR_Mw_Gw_A32_END:"); }
void OP_CMP_Mw_Gw_A32(void) { CMPALUOP_Mw_Gw_A32(cmp) asm (".global _OP_CMP_Mw_Gw_A32_END\n _OP_CMP_Mw_Gw_A32_END:"); }
void OP_TEST_Mw_Gw_A32(void) { CMPALUOP_Mw_Gw_A32(test) asm (".global _OP_TEST_Mw_Gw_A32_END\n _OP_TEST_Mw_Gw_A32_END:"); }

void OP_ADD_Md_Gd_A32(void) { ALUOP_Md_Gd_A32(add) asm (".global _OP_ADD_Md_Gd_A32_END\n _OP_ADD_Md_Gd_A32_END:"); }
void OP_OR_Md_Gd_A32(void) { ALUOP_Md_Gd_A32(or) asm (".global _OP_OR_Md_Gd_A32_END\n _OP_OR_Md_Gd_A32_END:"); }
void OP_ADC_Md_Gd_A32(void) { READFALUOP_Md_Gd_A32(adc) asm (".global _OP_ADC_Md_Gd_A32_END\n _OP_ADC_Md_Gd_A32_END:"); }
void OP_SBB_Md_Gd_A32(void) { READFALUOP_Md_Gd_A32(sbb) asm (".global _OP_SBB_Md_Gd_A32_END\n _OP_SBB_Md_Gd_A32_END:"); }
void OP_AND_Md_Gd_A32(void) { ALUOP_Md_Gd_A32(and) asm (".global _OP_AND_Md_Gd_A32_END\n _OP_AND_Md_Gd_A32_END:"); }
void OP_SUB_Md_Gd_A32(void) { ALUOP_Md_Gd_A32(sub) asm (".global _OP_SUB_Md_Gd_A32_END\n _OP_SUB_Md_Gd_A32_END:"); }
void OP_XOR_Md_Gd_A32(void) { ALUOP_Md_Gd_A32(xor) asm (".global _OP_XOR_Md_Gd_A32_END\n _OP_XOR_Md_Gd_A32_END:"); }
void OP_CMP_Md_Gd_A32(void) { CMPALUOP_Md_Gd_A32(cmp) asm (".global _OP_CMP_Md_Gd_A32_END\n _OP_CMP_Md_Gd_A32_END:"); }
void OP_TEST_Md_Gd_A32(void) { CMPALUOP_Md_Gd_A32(test) asm (".global _OP_TEST_Md_Gd_A32_END\n _OP_TEST_Md_Gd_A32_END:"); }

void OP_ADD_Gb_Rb(void) { ALUOP_Gb_Rb(add) asm (".global _OP_ADD_Gb_Rb_END\n _OP_ADD_Gb_Rb_END:"); }
void OP_OR_Gb_Rb(void) { ALUOP_Gb_Rb(or) asm (".global _OP_OR_Gb_Rb_END\n _OP_OR_Gb_Rb_END:"); }
void OP_ADC_Gb_Rb(void) { READFALUOP_Gb_Rb(adc) asm (".global _OP_ADC_Gb_Rb_END\n _OP_ADC_Gb_Rb_END:"); }
void OP_SBB_Gb_Rb(void) { READFALUOP_Gb_Rb(sbb) asm (".global _OP_SBB_Gb_Rb_END\n _OP_SBB_Gb_Rb_END:"); }
void OP_AND_Gb_Rb(void) { ALUOP_Gb_Rb(and) asm (".global _OP_AND_Gb_Rb_END\n _OP_AND_Gb_Rb_END:"); }
void OP_SUB_Gb_Rb(void) { ALUOP_Gb_Rb(sub) asm (".global _OP_SUB_Gb_Rb_END\n _OP_SUB_Gb_Rb_END:"); }
void OP_XOR_Gb_Rb(void) { ALUOP_Gb_Rb(xor) asm (".global _OP_XOR_Gb_Rb_END\n _OP_XOR_Gb_Rb_END:"); }
void OP_CMP_Gb_Rb(void) { ALUOP_Gb_Rb(cmp) asm (".global _OP_CMP_Gb_Rb_END\n _OP_CMP_Gb_Rb_END:"); }

void OP_ADD_Gw_Rw(void) { ALUOP_Gw_Rw(add) asm (".global _OP_ADD_Gw_Rw_END\n _OP_ADD_Gw_Rw_END:"); }
void OP_OR_Gw_Rw(void) { ALUOP_Gw_Rw(or) asm (".global _OP_OR_Gw_Rw_END\n _OP_OR_Gw_Rw_END:"); }
void OP_ADC_Gw_Rw(void) { READFALUOP_Gw_Rw(adc) asm (".global _OP_ADC_Gw_Rw_END\n _OP_ADC_Gw_Rw_END:"); }
void OP_SBB_Gw_Rw(void) { READFALUOP_Gw_Rw(sbb) asm (".global _OP_SBB_Gw_Rw_END\n _OP_SBB_Gw_Rw_END:"); }
void OP_AND_Gw_Rw(void) { ALUOP_Gw_Rw(and) asm (".global _OP_AND_Gw_Rw_END\n _OP_AND_Gw_Rw_END:"); }
void OP_SUB_Gw_Rw(void) { ALUOP_Gw_Rw(sub) asm (".global _OP_SUB_Gw_Rw_END\n _OP_SUB_Gw_Rw_END:"); }
void OP_XOR_Gw_Rw(void) { ALUOP_Gw_Rw(xor) asm (".global _OP_XOR_Gw_Rw_END\n _OP_XOR_Gw_Rw_END:"); }
void OP_CMP_Gw_Rw(void) { ALUOP_Gw_Rw(cmp) asm (".global _OP_CMP_Gw_Rw_END\n _OP_CMP_Gw_Rw_END:"); }
void OP_BSF_Gw_Rw(void) { ALUOP_Gw_Rw(bsf) asm (".global _OP_BSF_Gw_Rw_END\n _OP_BSF_Gw_Rw_END:"); }
void OP_BSR_Gw_Rw(void) { ALUOP_Gw_Rw(bsr) asm (".global _OP_BSR_Gw_Rw_END\n _OP_BSR_Gw_Rw_END:"); }

void OP_ADD_Gd_Rd(void) { ALUOP_Gd_Rd(add) asm (".global _OP_ADD_Gd_Rd_END\n _OP_ADD_Gd_Rd_END:"); }
void OP_OR_Gd_Rd(void) { ALUOP_Gd_Rd(or) asm (".global _OP_OR_Gd_Rd_END\n _OP_OR_Gd_Rd_END:"); }
void OP_ADC_Gd_Rd(void) { READFALUOP_Gd_Rd(adc) asm (".global _OP_ADC_Gd_Rd_END\n _OP_ADC_Gd_Rd_END:"); }
void OP_SBB_Gd_Rd(void) { READFALUOP_Gd_Rd(sbb) asm (".global _OP_SBB_Gd_Rd_END\n _OP_SBB_Gd_Rd_END:"); }
void OP_AND_Gd_Rd(void) { ALUOP_Gd_Rd(and) asm (".global _OP_AND_Gd_Rd_END\n _OP_AND_Gd_Rd_END:"); }
void OP_SUB_Gd_Rd(void) { ALUOP_Gd_Rd(sub) asm (".global _OP_SUB_Gd_Rd_END\n _OP_SUB_Gd_Rd_END:"); }
void OP_XOR_Gd_Rd(void) { ALUOP_Gd_Rd(xor) asm (".global _OP_XOR_Gd_Rd_END\n _OP_XOR_Gd_Rd_END:"); }
void OP_CMP_Gd_Rd(void) { ALUOP_Gd_Rd(cmp) asm (".global _OP_CMP_Gd_Rd_END\n _OP_CMP_Gd_Rd_END:"); }
void OP_BSF_Gd_Rd(void) { ALUOP_Gd_Rd(bsf) asm (".global _OP_BSF_Gd_Rd_END\n _OP_BSF_Gd_Rd_END:"); }
void OP_BSR_Gd_Rd(void) { ALUOP_Gd_Rd(bsr) asm (".global _OP_BSR_Gd_Rd_END\n _OP_BSR_Gd_Rd_END:"); }

void OP_ADD_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(add) asm (".global _OP_ADD_Gb_Mb_A16_END\n _OP_ADD_Gb_Mb_A16_END:"); }
void OP_OR_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(or) asm (".global _OP_OR_Gb_Mb_A16_END\n _OP_OR_Gb_Mb_A16_END:"); }
void OP_ADC_Gb_Mb_A16(void) { READFALUOP_Gb_Mb_A16(adc) asm (".global _OP_ADC_Gb_Mb_A16_END\n _OP_ADC_Gb_Mb_A16_END:"); }
void OP_SBB_Gb_Mb_A16(void) { READFALUOP_Gb_Mb_A16(sbb) asm (".global _OP_SBB_Gb_Mb_A16_END\n _OP_SBB_Gb_Mb_A16_END:"); }
void OP_AND_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(and) asm (".global _OP_AND_Gb_Mb_A16_END\n _OP_AND_Gb_Mb_A16_END:"); }
void OP_SUB_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(sub) asm (".global _OP_SUB_Gb_Mb_A16_END\n _OP_SUB_Gb_Mb_A16_END:"); }
void OP_XOR_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(xor) asm (".global _OP_XOR_Gb_Mb_A16_END\n _OP_XOR_Gb_Mb_A16_END:"); }
void OP_CMP_Gb_Mb_A16(void) { ALUOP_Gb_Mb_A16(cmp) asm (".global _OP_CMP_Gb_Mb_A16_END\n _OP_CMP_Gb_Mb_A16_END:"); }

void OP_ADD_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(add) asm (".global _OP_ADD_Gw_Mw_A16_END\n _OP_ADD_Gw_Mw_A16_END:"); }
void OP_OR_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(or) asm (".global _OP_OR_Gw_Mw_A16_END\n _OP_OR_Gw_Mw_A16_END:"); }
void OP_ADC_Gw_Mw_A16(void) { READFALUOP_Gw_Mw_A16(adc) asm (".global _OP_ADC_Gw_Mw_A16_END\n _OP_ADC_Gw_Mw_A16_END:"); }
void OP_SBB_Gw_Mw_A16(void) { READFALUOP_Gw_Mw_A16(sbb) asm (".global _OP_SBB_Gw_Mw_A16_END\n _OP_SBB_Gw_Mw_A16_END:"); }
void OP_AND_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(and) asm (".global _OP_AND_Gw_Mw_A16_END\n _OP_AND_Gw_Mw_A16_END:"); }
void OP_SUB_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(sub) asm (".global _OP_SUB_Gw_Mw_A16_END\n _OP_SUB_Gw_Mw_A16_END:"); }
void OP_XOR_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(xor) asm (".global _OP_XOR_Gw_Mw_A16_END\n _OP_XOR_Gw_Mw_A16_END:"); }
void OP_CMP_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(cmp) asm (".global _OP_CMP_Gw_Mw_A16_END\n _OP_CMP_Gw_Mw_A16_END:"); }
void OP_BSF_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(bsf) asm (".global _OP_BSF_Gw_Mw_A16_END\n _OP_BSF_Gw_Mw_A16_END:"); }
void OP_BSR_Gw_Mw_A16(void) { ALUOP_Gw_Mw_A16(bsr) asm (".global _OP_BSR_Gw_Mw_A16_END\n _OP_BSR_Gw_Mw_A16_END:"); }

void OP_ADD_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(add) asm (".global _OP_ADD_Gd_Md_A16_END\n _OP_ADD_Gd_Md_A16_END:"); }
void OP_OR_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(or) asm (".global _OP_OR_Gd_Md_A16_END\n _OP_OR_Gd_Md_A16_END:"); }
void OP_ADC_Gd_Md_A16(void) { READFALUOP_Gd_Md_A16(adc) asm (".global _OP_ADC_Gd_Md_A16_END\n _OP_ADC_Gd_Md_A16_END:"); }
void OP_SBB_Gd_Md_A16(void) { READFALUOP_Gd_Md_A16(sbb) asm (".global _OP_SBB_Gd_Md_A16_END\n _OP_SBB_Gd_Md_A16_END:"); }
void OP_AND_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(and) asm (".global _OP_AND_Gd_Md_A16_END\n _OP_AND_Gd_Md_A16_END:"); }
void OP_SUB_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(sub) asm (".global _OP_SUB_Gd_Md_A16_END\n _OP_SUB_Gd_Md_A16_END:"); }
void OP_XOR_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(xor) asm (".global _OP_XOR_Gd_Md_A16_END\n _OP_XOR_Gd_Md_A16_END:"); }
void OP_CMP_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(cmp) asm (".global _OP_CMP_Gd_Md_A16_END\n _OP_CMP_Gd_Md_A16_END:"); }
void OP_BSF_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(bsf) asm (".global _OP_BSF_Gd_Md_A16_END\n _OP_BSF_Gd_Md_A16_END:"); }
void OP_BSR_Gd_Md_A16(void) { ALUOP_Gd_Md_A16(bsr) asm (".global _OP_BSR_Gd_Md_A16_END\n _OP_BSR_Gd_Md_A16_END:"); }

void OP_ADD_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(add) asm (".global _OP_ADD_Gb_Mb_A32_END\n _OP_ADD_Gb_Mb_A32_END:"); }
void OP_OR_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(or) asm (".global _OP_OR_Gb_Mb_A32_END\n _OP_OR_Gb_Mb_A32_END:"); }
void OP_ADC_Gb_Mb_A32(void) { READFALUOP_Gb_Mb_A32(adc) asm (".global _OP_ADC_Gb_Mb_A32_END\n _OP_ADC_Gb_Mb_A32_END:"); }
void OP_SBB_Gb_Mb_A32(void) { READFALUOP_Gb_Mb_A32(sbb) asm (".global _OP_SBB_Gb_Mb_A32_END\n _OP_SBB_Gb_Mb_A32_END:"); }
void OP_AND_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(and) asm (".global _OP_AND_Gb_Mb_A32_END\n _OP_AND_Gb_Mb_A32_END:"); }
void OP_SUB_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(sub) asm (".global _OP_SUB_Gb_Mb_A32_END\n _OP_SUB_Gb_Mb_A32_END:"); }
void OP_XOR_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(xor) asm (".global _OP_XOR_Gb_Mb_A32_END\n _OP_XOR_Gb_Mb_A32_END:"); }
void OP_CMP_Gb_Mb_A32(void) { ALUOP_Gb_Mb_A32(cmp) asm (".global _OP_CMP_Gb_Mb_A32_END\n _OP_CMP_Gb_Mb_A32_END:"); }

void OP_ADD_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(add) asm (".global _OP_ADD_Gw_Mw_A32_END\n _OP_ADD_Gw_Mw_A32_END:"); }
void OP_OR_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(or) asm (".global _OP_OR_Gw_Mw_A32_END\n _OP_OR_Gw_Mw_A32_END:"); }
void OP_ADC_Gw_Mw_A32(void) { READFALUOP_Gw_Mw_A32(adc) asm (".global _OP_ADC_Gw_Mw_A32_END\n _OP_ADC_Gw_Mw_A32_END:"); }
void OP_SBB_Gw_Mw_A32(void) { READFALUOP_Gw_Mw_A32(sbb) asm (".global _OP_SBB_Gw_Mw_A32_END\n _OP_SBB_Gw_Mw_A32_END:"); }
void OP_AND_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(and) asm (".global _OP_AND_Gw_Mw_A32_END\n _OP_AND_Gw_Mw_A32_END:"); }
void OP_SUB_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(sub) asm (".global _OP_SUB_Gw_Mw_A32_END\n _OP_SUB_Gw_Mw_A32_END:"); }
void OP_XOR_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(xor) asm (".global _OP_XOR_Gw_Mw_A32_END\n _OP_XOR_Gw_Mw_A32_END:"); }
void OP_CMP_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(cmp) asm (".global _OP_CMP_Gw_Mw_A32_END\n _OP_CMP_Gw_Mw_A32_END:"); }
void OP_BSF_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(bsf) asm (".global _OP_BSF_Gw_Mw_A32_END\n _OP_BSF_Gw_Mw_A32_END:"); }
void OP_BSR_Gw_Mw_A32(void) { ALUOP_Gw_Mw_A32(bsr) asm (".global _OP_BSR_Gw_Mw_A32_END\n _OP_BSR_Gw_Mw_A32_END:"); }

void OP_ADD_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(add) asm (".global _OP_ADD_Gd_Md_A32_END\n _OP_ADD_Gd_Md_A32_END:"); }
void OP_OR_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(or) asm (".global _OP_OR_Gd_Md_A32_END\n _OP_OR_Gd_Md_A32_END:"); }
void OP_ADC_Gd_Md_A32(void) { READFALUOP_Gd_Md_A32(adc) asm (".global _OP_ADC_Gd_Md_A32_END\n _OP_ADC_Gd_Md_A32_END:"); }
void OP_SBB_Gd_Md_A32(void) { READFALUOP_Gd_Md_A32(sbb) asm (".global _OP_SBB_Gd_Md_A32_END\n _OP_SBB_Gd_Md_A32_END:"); }
void OP_AND_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(and) asm (".global _OP_AND_Gd_Md_A32_END\n _OP_AND_Gd_Md_A32_END:"); }
void OP_SUB_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(sub) asm (".global _OP_SUB_Gd_Md_A32_END\n _OP_SUB_Gd_Md_A32_END:"); }
void OP_XOR_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(xor) asm (".global _OP_XOR_Gd_Md_A32_END\n _OP_XOR_Gd_Md_A32_END:"); }
void OP_CMP_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(cmp) asm (".global _OP_CMP_Gd_Md_A32_END\n _OP_CMP_Gd_Md_A32_END:"); }
void OP_BSF_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(bsf) asm (".global _OP_BSF_Gd_Md_A32_END\n _OP_BSF_Gd_Md_A32_END:"); }
void OP_BSR_Gd_Md_A32(void) { ALUOP_Gd_Md_A32(bsr) asm (".global _OP_BSR_Gd_Md_A32_END\n _OP_BSR_Gd_Md_A32_END:"); }

void OP_ADD_Rb_Ib(void) { ALUOP_Rb_Ib(add) asm (".global _OP_ADD_Rb_Ib_END\n _OP_ADD_Rb_Ib_END:"); }
void OP_OR_Rb_Ib(void) { ALUOP_Rb_Ib(or) asm (".global _OP_OR_Rb_Ib_END\n _OP_OR_Rb_Ib_END:"); }
void OP_ADC_Rb_Ib(void) { READFALUOP_Rb_Ib(adc) asm (".global _OP_ADC_Rb_Ib_END\n _OP_ADC_Rb_Ib_END:"); }
void OP_SBB_Rb_Ib(void) { READFALUOP_Rb_Ib(sbb) asm (".global _OP_SBB_Rb_Ib_END\n _OP_SBB_Rb_Ib_END:"); }
void OP_AND_Rb_Ib(void) { ALUOP_Rb_Ib(and) asm (".global _OP_AND_Rb_Ib_END\n _OP_AND_Rb_Ib_END:"); }
void OP_SUB_Rb_Ib(void) { ALUOP_Rb_Ib(sub) asm (".global _OP_SUB_Rb_Ib_END\n _OP_SUB_Rb_Ib_END:"); }
void OP_XOR_Rb_Ib(void) { ALUOP_Rb_Ib(xor) asm (".global _OP_XOR_Rb_Ib_END\n _OP_XOR_Rb_Ib_END:"); }
void OP_CMP_Rb_Ib(void) { ALUOP_Rb_Ib(cmp) asm (".global _OP_CMP_Rb_Ib_END\n _OP_CMP_Rb_Ib_END:"); }
void OP_TEST_Rb_Ib(void) { ALUOP_Rb_Ib(test) asm (".global _OP_TEST_Rb_Ib_END\n _OP_TEST_Rb_Ib_END:"); }

void OP_ADD_Rw_Iw(void) { ALUOP_Rw_Iw(add) asm (".global _OP_ADD_Rw_Iw_END\n _OP_ADD_Rw_Iw_END:"); }
void OP_OR_Rw_Iw(void) { ALUOP_Rw_Iw(or) asm (".global _OP_OR_Rw_Iw_END\n _OP_OR_Rw_Iw_END:"); }
void OP_ADC_Rw_Iw(void) { READFALUOP_Rw_Iw(adc) asm (".global _OP_ADC_Rw_Iw_END\n _OP_ADC_Rw_Iw_END:"); }
void OP_SBB_Rw_Iw(void) { READFALUOP_Rw_Iw(sbb) asm (".global _OP_SBB_Rw_Iw_END\n _OP_SBB_Rw_Iw_END:"); }
void OP_AND_Rw_Iw(void) { ALUOP_Rw_Iw(and) asm (".global _OP_AND_Rw_Iw_END\n _OP_AND_Rw_Iw_END:"); }
void OP_SUB_Rw_Iw(void) { ALUOP_Rw_Iw(sub) asm (".global _OP_SUB_Rw_Iw_END\n _OP_SUB_Rw_Iw_END:"); }
void OP_XOR_Rw_Iw(void) { ALUOP_Rw_Iw(xor) asm (".global _OP_XOR_Rw_Iw_END\n _OP_XOR_Rw_Iw_END:"); }
void OP_CMP_Rw_Iw(void) { ALUOP_Rw_Iw(cmp) asm (".global _OP_CMP_Rw_Iw_END\n _OP_CMP_Rw_Iw_END:"); }
void OP_TEST_Rw_Iw(void) { ALUOP_Rw_Iw(test) asm (".global _OP_TEST_Rw_Iw_END\n _OP_TEST_Rw_Iw_END:"); }

void OP_ADD_Rd_Id(void) { ALUOP_Rd_Id(add) asm (".global _OP_ADD_Rd_Id_END\n _OP_ADD_Rd_Id_END:"); }
void OP_OR_Rd_Id(void) { ALUOP_Rd_Id(or) asm (".global _OP_OR_Rd_Id_END\n _OP_OR_Rd_Id_END:"); }
void OP_ADC_Rd_Id(void) { READFALUOP_Rd_Id(adc) asm (".global _OP_ADC_Rd_Id_END\n _OP_ADC_Rd_Id_END:"); }
void OP_SBB_Rd_Id(void) { READFALUOP_Rd_Id(sbb) asm (".global _OP_SBB_Rd_Id_END\n _OP_SBB_Rd_Id_END:"); }
void OP_AND_Rd_Id(void) { ALUOP_Rd_Id(and) asm (".global _OP_AND_Rd_Id_END\n _OP_AND_Rd_Id_END:"); }
void OP_SUB_Rd_Id(void) { ALUOP_Rd_Id(sub) asm (".global _OP_SUB_Rd_Id_END\n _OP_SUB_Rd_Id_END:"); }
void OP_XOR_Rd_Id(void) { ALUOP_Rd_Id(xor) asm (".global _OP_XOR_Rd_Id_END\n _OP_XOR_Rd_Id_END:"); }
void OP_CMP_Rd_Id(void) { ALUOP_Rd_Id(cmp) asm (".global _OP_CMP_Rd_Id_END\n _OP_CMP_Rd_Id_END:"); }
void OP_TEST_Rd_Id(void) { ALUOP_Rd_Id(test) asm (".global _OP_TEST_Rd_Id_END\n _OP_TEST_Rd_Id_END:"); }

void OP_ADD_Rw_Ib(void) { ALUOP_Rw_Ib(add) asm (".global _OP_ADD_Rw_Ib_END\n _OP_ADD_Rw_Ib_END:"); }
void OP_OR_Rw_Ib(void) { ALUOP_Rw_Ib(or) asm (".global _OP_OR_Rw_Ib_END\n _OP_OR_Rw_Ib_END:"); }
void OP_ADC_Rw_Ib(void) { READFALUOP_Rw_Ib(adc) asm (".global _OP_ADC_Rw_Ib_END\n _OP_ADC_Rw_Ib_END:"); }
void OP_SBB_Rw_Ib(void) { READFALUOP_Rw_Ib(sbb) asm (".global _OP_SBB_Rw_Ib_END\n _OP_SBB_Rw_Ib_END:"); }
void OP_AND_Rw_Ib(void) { ALUOP_Rw_Ib(and) asm (".global _OP_AND_Rw_Ib_END\n _OP_AND_Rw_Ib_END:"); }
void OP_SUB_Rw_Ib(void) { ALUOP_Rw_Ib(sub) asm (".global _OP_SUB_Rw_Ib_END\n _OP_SUB_Rw_Ib_END:"); }
void OP_XOR_Rw_Ib(void) { ALUOP_Rw_Ib(xor) asm (".global _OP_XOR_Rw_Ib_END\n _OP_XOR_Rw_Ib_END:"); }
void OP_CMP_Rw_Ib(void) { ALUOP_Rw_Ib(cmp) asm (".global _OP_CMP_Rw_Ib_END\n _OP_CMP_Rw_Ib_END:"); }

void OP_ADD_Rd_Ib(void) { ALUOP_Rd_Ib(add) asm (".global _OP_ADD_Rd_Ib_END\n _OP_ADD_Rd_Ib_END:"); }
void OP_OR_Rd_Ib(void) { ALUOP_Rd_Ib(or) asm (".global _OP_OR_Rd_Ib_END\n _OP_OR_Rd_Ib_END:"); }
void OP_ADC_Rd_Ib(void) { READFALUOP_Rd_Ib(adc) asm (".global _OP_ADC_Rd_Ib_END\n _OP_ADC_Rd_Ib_END:"); }
void OP_SBB_Rd_Ib(void) { READFALUOP_Rd_Ib(sbb) asm (".global _OP_SBB_Rd_Ib_END\n _OP_SBB_Rd_Ib_END:"); }
void OP_AND_Rd_Ib(void) { ALUOP_Rd_Ib(and) asm (".global _OP_AND_Rd_Ib_END\n _OP_AND_Rd_Ib_END:"); }
void OP_SUB_Rd_Ib(void) { ALUOP_Rd_Ib(sub) asm (".global _OP_SUB_Rd_Ib_END\n _OP_SUB_Rd_Ib_END:"); }
void OP_XOR_Rd_Ib(void) { ALUOP_Rd_Ib(xor) asm (".global _OP_XOR_Rd_Ib_END\n _OP_XOR_Rd_Ib_END:"); }
void OP_CMP_Rd_Ib(void) { ALUOP_Rd_Ib(cmp) asm (".global _OP_CMP_Rd_Ib_END\n _OP_CMP_Rd_Ib_END:"); }

void OP_ADD_Mb_Ib_A16(void) { ALUOP_Mb_Ib_A16(add) asm (".global _OP_ADD_Mb_Ib_A16_END\n _OP_ADD_Mb_Ib_A16_END:"); }
void OP_OR_Mb_Ib_A16(void) { ALUOP_Mb_Ib_A16(or) asm (".global _OP_OR_Mb_Ib_A16_END\n _OP_OR_Mb_Ib_A16_END:"); }
void OP_ADC_Mb_Ib_A16(void) { READFALUOP_Mb_Ib_A16(adc) asm (".global _OP_ADC_Mb_Ib_A16_END\n _OP_ADC_Mb_Ib_A16_END:"); }
void OP_SBB_Mb_Ib_A16(void) { READFALUOP_Mb_Ib_A16(sbb) asm (".global _OP_SBB_Mb_Ib_A16_END\n _OP_SBB_Mb_Ib_A16_END:"); }
void OP_AND_Mb_Ib_A16(void) { ALUOP_Mb_Ib_A16(and) asm (".global _OP_AND_Mb_Ib_A16_END\n _OP_AND_Mb_Ib_A16_END:"); }
void OP_SUB_Mb_Ib_A16(void) { ALUOP_Mb_Ib_A16(sub) asm (".global _OP_SUB_Mb_Ib_A16_END\n _OP_SUB_Mb_Ib_A16_END:"); }
void OP_XOR_Mb_Ib_A16(void) { ALUOP_Mb_Ib_A16(xor) asm (".global _OP_XOR_Mb_Ib_A16_END\n _OP_XOR_Mb_Ib_A16_END:"); }
void OP_CMP_Mb_Ib_A16(void) { CMPALUOP_Mb_Ib_A16(cmp) asm (".global _OP_CMP_Mb_Ib_A16_END\n _OP_CMP_Mb_Ib_A16_END:"); }
void OP_TEST_Mb_Ib_A16(void) { CMPALUOP_Mb_Ib_A16(test) asm (".global _OP_TEST_Mb_Ib_A16_END\n _OP_TEST_Mb_Ib_A16_END:"); }

void OP_ADD_Mw_Iw_A16(void) { ALUOP_Mw_Iw_A16(add) asm (".global _OP_ADD_Mw_Iw_A16_END\n _OP_ADD_Mw_Iw_A16_END:"); }
void OP_OR_Mw_Iw_A16(void) { ALUOP_Mw_Iw_A16(or) asm (".global _OP_OR_Mw_Iw_A16_END\n _OP_OR_Mw_Iw_A16_END:"); }
void OP_ADC_Mw_Iw_A16(void) { READFALUOP_Mw_Iw_A16(adc) asm (".global _OP_ADC_Mw_Iw_A16_END\n _OP_ADC_Mw_Iw_A16_END:"); }
void OP_SBB_Mw_Iw_A16(void) { READFALUOP_Mw_Iw_A16(sbb) asm (".global _OP_SBB_Mw_Iw_A16_END\n _OP_SBB_Mw_Iw_A16_END:"); }
void OP_AND_Mw_Iw_A16(void) { ALUOP_Mw_Iw_A16(and) asm (".global _OP_AND_Mw_Iw_A16_END\n _OP_AND_Mw_Iw_A16_END:"); }
void OP_SUB_Mw_Iw_A16(void) { ALUOP_Mw_Iw_A16(sub) asm (".global _OP_SUB_Mw_Iw_A16_END\n _OP_SUB_Mw_Iw_A16_END:"); }
void OP_XOR_Mw_Iw_A16(void) { ALUOP_Mw_Iw_A16(xor) asm (".global _OP_XOR_Mw_Iw_A16_END\n _OP_XOR_Mw_Iw_A16_END:"); }
void OP_CMP_Mw_Iw_A16(void) { CMPALUOP_Mw_Iw_A16(cmp) asm (".global _OP_CMP_Mw_Iw_A16_END\n _OP_CMP_Mw_Iw_A16_END:"); }
void OP_TEST_Mw_Iw_A16(void) { CMPALUOP_Mw_Iw_A16(test) asm (".global _OP_TEST_Mw_Iw_A16_END\n _OP_TEST_Mw_Iw_A16_END:"); }

void OP_ADD_Md_Id_A16(void) { ALUOP_Md_Id_A16(add) asm (".global _OP_ADD_Md_Id_A16_END\n _OP_ADD_Md_Id_A16_END:"); }
void OP_OR_Md_Id_A16(void) { ALUOP_Md_Id_A16(or) asm (".global _OP_OR_Md_Id_A16_END\n _OP_OR_Md_Id_A16_END:"); }
void OP_ADC_Md_Id_A16(void) { READFALUOP_Md_Id_A16(adc) asm (".global _OP_ADC_Md_Id_A16_END\n _OP_ADC_Md_Id_A16_END:"); }
void OP_SBB_Md_Id_A16(void) { READFALUOP_Md_Id_A16(sbb) asm (".global _OP_SBB_Md_Id_A16_END\n _OP_SBB_Md_Id_A16_END:"); }
void OP_AND_Md_Id_A16(void) { ALUOP_Md_Id_A16(and) asm (".global _OP_AND_Md_Id_A16_END\n _OP_AND_Md_Id_A16_END:"); }
void OP_SUB_Md_Id_A16(void) { ALUOP_Md_Id_A16(sub) asm (".global _OP_SUB_Md_Id_A16_END\n _OP_SUB_Md_Id_A16_END:"); }
void OP_XOR_Md_Id_A16(void) { ALUOP_Md_Id_A16(xor) asm (".global _OP_XOR_Md_Id_A16_END\n _OP_XOR_Md_Id_A16_END:"); }
void OP_CMP_Md_Id_A16(void) { CMPALUOP_Md_Id_A16(cmp) asm (".global _OP_CMP_Md_Id_A16_END\n _OP_CMP_Md_Id_A16_END:"); }
void OP_TEST_Md_Id_A16(void) { CMPALUOP_Md_Id_A16(test) asm (".global _OP_TEST_Md_Id_A16_END\n _OP_TEST_Md_Id_A16_END:"); }

void OP_ADD_Mw_Ib_A16(void) { ALUOP_Mw_Ib_A16(add) asm (".global _OP_ADD_Mw_Ib_A16_END\n _OP_ADD_Mw_Ib_A16_END:"); }
void OP_OR_Mw_Ib_A16(void) { ALUOP_Mw_Ib_A16(or) asm (".global _OP_OR_Mw_Ib_A16_END\n _OP_OR_Mw_Ib_A16_END:"); }
void OP_ADC_Mw_Ib_A16(void) { READFALUOP_Mw_Ib_A16(adc) asm (".global _OP_ADC_Mw_Ib_A16_END\n _OP_ADC_Mw_Ib_A16_END:"); }
void OP_SBB_Mw_Ib_A16(void) { READFALUOP_Mw_Ib_A16(sbb) asm (".global _OP_SBB_Mw_Ib_A16_END\n _OP_SBB_Mw_Ib_A16_END:"); }
void OP_AND_Mw_Ib_A16(void) { ALUOP_Mw_Ib_A16(and) asm (".global _OP_AND_Mw_Ib_A16_END\n _OP_AND_Mw_Ib_A16_END:"); }
void OP_SUB_Mw_Ib_A16(void) { ALUOP_Mw_Ib_A16(sub) asm (".global _OP_SUB_Mw_Ib_A16_END\n _OP_SUB_Mw_Ib_A16_END:"); }
void OP_XOR_Mw_Ib_A16(void) { ALUOP_Mw_Ib_A16(xor) asm (".global _OP_XOR_Mw_Ib_A16_END\n _OP_XOR_Mw_Ib_A16_END:"); }
void OP_CMP_Mw_Ib_A16(void) { CMPALUOP_Mw_Ib_A16(cmp) asm (".global _OP_CMP_Mw_Ib_A16_END\n _OP_CMP_Mw_Ib_A16_END:"); }

void OP_ADD_Md_Ib_A16(void) { ALUOP_Md_Ib_A16(add) asm (".global _OP_ADD_Md_Ib_A16_END\n _OP_ADD_Md_Ib_A16_END:"); }
void OP_OR_Md_Ib_A16(void) { ALUOP_Md_Ib_A16(or) asm (".global _OP_OR_Md_Ib_A16_END\n _OP_OR_Md_Ib_A16_END:"); }
void OP_ADC_Md_Ib_A16(void) { READFALUOP_Md_Ib_A16(adc) asm (".global _OP_ADC_Md_Ib_A16_END\n _OP_ADC_Md_Ib_A16_END:"); }
void OP_SBB_Md_Ib_A16(void) { READFALUOP_Md_Ib_A16(sbb) asm (".global _OP_SBB_Md_Ib_A16_END\n _OP_SBB_Md_Ib_A16_END:"); }
void OP_AND_Md_Ib_A16(void) { ALUOP_Md_Ib_A16(and) asm (".global _OP_AND_Md_Ib_A16_END\n _OP_AND_Md_Ib_A16_END:"); }
void OP_SUB_Md_Ib_A16(void) { ALUOP_Md_Ib_A16(sub) asm (".global _OP_SUB_Md_Ib_A16_END\n _OP_SUB_Md_Ib_A16_END:"); }
void OP_XOR_Md_Ib_A16(void) { ALUOP_Md_Ib_A16(xor) asm (".global _OP_XOR_Md_Ib_A16_END\n _OP_XOR_Md_Ib_A16_END:"); }
void OP_CMP_Md_Ib_A16(void) { CMPALUOP_Md_Ib_A16(cmp) asm (".global _OP_CMP_Md_Ib_A16_END\n _OP_CMP_Md_Ib_A16_END:"); }

void OP_ADD_Mb_Ib_A32(void) { ALUOP_Mb_Ib_A32(add) asm (".global _OP_ADD_Mb_Ib_A32_END\n _OP_ADD_Mb_Ib_A32_END:"); }
void OP_OR_Mb_Ib_A32(void) { ALUOP_Mb_Ib_A32(or) asm (".global _OP_OR_Mb_Ib_A32_END\n _OP_OR_Mb_Ib_A32_END:"); }
void OP_ADC_Mb_Ib_A32(void) { READFALUOP_Mb_Ib_A32(adc) asm (".global _OP_ADC_Mb_Ib_A32_END\n _OP_ADC_Mb_Ib_A32_END:"); }
void OP_SBB_Mb_Ib_A32(void) { READFALUOP_Mb_Ib_A32(sbb) asm (".global _OP_SBB_Mb_Ib_A32_END\n _OP_SBB_Mb_Ib_A32_END:"); }
void OP_AND_Mb_Ib_A32(void) { ALUOP_Mb_Ib_A32(and) asm (".global _OP_AND_Mb_Ib_A32_END\n _OP_AND_Mb_Ib_A32_END:"); }
void OP_SUB_Mb_Ib_A32(void) { ALUOP_Mb_Ib_A32(sub) asm (".global _OP_SUB_Mb_Ib_A32_END\n _OP_SUB_Mb_Ib_A32_END:"); }
void OP_XOR_Mb_Ib_A32(void) { ALUOP_Mb_Ib_A32(xor) asm (".global _OP_XOR_Mb_Ib_A32_END\n _OP_XOR_Mb_Ib_A32_END:"); }
void OP_CMP_Mb_Ib_A32(void) { CMPALUOP_Mb_Ib_A32(cmp) asm (".global _OP_CMP_Mb_Ib_A32_END\n _OP_CMP_Mb_Ib_A32_END:"); }
void OP_TEST_Mb_Ib_A32(void) { CMPALUOP_Mb_Ib_A32(test) asm (".global _OP_TEST_Mb_Ib_A32_END\n _OP_TEST_Mb_Ib_A32_END:"); }

void OP_ADD_Mw_Iw_A32(void) { ALUOP_Mw_Iw_A32(add) asm (".global _OP_ADD_Mw_Iw_A32_END\n _OP_ADD_Mw_Iw_A32_END:"); }
void OP_OR_Mw_Iw_A32(void) { ALUOP_Mw_Iw_A32(or) asm (".global _OP_OR_Mw_Iw_A32_END\n _OP_OR_Mw_Iw_A32_END:"); }
void OP_ADC_Mw_Iw_A32(void) { READFALUOP_Mw_Iw_A32(adc) asm (".global _OP_ADC_Mw_Iw_A32_END\n _OP_ADC_Mw_Iw_A32_END:"); }
void OP_SBB_Mw_Iw_A32(void) { READFALUOP_Mw_Iw_A32(sbb) asm (".global _OP_SBB_Mw_Iw_A32_END\n _OP_SBB_Mw_Iw_A32_END:"); }
void OP_AND_Mw_Iw_A32(void) { ALUOP_Mw_Iw_A32(and) asm (".global _OP_AND_Mw_Iw_A32_END\n _OP_AND_Mw_Iw_A32_END:"); }
void OP_SUB_Mw_Iw_A32(void) { ALUOP_Mw_Iw_A32(sub) asm (".global _OP_SUB_Mw_Iw_A32_END\n _OP_SUB_Mw_Iw_A32_END:"); }
void OP_XOR_Mw_Iw_A32(void) { ALUOP_Mw_Iw_A32(xor) asm (".global _OP_XOR_Mw_Iw_A32_END\n _OP_XOR_Mw_Iw_A32_END:"); }
void OP_CMP_Mw_Iw_A32(void) { CMPALUOP_Mw_Iw_A32(cmp) asm (".global _OP_CMP_Mw_Iw_A32_END\n _OP_CMP_Mw_Iw_A32_END:"); }
void OP_TEST_Mw_Iw_A32(void) { CMPALUOP_Mw_Iw_A32(test) asm (".global _OP_TEST_Mw_Iw_A32_END\n _OP_TEST_Mw_Iw_A32_END:"); }

void OP_ADD_Md_Id_A32(void) { ALUOP_Md_Id_A32(add) asm (".global _OP_ADD_Md_Id_A32_END\n _OP_ADD_Md_Id_A32_END:"); }
void OP_OR_Md_Id_A32(void) { ALUOP_Md_Id_A32(or) asm (".global _OP_OR_Md_Id_A32_END\n _OP_OR_Md_Id_A32_END:"); }
void OP_ADC_Md_Id_A32(void) { READFALUOP_Md_Id_A32(adc) asm (".global _OP_ADC_Md_Id_A32_END\n _OP_ADC_Md_Id_A32_END:"); }
void OP_SBB_Md_Id_A32(void) { READFALUOP_Md_Id_A32(sbb) asm (".global _OP_SBB_Md_Id_A32_END\n _OP_SBB_Md_Id_A32_END:"); }
void OP_AND_Md_Id_A32(void) { ALUOP_Md_Id_A32(and) asm (".global _OP_AND_Md_Id_A32_END\n _OP_AND_Md_Id_A32_END:"); }
void OP_SUB_Md_Id_A32(void) { ALUOP_Md_Id_A32(sub) asm (".global _OP_SUB_Md_Id_A32_END\n _OP_SUB_Md_Id_A32_END:"); }
void OP_XOR_Md_Id_A32(void) { ALUOP_Md_Id_A32(xor) asm (".global _OP_XOR_Md_Id_A32_END\n _OP_XOR_Md_Id_A32_END:"); }
void OP_CMP_Md_Id_A32(void) { CMPALUOP_Md_Id_A32(cmp) asm (".global _OP_CMP_Md_Id_A32_END\n _OP_CMP_Md_Id_A32_END:"); }
void OP_TEST_Md_Id_A32(void) { CMPALUOP_Md_Id_A32(test) asm (".global _OP_TEST_Md_Id_A32_END\n _OP_TEST_Md_Id_A32_END:"); }

void OP_ADD_Mw_Ib_A32(void) { ALUOP_Mw_Ib_A32(add) asm (".global _OP_ADD_Mw_Ib_A32_END\n _OP_ADD_Mw_Ib_A32_END:"); }
void OP_OR_Mw_Ib_A32(void) { ALUOP_Mw_Ib_A32(or) asm (".global _OP_OR_Mw_Ib_A32_END\n _OP_OR_Mw_Ib_A32_END:"); }
void OP_ADC_Mw_Ib_A32(void) { READFALUOP_Mw_Ib_A32(adc) asm (".global _OP_ADC_Mw_Ib_A32_END\n _OP_ADC_Mw_Ib_A32_END:"); }
void OP_SBB_Mw_Ib_A32(void) { READFALUOP_Mw_Ib_A32(sbb) asm (".global _OP_SBB_Mw_Ib_A32_END\n _OP_SBB_Mw_Ib_A32_END:"); }
void OP_AND_Mw_Ib_A32(void) { ALUOP_Mw_Ib_A32(and) asm (".global _OP_AND_Mw_Ib_A32_END\n _OP_AND_Mw_Ib_A32_END:"); }
void OP_SUB_Mw_Ib_A32(void) { ALUOP_Mw_Ib_A32(sub) asm (".global _OP_SUB_Mw_Ib_A32_END\n _OP_SUB_Mw_Ib_A32_END:"); }
void OP_XOR_Mw_Ib_A32(void) { ALUOP_Mw_Ib_A32(xor) asm (".global _OP_XOR_Mw_Ib_A32_END\n _OP_XOR_Mw_Ib_A32_END:"); }
void OP_CMP_Mw_Ib_A32(void) { CMPALUOP_Mw_Ib_A32(cmp) asm (".global _OP_CMP_Mw_Ib_A32_END\n _OP_CMP_Mw_Ib_A32_END:"); }

void OP_ADD_Md_Ib_A32(void) { ALUOP_Md_Ib_A32(add) asm (".global _OP_ADD_Md_Ib_A32_END\n _OP_ADD_Md_Ib_A32_END:"); }
void OP_OR_Md_Ib_A32(void) { ALUOP_Md_Ib_A32(or) asm (".global _OP_OR_Md_Ib_A32_END\n _OP_OR_Md_Ib_A32_END:"); }
void OP_ADC_Md_Ib_A32(void) { READFALUOP_Md_Ib_A32(adc) asm (".global _OP_ADC_Md_Ib_A32_END\n _OP_ADC_Md_Ib_A32_END:"); }
void OP_SBB_Md_Ib_A32(void) { READFALUOP_Md_Ib_A32(sbb) asm (".global _OP_SBB_Md_Ib_A32_END\n _OP_SBB_Md_Ib_A32_END:"); }
void OP_AND_Md_Ib_A32(void) { ALUOP_Md_Ib_A32(and) asm (".global _OP_AND_Md_Ib_A32_END\n _OP_AND_Md_Ib_A32_END:"); }
void OP_SUB_Md_Ib_A32(void) { ALUOP_Md_Ib_A32(sub) asm (".global _OP_SUB_Md_Ib_A32_END\n _OP_SUB_Md_Ib_A32_END:"); }
void OP_XOR_Md_Ib_A32(void) { ALUOP_Md_Ib_A32(xor) asm (".global _OP_XOR_Md_Ib_A32_END\n _OP_XOR_Md_Ib_A32_END:"); }
void OP_CMP_Md_Ib_A32(void) { CMPALUOP_Md_Ib_A32(cmp) asm (".global _OP_CMP_Md_Ib_A32_END\n _OP_CMP_Md_Ib_A32_END:"); }

void OP_ADD_AL_Ib(void) { ALUOP_AL_Ib(add) asm (".global _OP_ADD_AL_Ib_END\n _OP_ADD_AL_Ib_END:"); }
void OP_OR_AL_Ib(void) { ALUOP_AL_Ib(or) asm (".global _OP_OR_AL_Ib_END\n _OP_OR_AL_Ib_END:"); }
void OP_ADC_AL_Ib(void) { READFALUOP_AL_Ib(adc) asm (".global _OP_ADC_AL_Ib_END\n _OP_ADC_AL_Ib_END:"); }
void OP_SBB_AL_Ib(void) { READFALUOP_AL_Ib(sbb) asm (".global _OP_SBB_AL_Ib_END\n _OP_SBB_AL_Ib_END:"); }
void OP_AND_AL_Ib(void) { ALUOP_AL_Ib(and) asm (".global _OP_AND_AL_Ib_END\n _OP_AND_AL_Ib_END:"); }
void OP_SUB_AL_Ib(void) { ALUOP_AL_Ib(sub) asm (".global _OP_SUB_AL_Ib_END\n _OP_SUB_AL_Ib_END:"); }
void OP_XOR_AL_Ib(void) { ALUOP_AL_Ib(xor) asm (".global _OP_XOR_AL_Ib_END\n _OP_XOR_AL_Ib_END:"); }
void OP_CMP_AL_Ib(void) { ALUOP_AL_Ib(cmp) asm (".global _OP_CMP_AL_Ib_END\n _OP_CMP_AL_Ib_END:"); }
void OP_TEST_AL_Ib(void) { ALUOP_AL_Ib(test) asm (".global _OP_TEST_AL_Ib_END\n _OP_TEST_AL_Ib_END:"); }

void OP_ADD_AX_Iw(void) { ALUOP_AX_Iw(add) asm (".global _OP_ADD_AX_Iw_END\n _OP_ADD_AX_Iw_END:"); }
void OP_OR_AX_Iw(void) { ALUOP_AX_Iw(or) asm (".global _OP_OR_AX_Iw_END\n _OP_OR_AX_Iw_END:"); }
void OP_ADC_AX_Iw(void) { READFALUOP_AX_Iw(adc) asm (".global _OP_ADC_AX_Iw_END\n _OP_ADC_AX_Iw_END:"); }
void OP_SBB_AX_Iw(void) { READFALUOP_AX_Iw(sbb) asm (".global _OP_SBB_AX_Iw_END\n _OP_SBB_AX_Iw_END:"); }
void OP_AND_AX_Iw(void) { ALUOP_AX_Iw(and) asm (".global _OP_AND_AX_Iw_END\n _OP_AND_AX_Iw_END:"); }
void OP_SUB_AX_Iw(void) { ALUOP_AX_Iw(sub) asm (".global _OP_SUB_AX_Iw_END\n _OP_SUB_AX_Iw_END:"); }
void OP_XOR_AX_Iw(void) { ALUOP_AX_Iw(xor) asm (".global _OP_XOR_AX_Iw_END\n _OP_XOR_AX_Iw_END:"); }
void OP_CMP_AX_Iw(void) { ALUOP_AX_Iw(cmp) asm (".global _OP_CMP_AX_Iw_END\n _OP_CMP_AX_Iw_END:"); }
void OP_TEST_AX_Iw(void) { ALUOP_AX_Iw(test) asm (".global _OP_TEST_AX_Iw_END\n _OP_TEST_AX_Iw_END:"); }

void OP_ADD_EAX_Id(void) { ALUOP_EAX_Id(add) asm (".global _OP_ADD_EAX_Id_END\n _OP_ADD_EAX_Id_END:"); }
void OP_OR_EAX_Id(void) { ALUOP_EAX_Id(or) asm (".global _OP_OR_EAX_Id_END\n _OP_OR_EAX_Id_END:"); }
void OP_ADC_EAX_Id(void) { READFALUOP_EAX_Id(adc) asm (".global _OP_ADC_EAX_Id_END\n _OP_ADC_EAX_Id_END:"); }
void OP_SBB_EAX_Id(void) { READFALUOP_EAX_Id(sbb) asm (".global _OP_SBB_EAX_Id_END\n _OP_SBB_EAX_Id_END:"); }
void OP_AND_EAX_Id(void) { ALUOP_EAX_Id(and) asm (".global _OP_AND_EAX_Id_END\n _OP_AND_EAX_Id_END:"); }
void OP_SUB_EAX_Id(void) { ALUOP_EAX_Id(sub) asm (".global _OP_SUB_EAX_Id_END\n _OP_SUB_EAX_Id_END:"); }
void OP_XOR_EAX_Id(void) { ALUOP_EAX_Id(xor) asm (".global _OP_XOR_EAX_Id_END\n _OP_XOR_EAX_Id_END:"); }
void OP_CMP_EAX_Id(void) { ALUOP_EAX_Id(cmp) asm (".global _OP_CMP_EAX_Id_END\n _OP_CMP_EAX_Id_END:"); }
void OP_TEST_EAX_Id(void) { ALUOP_EAX_Id(test) asm (".global _OP_TEST_EAX_Id_END\n _OP_TEST_EAX_Id_END:"); }

void OP_INC_AX(void) { INCDEC_Regw(inc, ax) asm (".global _OP_INC_AX_END\n _OP_INC_AX_END:"); }
void OP_INC_CX(void) { INCDEC_Regw(inc, cx) asm (".global _OP_INC_CX_END\n _OP_INC_CX_END:"); }
void OP_INC_DX(void) { INCDEC_Regw(inc, dx) asm (".global _OP_INC_DX_END\n _OP_INC_DX_END:"); }
void OP_INC_BX(void) { INCDEC_Regw(inc, bx) asm (".global _OP_INC_BX_END\n _OP_INC_BX_END:"); }
void OP_INC_SP(void) { INCDEC_Regw(inc, sp) asm (".global _OP_INC_SP_END\n _OP_INC_SP_END:"); }
void OP_INC_BP(void) { INCDEC_Regw(inc, bp) asm (".global _OP_INC_BP_END\n _OP_INC_BP_END:"); }
void OP_INC_SI(void) { INCDEC_Regw(inc, si) asm (".global _OP_INC_SI_END\n _OP_INC_SI_END:"); }
void OP_INC_DI(void) { INCDEC_Regw(inc, di) asm (".global _OP_INC_DI_END\n _OP_INC_DI_END:"); }

void OP_INC_EAX(void) { INCDEC_Regd(inc, eax) asm (".global _OP_INC_EAX_END\n _OP_INC_EAX_END:"); }
void OP_INC_ECX(void) { INCDEC_Regd(inc, ecx) asm (".global _OP_INC_ECX_END\n _OP_INC_ECX_END:"); }
void OP_INC_EDX(void) { INCDEC_Regd(inc, edx) asm (".global _OP_INC_EDX_END\n _OP_INC_EDX_END:"); }
void OP_INC_EBX(void) { INCDEC_Regd(inc, ebx) asm (".global _OP_INC_EBX_END\n _OP_INC_EBX_END:"); }
void OP_INC_ESP(void) { INCDEC_Regd(inc, esp) asm (".global _OP_INC_ESP_END\n _OP_INC_ESP_END:"); }
void OP_INC_EBP(void) { INCDEC_Regd(inc, ebp) asm (".global _OP_INC_EBP_END\n _OP_INC_EBP_END:"); }
void OP_INC_ESI(void) { INCDEC_Regd(inc, esi) asm (".global _OP_INC_ESI_END\n _OP_INC_ESI_END:"); }
void OP_INC_EDI(void) { INCDEC_Regd(inc, edi) asm (".global _OP_INC_EDI_END\n _OP_INC_EDI_END:"); }

void OP_DEC_AX(void) { INCDEC_Regw(dec, ax) asm (".global _OP_DEC_AX_END\n _OP_DEC_AX_END:"); }
void OP_DEC_CX(void) { INCDEC_Regw(dec, cx) asm (".global _OP_DEC_CX_END\n _OP_DEC_CX_END:"); }
void OP_DEC_DX(void) { INCDEC_Regw(dec, dx) asm (".global _OP_DEC_DX_END\n _OP_DEC_DX_END:"); }
void OP_DEC_BX(void) { INCDEC_Regw(dec, bx) asm (".global _OP_DEC_BX_END\n _OP_DEC_BX_END:"); }
void OP_DEC_SP(void) { INCDEC_Regw(dec, sp) asm (".global _OP_DEC_SP_END\n _OP_DEC_SP_END:"); }
void OP_DEC_BP(void) { INCDEC_Regw(dec, bp) asm (".global _OP_DEC_BP_END\n _OP_DEC_BP_END:"); }
void OP_DEC_SI(void) { INCDEC_Regw(dec, si) asm (".global _OP_DEC_SI_END\n _OP_DEC_SI_END:"); }
void OP_DEC_DI(void) { INCDEC_Regw(dec, di) asm (".global _OP_DEC_DI_END\n _OP_DEC_DI_END:"); }

void OP_DEC_EAX(void) { INCDEC_Regd(dec, eax) asm (".global _OP_DEC_EAX_END\n _OP_DEC_EAX_END:"); }
void OP_DEC_ECX(void) { INCDEC_Regd(dec, ecx) asm (".global _OP_DEC_ECX_END\n _OP_DEC_ECX_END:"); }
void OP_DEC_EDX(void) { INCDEC_Regd(dec, edx) asm (".global _OP_DEC_EDX_END\n _OP_DEC_EDX_END:"); }
void OP_DEC_EBX(void) { INCDEC_Regd(dec, ebx) asm (".global _OP_DEC_EBX_END\n _OP_DEC_EBX_END:"); }
void OP_DEC_ESP(void) { INCDEC_Regd(dec, esp) asm (".global _OP_DEC_ESP_END\n _OP_DEC_ESP_END:"); }
void OP_DEC_EBP(void) { INCDEC_Regd(dec, ebp) asm (".global _OP_DEC_EBP_END\n _OP_DEC_EBP_END:"); }
void OP_DEC_ESI(void) { INCDEC_Regd(dec, esi) asm (".global _OP_DEC_ESI_END\n _OP_DEC_ESI_END:"); }
void OP_DEC_EDI(void) { INCDEC_Regd(dec, edi) asm (".global _OP_DEC_EDI_END\n _OP_DEC_EDI_END:"); }

void OP_NOT_Rb(void) { UALUOP_Rb(not) asm (".global _OP_NOT_Rb_END\n _OP_NOT_Rb_END:"); }
void OP_NEG_Rb(void) { UALUOP_Rb(neg) asm (".global _OP_NEG_Rb_END\n _OP_NEG_Rb_END:"); }
void OP_INC_Rb(void) { UALUOP_Rb(inc) asm (".global _OP_INC_Rb_END\n _OP_INC_Rb_END:"); }
void OP_DEC_Rb(void) { UALUOP_Rb(dec) asm (".global _OP_DEC_Rb_END\n _OP_DEC_Rb_END:"); }

void OP_NOT_Rw(void) { UALUOP_Rw(not) asm (".global _OP_NOT_Rw_END\n _OP_NOT_Rw_END:"); }
void OP_NEG_Rw(void) { UALUOP_Rw(neg) asm (".global _OP_NEG_Rw_END\n _OP_NEG_Rw_END:"); }
void OP_INC_Rw(void) { UALUOP_Rw(inc) asm (".global _OP_INC_Rw_END\n _OP_INC_Rw_END:"); }
void OP_DEC_Rw(void) { UALUOP_Rw(dec) asm (".global _OP_DEC_Rw_END\n _OP_DEC_Rw_END:"); }

void OP_NOT_Rd(void) { UALUOP_Rd(not) asm (".global _OP_NOT_Rd_END\n _OP_NOT_Rd_END:"); }
void OP_NEG_Rd(void) { UALUOP_Rd(neg) asm (".global _OP_NEG_Rd_END\n _OP_NEG_Rd_END:"); }
void OP_INC_Rd(void) { UALUOP_Rd(inc) asm (".global _OP_INC_Rd_END\n _OP_INC_Rd_END:"); }
void OP_DEC_Rd(void) { UALUOP_Rd(dec) asm (".global _OP_DEC_Rd_END\n _OP_DEC_Rd_END:"); }

void OP_NOT_Mb_A16(void) { UALUOP_Mb_A16(not) asm (".global _OP_NOT_Mb_A16_END\n _OP_NOT_Mb_A16_END:"); }
void OP_NEG_Mb_A16(void) { UALUOP_Mb_A16(neg) asm (".global _OP_NEG_Mb_A16_END\n _OP_NEG_Mb_A16_END:"); }
void OP_INC_Mb_A16(void) { UALUOP_Mb_A16(inc) asm (".global _OP_INC_Mb_A16_END\n _OP_INC_Mb_A16_END:"); }
void OP_DEC_Mb_A16(void) { UALUOP_Mb_A16(dec) asm (".global _OP_DEC_Mb_A16_END\n _OP_DEC_Mb_A16_END:"); }

void OP_NOT_Mw_A16(void) { UALUOP_Mw_A16(not) asm (".global _OP_NOT_Mw_A16_END\n _OP_NOT_Mw_A16_END:"); }
void OP_NEG_Mw_A16(void) { UALUOP_Mw_A16(neg) asm (".global _OP_NEG_Mw_A16_END\n _OP_NEG_Mw_A16_END:"); }
void OP_INC_Mw_A16(void) { UALUOP_Mw_A16(inc) asm (".global _OP_INC_Mw_A16_END\n _OP_INC_Mw_A16_END:"); }
void OP_DEC_Mw_A16(void) { UALUOP_Mw_A16(dec) asm (".global _OP_DEC_Mw_A16_END\n _OP_DEC_Mw_A16_END:"); }

void OP_NOT_Md_A16(void) { UALUOP_Md_A16(not) asm (".global _OP_NOT_Md_A16_END\n _OP_NOT_Md_A16_END:"); }
void OP_NEG_Md_A16(void) { UALUOP_Md_A16(neg) asm (".global _OP_NEG_Md_A16_END\n _OP_NEG_Md_A16_END:"); }
void OP_INC_Md_A16(void) { UALUOP_Md_A16(inc) asm (".global _OP_INC_Md_A16_END\n _OP_INC_Md_A16_END:"); }
void OP_DEC_Md_A16(void) { UALUOP_Md_A16(dec) asm (".global _OP_DEC_Md_A16_END\n _OP_DEC_Md_A16_END:"); }

void OP_NOT_Mb_A32(void) { UALUOP_Mb_A32(not) asm (".global _OP_NOT_Mb_A32_END\n _OP_NOT_Mb_A32_END:"); }
void OP_NEG_Mb_A32(void) { UALUOP_Mb_A32(neg) asm (".global _OP_NEG_Mb_A32_END\n _OP_NEG_Mb_A32_END:"); }
void OP_INC_Mb_A32(void) { UALUOP_Mb_A32(inc) asm (".global _OP_INC_Mb_A32_END\n _OP_INC_Mb_A32_END:"); }
void OP_DEC_Mb_A32(void) { UALUOP_Mb_A32(dec) asm (".global _OP_DEC_Mb_A32_END\n _OP_DEC_Mb_A32_END:"); }

void OP_NOT_Mw_A32(void) { UALUOP_Mw_A32(not) asm (".global _OP_NOT_Mw_A32_END\n _OP_NOT_Mw_A32_END:"); }
void OP_NEG_Mw_A32(void) { UALUOP_Mw_A32(neg) asm (".global _OP_NEG_Mw_A32_END\n _OP_NEG_Mw_A32_END:"); }
void OP_INC_Mw_A32(void) { UALUOP_Mw_A32(inc) asm (".global _OP_INC_Mw_A32_END\n _OP_INC_Mw_A32_END:"); }
void OP_DEC_Mw_A32(void) { UALUOP_Mw_A32(dec) asm (".global _OP_DEC_Mw_A32_END\n _OP_DEC_Mw_A32_END:"); }

void OP_NOT_Md_A32(void) { UALUOP_Md_A32(not) asm (".global _OP_NOT_Md_A32_END\n _OP_NOT_Md_A32_END:"); }
void OP_NEG_Md_A32(void) { UALUOP_Md_A32(neg) asm (".global _OP_NEG_Md_A32_END\n _OP_NEG_Md_A32_END:"); }
void OP_INC_Md_A32(void) { UALUOP_Md_A32(inc) asm (".global _OP_INC_Md_A32_END\n _OP_INC_Md_A32_END:"); }
void OP_DEC_Md_A32(void) { UALUOP_Md_A32(dec) asm (".global _OP_DEC_Md_A32_END\n _OP_DEC_Md_A32_END:"); }

void OP_MUL_AL_Rb(void) { MULOP_AL_Rb(mul) asm (".global _OP_MUL_AL_Rb_END\n _OP_MUL_AL_Rb_END:"); }
void OP_IMUL_AL_Rb(void) { MULOP_AL_Rb(imul) asm (".global _OP_IMUL_AL_Rb_END\n _OP_IMUL_AL_Rb_END:"); }

void OP_MUL_AX_Rw(void) { MULOP_AX_Rw(mul) asm (".global _OP_MUL_AX_Rw_END\n _OP_MUL_AX_Rw_END:"); }
void OP_IMUL_AX_Rw(void) { MULOP_AX_Rw(imul) asm (".global _OP_IMUL_AX_Rw_END\n _OP_IMUL_AX_Rw_END:"); }

void OP_MUL_EAX_Rd(void) { MULOP_EAX_Rd(mul) asm (".global _OP_MUL_EAX_Rd_END\n _OP_MUL_EAX_Rd_END:"); }
void OP_IMUL_EAX_Rd(void) { MULOP_EAX_Rd(imul) asm (".global _OP_IMUL_EAX_Rd_END\n _OP_IMUL_EAX_Rd_END:"); }

void OP_MUL_AL_Mb_A16(void) { MULOP_AL_Mb_A16(mul) asm (".global _OP_MUL_AL_Mb_A16_END\n _OP_MUL_AL_Mb_A16_END:"); }
void OP_IMUL_AL_Mb_A16(void) { MULOP_AL_Mb_A16(imul) asm (".global _OP_IMUL_AL_Mb_A16_END\n _OP_IMUL_AL_Mb_A16_END:"); }

void OP_MUL_AX_Mw_A16(void) { MULOP_AX_Mw_A16(mul) asm (".global _OP_MUL_AX_Mw_A16_END\n _OP_MUL_AX_Mw_A16_END:"); }
void OP_IMUL_AX_Mw_A16(void) { MULOP_AX_Mw_A16(imul) asm (".global _OP_IMUL_AX_Mw_A16_END\n _OP_IMUL_AX_Mw_A16_END:"); }

void OP_MUL_EAX_Md_A16(void) { MULOP_EAX_Md_A16(mul) asm (".global _OP_MUL_EAX_Md_A16_END\n _OP_MUL_EAX_Md_A16_END:"); }
void OP_IMUL_EAX_Md_A16(void) { MULOP_EAX_Md_A16(imul) asm (".global _OP_IMUL_EAX_Md_A16_END\n _OP_IMUL_EAX_Md_A16_END:"); }

void OP_MUL_AL_Mb_A32(void) { MULOP_AL_Mb_A32(mul) asm (".global _OP_MUL_AL_Mb_A32_END\n _OP_MUL_AL_Mb_A32_END:"); }
void OP_IMUL_AL_Mb_A32(void) { MULOP_AL_Mb_A32(imul) asm (".global _OP_IMUL_AL_Mb_A32_END\n _OP_IMUL_AL_Mb_A32_END:"); }

void OP_MUL_AX_Mw_A32(void) { MULOP_AX_Mw_A32(mul) asm (".global _OP_MUL_AX_Mw_A32_END\n _OP_MUL_AX_Mw_A32_END:"); }
void OP_IMUL_AX_Mw_A32(void) { MULOP_AX_Mw_A32(imul) asm (".global _OP_IMUL_AX_Mw_A32_END\n _OP_IMUL_AX_Mw_A32_END:"); }

void OP_MUL_EAX_Md_A32(void) { MULOP_EAX_Md_A32(mul) asm (".global _OP_MUL_EAX_Md_A32_END\n _OP_MUL_EAX_Md_A32_END:"); }
void OP_IMUL_EAX_Md_A32(void) { MULOP_EAX_Md_A32(imul) asm (".global _OP_IMUL_EAX_Md_A32_END\n _OP_IMUL_EAX_Md_A32_END:"); }

void OP_IMUL_Gw_Rw_Iw(void)
{
	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (immWord), "1" (Rw)
	);

	asm (".global _OP_IMUL_Gw_Rw_Iw_END\n _OP_IMUL_Gw_Rw_Iw_END:");
}

void OP_IMUL_Gd_Rd_Id(void)
{
	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" (immDword), "1" (Rd)
	);

	asm (".global _OP_IMUL_Gd_Rd_Id_END\n _OP_IMUL_Gd_Rd_Id_END:");
}

void OP_IMUL_Gw_Mw_Iw_A16(void)
{
	Word source = ReadWord(CalculateAddressA16());

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (immWord), "1" (source)
	);

	asm (".global _OP_IMUL_Gw_Mw_Iw_A16_END\n _OP_IMUL_Gw_Mw_Iw_A16_END:");
}

void OP_IMUL_Gd_Md_Id_A16(void)
{
	Dword source = ReadDword(CalculateAddressA16());

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" (immDword), "1" (source)
	);

	asm (".global _OP_IMUL_Gd_Md_Id_A16_END\n _OP_IMUL_Gd_Md_Id_A16_END:");
}

void OP_IMUL_Gw_Mw_Iw_A32(void)
{
	Word source = ReadWord(CalculateAddressA32());

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (immWord), "1" (source)
	);

	asm (".global _OP_IMUL_Gw_Mw_Iw_A32_END\n _OP_IMUL_Gw_Mw_Iw_A32_END:");
}

void OP_IMUL_Gd_Md_Id_A32(void)
{
	Dword source = ReadDword(CalculateAddressA32());

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		:"r" (immDword), "1" (source)
	);

	asm (".global _OP_IMUL_Gd_Md_Id_A32_END\n _OP_IMUL_Gd_Md_Id_A32_END:");
}

void OP_IMUL_Gw_Rw_Ib(void)
{
	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" ((SignedWord)immByte), "1" (Rw)
	);

	asm (".global _OP_IMUL_Gw_Rw_Ib_END\n _OP_IMUL_Gw_Rw_Ib_END:");
}

void OP_IMUL_Gd_Rd_Ib(void)
{
	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" ((SignedDword)immByte), "1" (Rd)
	);

	asm (".global _OP_IMUL_Gd_Rd_Ib_END\n _OP_IMUL_Gd_Rd_Ib_END:");
}

void OP_IMUL_Gw_Mw_Ib_A16(void)
{
	Word source = ReadWord(CalculateAddressA16());

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" ((SignedWord)immByte), "1" (source)
	);

	asm (".global _OP_IMUL_Gw_Mw_Ib_A16_END\n _OP_IMUL_Gw_Mw_Ib_A16_END:");
}

void OP_IMUL_Gd_Md_Ib_A16(void)
{
	Dword source = ReadDword(CalculateAddressA16());

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" ((SignedDword)immByte), "1" (source)
	);

	asm (".global _OP_IMUL_Gd_Md_Ib_A16_END\n _OP_IMUL_Gd_Md_Ib_A16_END:");
}

void OP_IMUL_Gw_Mw_Ib_A32(void)
{
	Word source = ReadWord(CalculateAddressA32());

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" ((SignedWord)immByte), "1" (source)
	);

	asm (".global _OP_IMUL_Gw_Mw_Ib_A32_END\n _OP_IMUL_Gw_Mw_Ib_A32_END:");
}

void OP_IMUL_Gd_Md_Ib_A32(void)
{
	Dword source = ReadDword(CalculateAddressA32());

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" ((SignedDword)immByte), "1" (source)
	);

	asm (".global _OP_IMUL_Gd_Md_Ib_A32_END\n _OP_IMUL_Gd_Md_Ib_A32_END:");
}

void OP_IMUL_Gw_Rw(void)
{
	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (Rw), "1" (Gw)
	);

	asm (".global _OP_IMUL_Gw_Rw_END\n _OP_IMUL_Gw_Rw_END:");
}

void OP_IMUL_Gw_Mw_A16(void)
{
	Dword address = CalculateAddressA16();
	Word source = ReadWord(address);

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (source), "1" (Gw)
	);

	asm (".global _OP_IMUL_Gw_Mw_A16_END\n _OP_IMUL_Gw_Mw_A16_END:");
}

void OP_IMUL_Gw_Mw_A32(void)
{
	Dword address = CalculateAddressA32();
	Word source = ReadWord(address);

	asm volatile
	(
		"imulw %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gw)
		: "r" (source), "1" (Gw)
	);

	asm (".global _OP_IMUL_Gw_Mw_A32_END\n _OP_IMUL_Gw_Mw_A32_END:");
}

void OP_IMUL_Gd_Rd(void)
{
	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" (Rd), "1" (Gd)
	);

	asm (".global _OP_IMUL_Gd_Rd_END\n _OP_IMUL_Gd_Rd_END:");
}

void OP_IMUL_Gd_Md_A16(void)
{
	Dword address = CalculateAddressA16();
	Dword source = ReadDword(address);

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" (source), "1" (Gd)
	);

	asm (".global _OP_IMUL_Gd_Md_A16_END\n _OP_IMUL_Gd_Md_A16_END:");
}

void OP_IMUL_Gd_Md_A32(void)
{
	Dword address = CalculateAddressA32();
	Dword source = ReadDword(address);

	asm volatile
	(
		"imull %2, %1	\n"
		"pushfl			\n"
		"popl %0		\n"
		: "=r" (registers->r_eflags), "=r" (Gd)
		: "r" (source), "1" (Gd)
	);

	asm (".global _OP_IMUL_Gd_Md_A32_END\n _OP_IMUL_Gd_Md_A32_END:");
}

void OP_DIV_AX_Rb(void) { DIVOP_AX_Rb(div) asm (".global _OP_DIV_AX_Rb_END\n _OP_DIV_AX_Rb_END:"); }
void OP_IDIV_AX_Rb(void) { DIVOP_AX_Rb(idiv) asm (".global _OP_IDIV_AX_Rb_END\n _OP_IDIV_AX_Rb_END:"); }

void OP_DIV_DX_AX_Rw(void) { DIVOP_DX_AX_Rw(div) asm (".global _OP_DIV_DX_AX_Rw_END\n _OP_DIV_DX_AX_Rw_END:"); }
void OP_IDIV_DX_AX_Rw(void) { DIVOP_DX_AX_Rw(idiv) asm (".global _OP_IDIV_DX_AX_Rw_END\n _OP_IDIV_DX_AX_Rw_END:"); }

void OP_DIV_EDX_EAX_Rd(void) { DIVOP_EDX_EAX_Rd(div) asm (".global _OP_DIV_EDX_EAX_Rd_END\n _OP_DIV_EDX_EAX_Rd_END:"); }
void OP_IDIV_EDX_EAX_Rd(void) { DIVOP_EDX_EAX_Rd(idiv) asm (".global _OP_IDIV_EDX_EAX_Rd_END\n _OP_IDIV_EDX_EAX_Rd_END:"); }

void OP_DIV_AX_Mb_A16(void) { DIVOP_AX_Mb_A16(div) asm (".global _OP_DIV_AX_Mb_A16_END\n _OP_DIV_AX_Mb_A16_END:"); }
void OP_IDIV_AX_Mb_A16(void) { DIVOP_AX_Mb_A16(idiv) asm (".global _OP_IDIV_AX_Mb_A16_END\n _OP_IDIV_AX_Mb_A16_END:"); }

void OP_DIV_DX_AX_Mw_A16(void) { DIVOP_DX_AX_Mw_A16(div) asm (".global _OP_DIV_DX_AX_Mw_A16_END\n _OP_DIV_DX_AX_Mw_A16_END:"); }
void OP_IDIV_DX_AX_Mw_A16(void) { DIVOP_DX_AX_Mw_A16(idiv) asm (".global _OP_IDIV_DX_AX_Mw_A16_END\n _OP_IDIV_DX_AX_Mw_A16_END:"); }

void OP_DIV_EDX_EAX_Md_A16(void) { DIVOP_EDX_EAX_Md_A16(div) asm (".global _OP_DIV_EDX_EAX_Md_A16_END\n _OP_DIV_EDX_EAX_Md_A16_END:"); }
void OP_IDIV_EDX_EAX_Md_A16(void) { DIVOP_EDX_EAX_Md_A16(idiv) asm (".global _OP_IDIV_EDX_EAX_Md_A16_END\n _OP_IDIV_EDX_EAX_Md_A16_END:"); }

void OP_DIV_AX_Mb_A32(void) { DIVOP_AX_Mb_A32(div) asm (".global _OP_DIV_AX_Mb_A32_END\n _OP_DIV_AX_Mb_A32_END:"); }
void OP_IDIV_AX_Mb_A32(void) { DIVOP_AX_Mb_A32(idiv) asm (".global _OP_IDIV_AX_Mb_A32_END\n _OP_IDIV_AX_Mb_A32_END:"); }

void OP_DIV_DX_AX_Mw_A32(void) { DIVOP_DX_AX_Mw_A32(div) asm (".global _OP_DIV_DX_AX_Mw_A32_END\n _OP_DIV_DX_AX_Mw_A32_END:"); }
void OP_IDIV_DX_AX_Mw_A32(void) { DIVOP_DX_AX_Mw_A32(idiv) asm (".global _OP_IDIV_DX_AX_Mw_A32_END\n _OP_IDIV_DX_AX_Mw_A32_END:"); }

void OP_DIV_EDX_EAX_Md_A32(void) { DIVOP_EDX_EAX_Md_A32(div) asm (".global _OP_DIV_EDX_EAX_Md_A32_END\n _OP_DIV_EDX_EAX_Md_A32_END:"); }
void OP_IDIV_EDX_EAX_Md_A32(void) { DIVOP_EDX_EAX_Md_A32(idiv) asm (".global _OP_IDIV_EDX_EAX_Md_A32_END\n _OP_IDIV_EDX_EAX_Md_A32_END:"); }

void OP_CBW(void)
{
	registers->r_ax = (SignedByte)registers->r_al;
	asm (".global _OP_CBW_END\n _OP_CBW_END:");
}

void OP_CWDE(void)
{
	registers->r_eax = (SignedWord)registers->r_ax;
	asm (".global _OP_CWDE_END\n _OP_CWDE_END:");
}

void OP_CWD(void)
{
	registers->r_dx = (registers->r_ax & 0x8000) ? 0xffff : 0x0000;
	asm (".global _OP_CWD_END\n _OP_CWD_END:");
}

void OP_CDQ(void)
{
	registers->r_edx = (registers->r_eax & 0x80000000) ? 0xffffffff : 0x00000000;
	asm (".global _OP_CDQ_END\n _OP_CDQ_END:");
}
