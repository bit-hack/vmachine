//	VMachine
//	Implementations of FPU instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define FPUOP_16_A16(op)		\
	Word data = ReadWord(CalculateAddressA16());	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_16_A32(op)		\
	Word data = ReadWord(CalculateAddressA32());	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_32_A16(op)		\
	Dword data = ReadDword(CalculateAddressA16());	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_32_A32(op)		\
	Dword data = ReadDword(CalculateAddressA32());	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_64_A16(op)		\
	Dword address = CalculateAddressA16();	\
	Qword data = ReadDword(address);	\
	data |= (Qword)ReadDword(address + 4) << 32;	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_64_A32(op)		\
	Dword address = CalculateAddressA32();	\
	Qword data = ReadDword(address);	\
	data |= (Qword)ReadDword(address + 4) << 32;	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_80_A16(op)		\
	Dword address = CalculateAddressA16();	\
	DQword data;				\
	data.low = ReadDword(address);	\
	data.low |= (Qword)ReadDword(address + 4) << 32;	\
	data.high = ReadWord(address + 8);	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUOP_80_A32(op)		\
	Dword address = CalculateAddressA32();	\
	DQword data;				\
	data.low = ReadDword(address);	\
	data.low |= (Qword)ReadDword(address + 4) << 32;	\
	data.high = ReadWord(address + 8);	\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		:						\
		: "m" (data)			\
	);

#define FPUSTOREOP_16_A16(op)	\
	Word data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	WriteWord(CalculateAddressA16(), data);

#define FPUSTOREOP_16_A32(op)	\
	Word data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	WriteWord(CalculateAddressA32(), data);

#define FPUSTOREOP_32_A16(op)	\
	Dword data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	WriteDword(CalculateAddressA16(), data);

#define FPUSTOREOP_32_A32(op)	\
	Dword data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	WriteDword(CalculateAddressA32(), data);

#define FPUSTOREOP_64_A16(op)	\
	Qword data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	Dword address = CalculateAddressA16();	\
	WriteDword(address, (Dword)data);	\
	WriteDword(address + 4, (Dword)(data >> 32));

#define FPUSTOREOP_64_A32(op)	\
	Qword data;					\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	Dword address = CalculateAddressA32();	\
	WriteDword(address, (Dword)data);	\
	WriteDword(address + 4, (Dword)(data >> 32));

#define FPUSTOREOP_80_A16(op)	\
	DQword data;				\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	Dword address = CalculateAddressA16();	\
	WriteDword(address, (Dword)data.low);	\
	WriteDword(address + 4, (Dword)(data.low >> 32));	\
	WriteWord(address + 8, (Word)(data.high));

#define FPUSTOREOP_80_A32(op)	\
	DQword data;				\
	asm volatile				\
	(							\
		#op" %0		\n"			\
		: "=m" (data)			\
	);							\
	Dword address = CalculateAddressA32();	\
	WriteDword(address, (Dword)data.low);	\
	WriteDword(address + 4, (Dword)(data.low >> 32));	\
	WriteWord(address + 8, (Word)(data.high));

void OP_FIADD_WORD_A16(void) { FPUOP_16_A16(fiadds) asm (".global _OP_FIADD_WORD_A16_END\n _OP_FIADD_WORD_A16_END:"); }
void OP_FIMUL_WORD_A16(void) { FPUOP_16_A16(fimuls) asm (".global _OP_FIMUL_WORD_A16_END\n _OP_FIMUL_WORD_A16_END:"); }
void OP_FICOM_WORD_A16(void) { FPUOP_16_A16(ficoms) asm (".global _OP_FICOM_WORD_A16_END\n _OP_FICOM_WORD_A16_END:"); }
void OP_FICOMP_WORD_A16(void) { FPUOP_16_A16(ficomps) asm (".global _OP_FICOMP_WORD_A16_END\n _OP_FICOMP_WORD_A16_END:"); }
void OP_FISUB_WORD_A16(void) { FPUOP_16_A16(fisubs) asm (".global _OP_FISUB_WORD_A16_END\n _OP_FISUB_WORD_A16_END:"); }
void OP_FISUBR_WORD_A16(void) { FPUOP_16_A16(fisubrs) asm (".global _OP_FISUBR_WORD_A16_END\n _OP_FISUBR_WORD_A16_END:"); }
void OP_FIDIV_WORD_A16(void) { FPUOP_16_A16(fidivs) asm (".global _OP_FIDIV_WORD_A16_END\n _OP_FIDIV_WORD_A16_END:"); }
void OP_FIDIVR_WORD_A16(void) { FPUOP_16_A16(fidivrs) asm (".global _OP_FIDIVR_WORD_A16_END\n _OP_FIDIVR_WORD_A16_END:"); }
void OP_FILD_WORD_A16(void) { FPUOP_16_A16(filds) asm (".global _OP_FILD_WORD_A16_END\n _OP_FILD_WORD_A16_END:"); }

