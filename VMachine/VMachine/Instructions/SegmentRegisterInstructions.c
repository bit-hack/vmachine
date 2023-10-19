//	VMachine
//	Implementations of segment register instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define LSEGREG_Gw_Mp_A16(sreg)					\
	Dword address = CalculateAddressA16();		\
	Word offset = ReadWord(address);			\
	Word selector = ReadWord(address + 2);		\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	Gw = offset;

#define LSEGREG_Gd_Mp_A16(sreg)					\
	Dword address = CalculateAddressA16();		\
	Dword offset = ReadDword(address);			\
	Word selector = ReadWord(address + 4);		\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	Gd = offset;

#define LSEGREG_Gw_Mp_A32(sreg)					\
	Dword address = CalculateAddressA32();		\
	Word offset = ReadWord(address);			\
	Word selector = ReadWord(address + 2);		\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	Gw = offset;

#define LSEGREG_Gd_Mp_A32(sreg)					\
	Dword address = CalculateAddressA32();		\
	Dword offset = ReadDword(address);			\
	Word selector = ReadWord(address + 4);		\
	SetDataSegmentRegisterValuePtr(&registers->r_##sreg, selector);	\
	Gd = offset;

void OP_LDS_Gw_Mp_A16(void) { LSEGREG_Gw_Mp_A16(ds) asm (".global _OP_LDS_Gw_Mp_A16_END\n _OP_LDS_Gw_Mp_A16_END:"); }
void OP_LES_Gw_Mp_A16(void) { LSEGREG_Gw_Mp_A16(es) asm (".global _OP_LES_Gw_Mp_A16_END\n _OP_LES_Gw_Mp_A16_END:"); }
void OP_LFS_Gw_Mp_A16(void) { LSEGREG_Gw_Mp_A16(fs) asm (".global _OP_LFS_Gw_Mp_A16_END\n _OP_LFS_Gw_Mp_A16_END:"); }
void OP_LGS_Gw_Mp_A16(void) { LSEGREG_Gw_Mp_A16(gs) asm (".global _OP_LGS_Gw_Mp_A16_END\n _OP_LGS_Gw_Mp_A16_END:"); }

void OP_LDS_Gd_Mp_A16(void) { LSEGREG_Gd_Mp_A16(ds) asm (".global _OP_LDS_Gd_Mp_A16_END\n _OP_LDS_Gd_Mp_A16_END:"); }
void OP_LES_Gd_Mp_A16(void) { LSEGREG_Gd_Mp_A16(es) asm (".global _OP_LES_Gd_Mp_A16_END\n _OP_LES_Gd_Mp_A16_END:"); }
void OP_LFS_Gd_Mp_A16(void) { LSEGREG_Gd_Mp_A16(fs) asm (".global _OP_LFS_Gd_Mp_A16_END\n _OP_LFS_Gd_Mp_A16_END:"); }
void OP_LGS_Gd_Mp_A16(void) { LSEGREG_Gd_Mp_A16(gs) asm (".global _OP_LGS_Gd_Mp_A16_END\n _OP_LGS_Gd_Mp_A16_END:"); }

void OP_LDS_Gw_Mp_A32(void) { LSEGREG_Gw_Mp_A32(ds) asm (".global _OP_LDS_Gw_Mp_A32_END\n _OP_LDS_Gw_Mp_A32_END:"); }
void OP_LES_Gw_Mp_A32(void) { LSEGREG_Gw_Mp_A32(es) asm (".global _OP_LES_Gw_Mp_A32_END\n _OP_LES_Gw_Mp_A32_END:"); }
void OP_LFS_Gw_Mp_A32(void) { LSEGREG_Gw_Mp_A32(fs) asm (".global _OP_LFS_Gw_Mp_A32_END\n _OP_LFS_Gw_Mp_A32_END:"); }
void OP_LGS_Gw_Mp_A32(void) { LSEGREG_Gw_Mp_A32(gs) asm (".global _OP_LGS_Gw_Mp_A32_END\n _OP_LGS_Gw_Mp_A32_END:"); }

void OP_LDS_Gd_Mp_A32(void) { LSEGREG_Gd_Mp_A32(ds) asm (".global _OP_LDS_Gd_Mp_A32_END\n _OP_LDS_Gd_Mp_A32_END:"); }
void OP_LES_Gd_Mp_A32(void) { LSEGREG_Gd_Mp_A32(es) asm (".global _OP_LES_Gd_Mp_A32_END\n _OP_LES_Gd_Mp_A32_END:"); }
void OP_LFS_Gd_Mp_A32(void) { LSEGREG_Gd_Mp_A32(fs) asm (".global _OP_LFS_Gd_Mp_A32_END\n _OP_LFS_Gd_Mp_A32_END:"); }
void OP_LGS_Gd_Mp_A32(void) { LSEGREG_Gd_Mp_A32(gs) asm (".global _OP_LGS_Gd_Mp_A32_END\n _OP_LGS_Gd_Mp_A32_END:"); }