void OP_FIADD_DWORD_A16(void) { FPUOP_32_A16(fiaddl) asm (".global _OP_FIADD_DWORD_A16_END\n _OP_FIADD_DWORD_A16_END:"); }
void OP_FIMUL_DWORD_A16(void) { FPUOP_32_A16(fimull) asm (".global _OP_FIMUL_DWORD_A16_END\n _OP_FIMUL_DWORD_A16_END:"); }
void OP_FICOM_DWORD_A16(void) { FPUOP_32_A16(ficoml) asm (".global _OP_FICOM_DWORD_A16_END\n _OP_FICOM_DWORD_A16_END:"); }
void OP_FICOMP_DWORD_A16(void) { FPUOP_32_A16(ficompl) asm (".global _OP_FICOMP_DWORD_A16_END\n _OP_FICOMP_DWORD_A16_END:"); }
void OP_FISUB_DWORD_A16(void) { FPUOP_32_A16(fisubl) asm (".global _OP_FISUB_DWORD_A16_END\n _OP_FISUB_DWORD_A16_END:"); }
void OP_FISUBR_DWORD_A16(void) { FPUOP_32_A16(fisubrl) asm (".global _OP_FISUBR_DWORD_A16_END\n _OP_FISUBR_DWORD_A16_END:"); }
void OP_FIDIV_DWORD_A16(void) { FPUOP_32_A16(fidivl) asm (".global _OP_FIDIV_DWORD_A16_END\n _OP_FIDIV_DWORD_A16_END:"); }
void OP_FIDIVR_DWORD_A16(void) { FPUOP_32_A16(fidivrl) asm (".global _OP_FIDIVR_DWORD_A16_END\n _OP_FIDIVR_DWORD_A16_END:"); }
void OP_FILD_DWORD_A16(void) { FPUOP_32_A16(fildl) asm (".global _OP_FILD_DWORD_A16_END\n _OP_FILD_DWORD_A16_END:"); }

void OP_FILD_QWORD_A16(void) { FPUOP_64_A16(fildq) asm (".global _OP_FILD_QWORD_A16_END\n _OP_FILD_QWORD_A16_END:"); }

void OP_FADD_SINGLE_A16(void) { FPUOP_32_A16(fadds) asm (".global _OP_FADD_SINGLE_A16_END\n _OP_FADD_SINGLE_A16_END:"); }
void OP_FMUL_SINGLE_A16(void) { FPUOP_32_A16(fmuls) asm (".global _OP_FMUL_SINGLE_A16_END\n _OP_FMUL_SINGLE_A16_END:"); }
void OP_FCOM_SINGLE_A16(void) { FPUOP_32_A16(fcoms) asm (".global _OP_FCOM_SINGLE_A16_END\n _OP_FCOM_SINGLE_A16_END:"); }
void OP_FCOMP_SINGLE_A16(void) { FPUOP_32_A16(fcomps) asm (".global _OP_FCOMP_SINGLE_A16_END\n _OP_FCOMP_SINGLE_A16_END:"); }
void OP_FSUB_SINGLE_A16(void) { FPUOP_32_A16(fsubs) asm (".global _OP_FSUB_SINGLE_A16_END\n _OP_FSUB_SINGLE_A16_END:"); }
void OP_FSUBR_SINGLE_A16(void) { FPUOP_32_A16(fsubrs) asm (".global _OP_FSUBR_SINGLE_A16_END\n _OP_FSUBR_SINGLE_A16_END:"); }
void OP_FDIV_SINGLE_A16(void) { FPUOP_32_A16(fdivs) asm (".global _OP_FDIV_SINGLE_A16_END\n _OP_FDIV_SINGLE_A16_END:"); }
void OP_FDIVR_SINGLE_A16(void) { FPUOP_32_A16(fdivrs) asm (".global _OP_FDIVR_SINGLE_A16_END\n _OP_FDIVR_SINGLE_A16_END:"); }
void OP_FLD_SINGLE_A16(void) { FPUOP_32_A16(flds) asm (".global _OP_FLD_SINGLE_A16_END\n _OP_FLD_SINGLE_A16_END:"); }

void OP_FADD_DOUBLE_A16(void) { FPUOP_64_A16(faddl) asm (".global _OP_FADD_DOUBLE_A16_END\n _OP_FADD_DOUBLE_A16_END:"); }
void OP_FMUL_DOUBLE_A16(void) { FPUOP_64_A16(fmull) asm (".global _OP_FMUL_DOUBLE_A16_END\n _OP_FMUL_DOUBLE_A16_END:"); }
void OP_FCOM_DOUBLE_A16(void) { FPUOP_64_A16(fcoml) asm (".global _OP_FCOM_DOUBLE_A16_END\n _OP_FCOM_DOUBLE_A16_END:"); }
void OP_FCOMP_DOUBLE_A16(void) { FPUOP_64_A16(fcompl) asm (".global _OP_FCOMP_DOUBLE_A16_END\n _OP_FCOMP_DOUBLE_A16_END:"); }
void OP_FSUB_DOUBLE_A16(void) { FPUOP_64_A16(fsubl) asm (".global _OP_FSUB_DOUBLE_A16_END\n _OP_FSUB_DOUBLE_A16_END:"); }
void OP_FSUBR_DOUBLE_A16(void) { FPUOP_64_A16(fsubrl) asm (".global _OP_FSUBR_DOUBLE_A16_END\n _OP_FSUBR_DOUBLE_A16_END:"); }
void OP_FDIV_DOUBLE_A16(void) { FPUOP_64_A16(fdivl) asm (".global _OP_FDIV_DOUBLE_A16_END\n _OP_FDIV_DOUBLE_A16_END:"); }
void OP_FDIVR_DOUBLE_A16(void) { FPUOP_64_A16(fdivrl) asm (".global _OP_FDIVR_DOUBLE_A16_END\n _OP_FDIVR_DOUBLE_A16_END:"); }
void OP_FLD_DOUBLE_A16(void) { FPUOP_64_A16(fldl) asm (".global _OP_FLD_DOUBLE_A16_END\n _OP_FLD_DOUBLE_A16_END:"); }

void OP_FLD_EXTENDED_A16(void) { FPUOP_80_A16(fldt) asm (".global _OP_FLD_EXTENDED_A16_END\n _OP_FLD_EXTENDED_A16_END:"); }

void OP_FIADD_WORD_A32(void) { FPUOP_16_A32(fiadds) asm (".global _OP_FIADD_WORD_A32_END\n _OP_FIADD_WORD_A32_END:"); }
void OP_FIMUL_WORD_A32(void) { FPUOP_16_A32(fimuls) asm (".global _OP_FIMUL_WORD_A32_END\n _OP_FIMUL_WORD_A32_END:"); }
void OP_FICOM_WORD_A32(void) { FPUOP_16_A32(ficoms) asm (".global _OP_FICOM_WORD_A32_END\n _OP_FICOM_WORD_A32_END:"); }
void OP_FICOMP_WORD_A32(void) { FPUOP_16_A32(ficomps) asm (".global _OP_FICOMP_WORD_A32_END\n _OP_FICOMP_WORD_A32_END:"); }
void OP_FISUB_WORD_A32(void) { FPUOP_16_A32(fisubs) asm (".global _OP_FISUB_WORD_A32_END\n _OP_FISUB_WORD_A32_END:"); }
void OP_FISUBR_WORD_A32(void) { FPUOP_16_A32(fisubrs) asm (".global _OP_FISUBR_WORD_A32_END\n _OP_FISUBR_WORD_A32_END:"); }
void OP_FIDIV_WORD_A32(void) { FPUOP_16_A32(fidivs) asm (".global _OP_FIDIV_WORD_A32_END\n _OP_FIDIV_WORD_A32_END:"); }
void OP_FIDIVR_WORD_A32(void) { FPUOP_16_A32(fidivrs) asm (".global _OP_FIDIVR_WORD_A32_END\n _OP_FIDIVR_WORD_A32_END:"); }
void OP_FILD_WORD_A32(void) { FPUOP_16_A32(filds) asm (".global _OP_FILD_WORD_A32_END\n _OP_FILD_WORD_A32_END:"); }

void OP_FIADD_DWORD_A32(void) { FPUOP_32_A32(fiaddl) asm (".global _OP_FIADD_DWORD_A32_END\n _OP_FIADD_DWORD_A32_END:"); }
void OP_FIMUL_DWORD_A32(void) { FPUOP_32_A32(fimull) asm (".global _OP_FIMUL_DWORD_A32_END\n _OP_FIMUL_DWORD_A32_END:"); }
void OP_FICOM_DWORD_A32(void) { FPUOP_32_A32(ficoml) asm (".global _OP_FICOM_DWORD_A32_END\n _OP_FICOM_DWORD_A32_END:"); }
void OP_FICOMP_DWORD_A32(void) { FPUOP_32_A32(ficompl) asm (".global _OP_FICOMP_DWORD_A32_END\n _OP_FICOMP_DWORD_A32_END:"); }
void OP_FISUB_DWORD_A32(void) { FPUOP_32_A32(fisubl) asm (".global _OP_FISUB_DWORD_A32_END\n _OP_FISUB_DWORD_A32_END:"); }
void OP_FISUBR_DWORD_A32(void) { FPUOP_32_A32(fisubrl) asm (".global _OP_FISUBR_DWORD_A32_END\n _OP_FISUBR_DWORD_A32_END:"); }
void OP_FIDIV_DWORD_A32(void) { FPUOP_32_A32(fidivl) asm (".global _OP_FIDIV_DWORD_A32_END\n _OP_FIDIV_DWORD_A32_END:"); }
void OP_FIDIVR_DWORD_A32(void) { FPUOP_32_A32(fidivrl) asm (".global _OP_FIDIVR_DWORD_A32_END\n _OP_FIDIVR_DWORD_A32_END:"); }
void OP_FILD_DWORD_A32(void) { FPUOP_32_A32(fildl) asm (".global _OP_FILD_DWORD_A32_END\n _OP_FILD_DWORD_A32_END:"); }

void OP_FILD_QWORD_A32(void) { FPUOP_64_A32(fildq) asm (".global _OP_FILD_QWORD_A32_END\n _OP_FILD_QWORD_A32_END:"); }

void OP_FADD_SINGLE_A32(void) { FPUOP_32_A32(fadds) asm (".global _OP_FADD_SINGLE_A32_END\n _OP_FADD_SINGLE_A32_END:"); }
void OP_FMUL_SINGLE_A32(void) { FPUOP_32_A32(fmuls) asm (".global _OP_FMUL_SINGLE_A32_END\n _OP_FMUL_SINGLE_A32_END:"); }
void OP_FCOM_SINGLE_A32(void) { FPUOP_32_A32(fcoms) asm (".global _OP_FCOM_SINGLE_A32_END\n _OP_FCOM_SINGLE_A32_END:"); }
void OP_FCOMP_SINGLE_A32(void) { FPUOP_32_A32(fcomps) asm (".global _OP_FCOMP_SINGLE_A32_END\n _OP_FCOMP_SINGLE_A32_END:"); }
void OP_FSUB_SINGLE_A32(void) { FPUOP_32_A32(fsubs) asm (".global _OP_FSUB_SINGLE_A32_END\n _OP_FSUB_SINGLE_A32_END:"); }
void OP_FSUBR_SINGLE_A32(void) { FPUOP_32_A32(fsubrs) asm (".global _OP_FSUBR_SINGLE_A32_END\n _OP_FSUBR_SINGLE_A32_END:"); }
void OP_FDIV_SINGLE_A32(void) { FPUOP_32_A32(fdivs) asm (".global _OP_FDIV_SINGLE_A32_END\n _OP_FDIV_SINGLE_A32_END:"); }
void OP_FDIVR_SINGLE_A32(void) { FPUOP_32_A32(fdivrs) asm (".global _OP_FDIVR_SINGLE_A32_END\n _OP_FDIVR_SINGLE_A32_END:"); }
void OP_FLD_SINGLE_A32(void) { FPUOP_32_A32(flds) asm (".global _OP_FLD_SINGLE_A32_END\n _OP_FLD_SINGLE_A32_END:"); }

void OP_FADD_DOUBLE_A32(void) { FPUOP_64_A32(faddl) asm (".global _OP_FADD_DOUBLE_A32_END\n _OP_FADD_DOUBLE_A32_END:"); }
void OP_FMUL_DOUBLE_A32(void) { FPUOP_64_A32(fmull) asm (".global _OP_FMUL_DOUBLE_A32_END\n _OP_FMUL_DOUBLE_A32_END:"); }
void OP_FCOM_DOUBLE_A32(void) { FPUOP_64_A32(fcoml) asm (".global _OP_FCOM_DOUBLE_A32_END\n _OP_FCOM_DOUBLE_A32_END:"); }
void OP_FCOMP_DOUBLE_A32(void) { FPUOP_64_A32(fcompl) asm (".global _OP_FCOMP_DOUBLE_A32_END\n _OP_FCOMP_DOUBLE_A32_END:"); }
void OP_FSUB_DOUBLE_A32(void) { FPUOP_64_A32(fsubl) asm (".global _OP_FSUB_DOUBLE_A32_END\n _OP_FSUB_DOUBLE_A32_END:"); }
void OP_FSUBR_DOUBLE_A32(void) { FPUOP_64_A32(fsubrl) asm (".global _OP_FSUBR_DOUBLE_A32_END\n _OP_FSUBR_DOUBLE_A32_END:"); }
void OP_FDIV_DOUBLE_A32(void) { FPUOP_64_A32(fdivl) asm (".global _OP_FDIV_DOUBLE_A32_END\n _OP_FDIV_DOUBLE_A32_END:"); }
void OP_FDIVR_DOUBLE_A32(void) { FPUOP_64_A32(fdivrl) asm (".global _OP_FDIVR_DOUBLE_A32_END\n _OP_FDIVR_DOUBLE_A32_END:"); }
void OP_FLD_DOUBLE_A32(void) { FPUOP_64_A32(fldl) asm (".global _OP_FLD_DOUBLE_A32_END\n _OP_FLD_DOUBLE_A32_END:"); }

void OP_FLD_EXTENDED_A32(void) { FPUOP_80_A32(fldt) asm (".global _OP_FLD_EXTENDED_A32_END\n _OP_FLD_EXTENDED_A32_END:"); }

void OP_FIST_WORD_A16(void) { FPUSTOREOP_16_A16(fists) asm (".global _OP_FIST_WORD_A16_END\n _OP_FIST_WORD_A16_END:"); }
void OP_FISTP_WORD_A16(void) { FPUSTOREOP_16_A16(fistps) asm (".global _OP_FISTP_WORD_A16_END\n _OP_FISTP_WORD_A16_END:"); }
void OP_FISTTP_WORD_A16(void) { FPUSTOREOP_16_A16(fisttps) asm (".global _OP_FISTTP_WORD_A16_END\n _OP_FISTTP_WORD_A16_END:"); }

void OP_FIST_DWORD_A16(void) { FPUSTOREOP_32_A16(fistl) asm (".global _OP_FIST_DWORD_A16_END\n _OP_FIST_DWORD_A16_END:"); }
void OP_FISTP_DWORD_A16(void) { FPUSTOREOP_32_A16(fistpl) asm (".global _OP_FISTP_DWORD_A16_END\n _OP_FISTP_DWORD_A16_END:"); }
void OP_FISTTP_DWORD_A16(void) { FPUSTOREOP_32_A16(fisttpl) asm (".global _OP_FISTTP_DWORD_A16_END\n _OP_FISTTP_DWORD_A16_END:"); }

void OP_FISTP_QWORD_A16(void) { FPUSTOREOP_64_A16(fistpq) asm (".global _OP_FISTP_QWORD_A16_END\n _OP_FISTP_QWORD_A16_END:"); }
void OP_FISTTP_QWORD_A16(void) { FPUSTOREOP_64_A16(fisttpq) asm (".global _OP_FISTTP_QWORD_A16_END\n _OP_FISTTP_QWORD_A16_END:"); }

void OP_FST_SINGLE_A16(void) { FPUSTOREOP_32_A16(fsts) asm (".global _OP_FST_SINGLE_A16_END\n _OP_FST_SINGLE_A16_END:"); }
void OP_FSTP_SINGLE_A16(void) { FPUSTOREOP_32_A16(fstps) asm (".global _OP_FSTP_SINGLE_A16_END\n _OP_FSTP_SINGLE_A16_END:"); }

void OP_FST_DOUBLE_A16(void) { FPUSTOREOP_64_A16(fstl) asm (".global _OP_FST_DOUBLE_A16_END\n _OP_FST_DOUBLE_A16_END:"); }
void OP_FSTP_DOUBLE_A16(void) { FPUSTOREOP_64_A16(fstpl) asm (".global _OP_FSTP_DOUBLE_A16_END\n _OP_FSTP_DOUBLE_A16_END:"); }

void OP_FSTP_EXTENDED_A16(void) { FPUSTOREOP_80_A16(fstpt) asm (".global _OP_FSTP_EXTENDED_A16_END\n _OP_FSTP_EXTENDED_A16_END:"); }

void OP_FIST_WORD_A32(void) { FPUSTOREOP_16_A32(fists) asm (".global _OP_FIST_WORD_A32_END\n _OP_FIST_WORD_A32_END:"); }
void OP_FISTP_WORD_A32(void) { FPUSTOREOP_16_A32(fistps) asm (".global _OP_FISTP_WORD_A32_END\n _OP_FISTP_WORD_A32_END:"); }
void OP_FISTTP_WORD_A32(void) { FPUSTOREOP_16_A32(fisttps) asm (".global _OP_FISTTP_WORD_A32_END\n _OP_FISTTP_WORD_A32_END:"); }

void OP_FIST_DWORD_A32(void) { FPUSTOREOP_32_A32(fistl) asm (".global _OP_FIST_DWORD_A32_END\n _OP_FIST_DWORD_A32_END:"); }
void OP_FISTP_DWORD_A32(void) { FPUSTOREOP_32_A32(fistpl) asm (".global _OP_FISTP_DWORD_A32_END\n _OP_FISTP_DWORD_A32_END:"); }
void OP_FISTTP_DWORD_A32(void) { FPUSTOREOP_32_A32(fisttpl) asm (".global _OP_FISTTP_DWORD_A32_END\n _OP_FISTTP_DWORD_A32_END:"); }

void OP_FISTP_QWORD_A32(void) { FPUSTOREOP_64_A32(fistpq) asm (".global _OP_FISTP_QWORD_A32_END\n _OP_FISTP_QWORD_A32_END:"); }
void OP_FISTTP_QWORD_A32(void) { FPUSTOREOP_64_A32(fisttpq) asm (".global _OP_FISTTP_QWORD_A32_END\n _OP_FISTTP_QWORD_A32_END:"); }

void OP_FST_SINGLE_A32(void) { FPUSTOREOP_32_A32(fsts) asm (".global _OP_FST_SINGLE_A32_END\n _OP_FST_SINGLE_A32_END:"); }
void OP_FSTP_SINGLE_A32(void) { FPUSTOREOP_32_A32(fstps) asm (".global _OP_FSTP_SINGLE_A32_END\n _OP_FSTP_SINGLE_A32_END:"); }

void OP_FST_DOUBLE_A32(void) { FPUSTOREOP_64_A32(fstl) asm (".global _OP_FST_DOUBLE_A32_END\n _OP_FST_DOUBLE_A32_END:"); }
void OP_FSTP_DOUBLE_A32(void) { FPUSTOREOP_64_A32(fstpl) asm (".global _OP_FSTP_DOUBLE_A32_END\n _OP_FSTP_DOUBLE_A32_END:"); }

void OP_FSTP_EXTENDED_A32(void) { FPUSTOREOP_80_A32(fstpt) asm (".global _OP_FSTP_EXTENDED_A32_END\n _OP_FSTP_EXTENDED_A32_END:"); }

void OP_WAIT(void)
{
	asm volatile
	(
		"wait		\n"
	);

	asm (".global _OP_WAIT_END\n _OP_WAIT_END:");
}

void OP_FLDCW_A16(void)
{
	Word controlWord = ReadWord(CalculateAddressA16());

	asm volatile
	(
		"fldcw %0		\n"
		: 
		: "m" (controlWord)
	);

	asm (".global _OP_FLDCW_A16_END\n _OP_FLDCW_A16_END:");
}

void OP_FLDCW_A32(void)
{
	Word controlWord = ReadWord(CalculateAddressA32());

	asm volatile
	(
		"fldcw %0		\n"
		: 
		: "m" (controlWord)
	);

	asm (".global _OP_FLDCW_A32_END\n _OP_FLDCW_A32_END:");
}

void OP_FNSTCW_A16(void)
{
	Word controlWord;

	asm volatile
	(
		"fnstcw %0		\n"
		: "=m" (controlWord)
	);

	WriteWord(CalculateAddressA16(), controlWord);

	asm (".global _OP_FNSTCW_A16_END\n _OP_FNSTCW_A16_END:");
}

void OP_FNSTCW_A32(void)
{
	Word controlWord;

	asm volatile
	(
		"fnstcw %0		\n"
		: "=m" (controlWord)
	);

	WriteWord(CalculateAddressA32(), controlWord);

	asm (".global _OP_FNSTCW_A32_END\n _OP_FNSTCW_A32_END:");
}

void OP_FNSTSW_A16(void)
{
	Word statusWord;

	asm volatile
	(
		"fnstsw %0		\n"
		: "=m" (statusWord)
	);

	WriteWord(CalculateAddressA16(), statusWord);

	asm (".global _OP_FNSTSW_A16_END\n _OP_FNSTSW_A16_END:");
}

void OP_FNSTSW_A32(void)
{
	Word statusWord;

	asm volatile
	(
		"fnstsw %0		\n"
		: "=m" (statusWord)
	);

	WriteWord(CalculateAddressA32(), statusWord);

	asm (".global _OP_FNSTSW_A32_END\n _OP_FNSTSW_A32_END:");
}

void OP_FNSTENV_A16O16(void)
{
	Dword env[7];

	asm volatile
	(
		"fnstenv (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, ((env[3] & 0x000f0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, (env[5] & 0x000f0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, env[4]);
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, env[6]);
	}

	asm (".global _OP_FNSTENV_A16O16_END\n _OP_FNSTENV_A16O16_END:");
}

void OP_FNSTENV_A32O16(void)
{
	Dword env[7];

	asm volatile
	(
		"fnstenv (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, ((env[3] & 0x000f0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, (env[5] & 0x000f0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, env[4]);
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, env[6]);
	}

	asm (".global _OP_FNSTENV_A32O16_END\n _OP_FNSTENV_A32O16_END:");
}

void OP_FNSTENV_A16O32(void)
{
	Dword env[7];

	asm volatile
	(
		"fnstenv (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3] & 0x0000ffff);
		WriteDword(address + 16, ((env[3] & 0xffff0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteDword(address + 20, env[5] & 0x0000ffff);
		WriteDword(address + 24, (env[5] & 0xffff0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3]);
		WriteDword(address + 16, env[4]);
		WriteDword(address + 20, env[5]);
		WriteDword(address + 24, env[6]);
	}

	asm (".global _OP_FNSTENV_A16O32_END\n _OP_FNSTENV_A16O32_END:");
}

void OP_FNSTENV_A32O32(void)
{
	Dword env[7];

	asm volatile
	(
		"fnstenv (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3] & 0x0000ffff);
		WriteDword(address + 16, ((env[3] & 0xffff0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteDword(address + 20, env[5] & 0x0000ffff);
		WriteDword(address + 24, (env[5] & 0xffff0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3]);
		WriteDword(address + 16, env[4]);
		WriteDword(address + 20, env[5]);
		WriteDword(address + 24, env[6]);
	}

	asm (".global _OP_FNSTENV_A32O32_END\n _OP_FNSTENV_A32O32_END:");
}

void OP_FLDENV_A16O16(void)
{
	Dword env[7];

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6) | ((Dword)(ReadWord(address + 8) & 0xf000) << 4);
		env[4] = (Dword)(ReadWord(address + 8) & 0x07ff) << 16;
		env[5] = ReadWord(address + 10) | ((Dword)(ReadWord(address + 12) & 0xf000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6);
		env[4] = ReadWord(address + 8);
		env[5] = ReadWord(address + 10);
		env[6] = ReadWord(address + 12);
	}

	asm volatile
	(
		"fldenv (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FLDENV_A16O16_END\n _OP_FLDENV_A16O16_END:");
}

void OP_FLDENV_A32O16(void)
{
	Dword env[7];

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6) | ((Dword)(ReadWord(address + 8) & 0xf000) << 4);
		env[4] = (Dword)(ReadWord(address + 8) & 0x07ff) << 16;
		env[5] = ReadWord(address + 10) | ((Dword)(ReadWord(address + 12) & 0xf000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6);
		env[4] = ReadWord(address + 8);
		env[5] = ReadWord(address + 10);
		env[6] = ReadWord(address + 12);
	}

	asm volatile
	(
		"fldenv (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FLDENV_A32O16_END\n _OP_FLDENV_A32O16_END:");
}

void OP_FLDENV_A16O32(void)
{
	Dword env[7];

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = (ReadDword(address + 12) & 0x0000ffff) |
					((ReadDword(address + 16) & 0x0ffff000) << 4);
		env[4] = (ReadDword(address + 16) & 0x000007ff) << 16;
		env[5] = (ReadDword(address + 20) & 0x0000ffff) |
					((ReadDword(address + 24) & 0x0ffff000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = ReadDword(address + 12);
		env[4] = ReadDword(address + 16);
		env[5] = ReadDword(address + 20);
		env[6] = ReadDword(address + 24);
	}

	asm volatile
	(
		"fldenv (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FLDENV_A16O32_END\n _OP_FLDENV_A16O32_END:");
}

void OP_FLDENV_A32O32(void)
{
	Dword env[7];

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = (ReadDword(address + 12) & 0x0000ffff) |
					((ReadDword(address + 16) & 0x0ffff000) << 4);
		env[4] = (ReadDword(address + 16) & 0x000007ff) << 16;
		env[5] = (ReadDword(address + 20) & 0x0000ffff) |
					((ReadDword(address + 24) & 0x0ffff000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = ReadDword(address + 12);
		env[4] = ReadDword(address + 16);
		env[5] = ReadDword(address + 20);
		env[6] = ReadDword(address + 24);
	}

	asm volatile
	(
		"fldenv (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FLDENV_A32O32_END\n _OP_FLDENV_A32O32_END:");
}

void OP_FNSAVE_A16O16(void)
{
	Dword env[27];

	asm volatile
	(
		"fnsave (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, ((env[3] & 0x000f0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, (env[5] & 0x000f0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, env[4]);
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, env[6]);
	}

	for(Dword i = 0; i < 20; ++i)
		WriteDword(address + 14 + (i * 4), env[7 + i]);

	asm (".global _OP_FNSAVE_A16O16_END\n _OP_FNSAVE_A16O16_END:");
}

void OP_FNSAVE_A32O16(void)
{
	Dword env[27];

	asm volatile
	(
		"fnsave (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, ((env[3] & 0x000f0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, (env[5] & 0x000f0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteWord(address, env[0]);
		WriteWord(address + 2, env[1]);
		WriteWord(address + 4, env[2]);
		WriteWord(address + 6, env[3]);
		WriteWord(address + 8, env[4]);
		WriteWord(address + 10, env[5]);
		WriteWord(address + 12, env[6]);
	}

	for(Dword i = 0; i < 20; ++i)
		WriteDword(address + 14 + (i * 4), env[7 + i]);

	asm (".global _OP_FNSAVE_A32O16_END\n _OP_FNSAVE_A32O16_END:");
}

void OP_FNSAVE_A16O32(void)
{
	Dword env[27];

	asm volatile
	(
		"fnsave (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3] & 0x0000ffff);
		WriteDword(address + 16, ((env[3] & 0xffff0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteDword(address + 20, env[5] & 0x0000ffff);
		WriteDword(address + 24, (env[5] & 0xffff0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3]);
		WriteDword(address + 16, env[4]);
		WriteDword(address + 20, env[5]);
		WriteDword(address + 24, env[6]);
	}

	for(Dword i = 0; i < 20; ++i)
		WriteDword(address + 28 + (i * 4), env[7 + i]);

	asm (".global _OP_FNSAVE_A16O32_END\n _OP_FNSAVE_A16O32_END:");
}

void OP_FNSAVE_A32O32(void)
{
	Dword env[27];

	asm volatile
	(
		"fnsave (%0)		\n"
		:
		: "r" (env)
	);

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3] & 0x0000ffff);
		WriteDword(address + 16, ((env[3] & 0xffff0000) >> 4) | ((env[4] & 0x07ff000) >> 16));
		WriteDword(address + 20, env[5] & 0x0000ffff);
		WriteDword(address + 24, (env[5] & 0xffff0000) >> 4);
	}
	else
	{
		//Protected mode
		WriteDword(address, env[0]);
		WriteDword(address + 4, env[1]);
		WriteDword(address + 8, env[2]);
		WriteDword(address + 12, env[3]);
		WriteDword(address + 16, env[4]);
		WriteDword(address + 20, env[5]);
		WriteDword(address + 24, env[6]);
	}

	for(Dword i = 0; i < 20; ++i)
		WriteDword(address + 28 + (i * 4), env[7 + i]);

	asm (".global _OP_FNSAVE_A32O32_END\n _OP_FNSAVE_A32O32_END:");
}

void OP_FRSTOR_A16O16(void)
{
	Dword env[27];

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6) | ((Dword)(ReadWord(address + 8) & 0xf000) << 4);
		env[4] = (Dword)(ReadWord(address + 8) & 0x07ff) << 16;
		env[5] = ReadWord(address + 10) | ((Dword)(ReadWord(address + 12) & 0xf000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6);
		env[4] = ReadWord(address + 8);
		env[5] = ReadWord(address + 10);
		env[6] = ReadWord(address + 12);
	}

	for(Dword i = 0; i < 20; ++i)
		env[7 + i] = ReadDword(address + 14 + (i * 4));

	asm volatile
	(
		"frstor (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FRSTOR_A16O16_END\n _OP_FRSTOR_A16O16_END:");
}

void OP_FRSTOR_A32O16(void)
{
	Dword env[27];

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6) | ((Dword)(ReadWord(address + 8) & 0xf000) << 4);
		env[4] = (Dword)(ReadWord(address + 8) & 0x07ff) << 16;
		env[5] = ReadWord(address + 10) | ((Dword)(ReadWord(address + 12) & 0xf000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadWord(address);
		env[1] = ReadWord(address + 2);
		env[2] = ReadWord(address + 4);
		env[3] = ReadWord(address + 6);
		env[4] = ReadWord(address + 8);
		env[5] = ReadWord(address + 10);
		env[6] = ReadWord(address + 12);
	}

	for(Dword i = 0; i < 20; ++i)
		env[7 + i] = ReadDword(address + 14 + (i * 4));

	asm volatile
	(
		"frstor (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FRSTOR_A32O16_END\n _OP_FRSTOR_A32O16_END:");
}

void OP_FRSTOR_A16O32(void)
{
	Dword env[27];

	Dword address = CalculateAddressA16();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = (ReadDword(address + 12) & 0x0000ffff) |
					((ReadDword(address + 16) & 0x0ffff000) << 4);
		env[4] = (ReadDword(address + 16) & 0x000007ff) << 16;
		env[5] = (ReadDword(address + 20) & 0x0000ffff) |
					((ReadDword(address + 24) & 0x0ffff000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = ReadDword(address + 12);
		env[4] = ReadDword(address + 16);
		env[5] = ReadDword(address + 20);
		env[6] = ReadDword(address + 24);
	}

	for(Dword i = 0; i < 20; ++i)
		env[7 + i] = ReadDword(address + 28 + (i * 4));

	asm volatile
	(
		"frstor (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FRSTOR_A16O32_END\n _OP_FRSTOR_A16O32_END:");
}

void OP_FRSTOR_A32O32(void)
{
	Dword env[27];

	Dword address = CalculateAddressA32();

	if(	(registers->r_cr0 & CR0_PE_FLAG) == 0 ||
		(registers->r_systemFlags & EFLAGS_VM_FLAG) != 0)
	{
		//Real/V8086 mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = (ReadDword(address + 12) & 0x0000ffff) |
					((ReadDword(address + 16) & 0x0ffff000) << 4);
		env[4] = (ReadDword(address + 16) & 0x000007ff) << 16;
		env[5] = (ReadDword(address + 20) & 0x0000ffff) |
					((ReadDword(address + 24) & 0x0ffff000) << 4);
		env[6] = 0x00000000;
	}
	else
	{
		//Protected mode
		env[0] = ReadDword(address);
		env[1] = ReadDword(address + 4);
		env[2] = ReadDword(address + 8);
		env[3] = ReadDword(address + 12);
		env[4] = ReadDword(address + 16);
		env[5] = ReadDword(address + 20);
		env[6] = ReadDword(address + 24);
	}

	for(Dword i = 0; i < 20; ++i)
		env[7 + i] = ReadDword(address + 28 + (i * 4));

	asm volatile
	(
		"frstor (%0)		\n"
		:
		: "r" (env)
	);

	asm (".global _OP_FRSTOR_A32O32_END\n _OP_FRSTOR_A32O32_END:");
}
