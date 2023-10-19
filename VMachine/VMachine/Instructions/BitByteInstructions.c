//	VMachine
//	Implementations of bit and byte instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../HelperFunctions/HelperFunctions.h"
#include "Instructions.h"

#define BT_Rw_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rw & (1 << (bit & 0x0f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BT_Rd_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rd & (1 << (bit & 0x1f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;

//The "M" macros require bit to be of a signed type
#define BT_M_bit_A16(bit)						\
	Dword address = CalculateAddressA16();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BT_M_bit_A32(bit)						\
	Dword address = CalculateAddressA32();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTS_Rw_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rw & (1 << (bit & 0x0f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rw |= 1 << (bit & 0x0f);

#define BTS_Rd_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rd & (1 << (bit & 0x1f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rd |= 1 << (bit & 0x1f);

#define BTS_M_bit_A16(bit)						\
	Dword address = CalculateAddressA16();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data | (1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTS_M_bit_A32(bit)						\
	Dword address = CalculateAddressA32();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data | (1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTR_Rw_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rw & (1 << (bit & 0x0f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rw &= ~(1 << (bit & 0x0f));

#define BTR_Rd_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rd & (1 << (bit & 0x1f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rd &= ~(1 << (bit & 0x1f));

#define BTR_M_bit_A16(bit)						\
	Dword address = CalculateAddressA16();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data & ~(1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTR_M_bit_A32(bit)						\
	Dword address = CalculateAddressA32();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data & ~(1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTC_Rw_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rw & (1 << (bit & 0x0f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rw ^= 1 << (bit & 0x0f);

#define BTC_Rd_bit(bit)							\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((Rd & (1 << (bit & 0x1f))) != 0)			\
		registers->r_eflags |= EFLAGS_C_FLAG;	\
	Rd ^= 1 << (bit & 0x1f);

#define BTC_M_bit_A16(bit)						\
	Dword address = CalculateAddressA16();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data ^ (1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

#define BTC_M_bit_A32(bit)						\
	Dword address = CalculateAddressA32();		\
	address += bit / 8;							\
	Byte data = ReadByte(address);				\
	WriteByte(address, data ^ (1 << (bit & 0x07)));	\
	registers->r_eflags &= ~EFLAGS_C_FLAG;		\
	if((data & (1 << (bit & 0x07))) != 0)		\
		registers->r_eflags |= EFLAGS_C_FLAG;

void OP_BT_Rw_Ib(void) { BT_Rw_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BT_Rw_Ib_END\n _OP_BT_Rw_Ib_END:"); }
void OP_BT_Rd_Ib(void) { BT_Rd_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BT_Rd_Ib_END\n _OP_BT_Rd_Ib_END:"); }
void OP_BT_M_Ib_A16(void) { BT_M_bit_A16((SignedDword)(immByte & 0x1f)) asm (".global _OP_BT_M_Ib_A16_END\n _OP_BT_M_Ib_A16_END:"); }
void OP_BT_M_Ib_A32(void) { BT_M_bit_A32((SignedDword)(immByte & 0x1f)) asm (".global _OP_BT_M_Ib_A32_END\n _OP_BT_M_Ib_A32_END:"); }

void OP_BT_Rw_Gw(void) { BT_Rw_bit((SignedWord)Gw) asm (".global _OP_BT_Rw_Gw_END\n _OP_BT_Rw_Gw_END:"); }
void OP_BT_M_Gw_A16(void) { BT_M_bit_A16((SignedWord)Gw) asm (".global _OP_BT_M_Gw_A16_END\n _OP_BT_M_Gw_A16_END:"); }
void OP_BT_M_Gw_A32(void) { BT_M_bit_A32((SignedWord)Gw) asm (".global _OP_BT_M_Gw_A32_END\n _OP_BT_M_Gw_A32_END:"); }

void OP_BT_Rd_Gd(void) { BT_Rd_bit((SignedDword)Gd) asm (".global _OP_BT_Rd_Gd_END\n _OP_BT_Rd_Gd_END:"); }
void OP_BT_M_Gd_A16(void) { BT_M_bit_A16((SignedDword)Gd) asm (".global _OP_BT_M_Gd_A16_END\n _OP_BT_M_Gd_A16_END:"); }
void OP_BT_M_Gd_A32(void) { BT_M_bit_A32((SignedDword)Gd) asm (".global _OP_BT_M_Gd_A32_END\n _OP_BT_M_Gd_A32_END:"); }

void OP_BTS_Rw_Ib(void) { BTS_Rw_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTS_Rw_Ib_END\n _OP_BTS_Rw_Ib_END:"); }
void OP_BTS_Rd_Ib(void) { BTS_Rd_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTS_Rd_Ib_END\n _OP_BTS_Rd_Ib_END:"); }
void OP_BTS_M_Ib_A16(void) { BTS_M_bit_A16((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTS_M_Ib_A16_END\n _OP_BTS_M_Ib_A16_END:"); }
void OP_BTS_M_Ib_A32(void) { BTS_M_bit_A32((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTS_M_Ib_A32_END\n _OP_BTS_M_Ib_A32_END:"); }

void OP_BTS_Rw_Gw(void) { BTS_Rw_bit((SignedWord)Gw) asm (".global _OP_BTS_Rw_Gw_END\n _OP_BTS_Rw_Gw_END:"); }
void OP_BTS_M_Gw_A16(void) { BTS_M_bit_A16((SignedWord)Gw) asm (".global _OP_BTS_M_Gw_A16_END\n _OP_BTS_M_Gw_A16_END:"); }
void OP_BTS_M_Gw_A32(void) { BTS_M_bit_A32((SignedWord)Gw) asm (".global _OP_BTS_M_Gw_A32_END\n _OP_BTS_M_Gw_A32_END:"); }

void OP_BTS_Rd_Gd(void) { BTS_Rd_bit((SignedDword)Gd) asm (".global _OP_BTS_Rd_Gd_END\n _OP_BTS_Rd_Gd_END:"); }
void OP_BTS_M_Gd_A16(void) { BTS_M_bit_A16((SignedDword)Gd) asm (".global _OP_BTS_M_Gd_A16_END\n _OP_BTS_M_Gd_A16_END:"); }
void OP_BTS_M_Gd_A32(void) { BTS_M_bit_A32((SignedDword)Gd) asm (".global _OP_BTS_M_Gd_A32_END\n _OP_BTS_M_Gd_A32_END:"); }

void OP_BTR_Rw_Ib(void) { BTR_Rw_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTR_Rw_Ib_END\n _OP_BTR_Rw_Ib_END:"); }
void OP_BTR_Rd_Ib(void) { BTR_Rd_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTR_Rd_Ib_END\n _OP_BTR_Rd_Ib_END:"); }
void OP_BTR_M_Ib_A16(void) { BTR_M_bit_A16((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTR_M_Ib_A16_END\n _OP_BTR_M_Ib_A16_END:"); }
void OP_BTR_M_Ib_A32(void) { BTR_M_bit_A32((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTR_M_Ib_A32_END\n _OP_BTR_M_Ib_A32_END:"); }

void OP_BTR_Rw_Gw(void) { BTR_Rw_bit((SignedWord)Gw) asm (".global _OP_BTR_Rw_Gw_END\n _OP_BTR_Rw_Gw_END:"); }
void OP_BTR_M_Gw_A16(void) { BTR_M_bit_A16((SignedWord)Gw) asm (".global _OP_BTR_M_Gw_A16_END\n _OP_BTR_M_Gw_A16_END:"); }
void OP_BTR_M_Gw_A32(void) { BTR_M_bit_A32((SignedWord)Gw) asm (".global _OP_BTR_M_Gw_A32_END\n _OP_BTR_M_Gw_A32_END:"); }

void OP_BTR_Rd_Gd(void) { BTR_Rd_bit((SignedDword)Gd) asm (".global _OP_BTR_Rd_Gd_END\n _OP_BTR_Rd_Gd_END:"); }
void OP_BTR_M_Gd_A16(void) { BTR_M_bit_A16((SignedDword)Gd) asm (".global _OP_BTR_M_Gd_A16_END\n _OP_BTR_M_Gd_A16_END:"); }
void OP_BTR_M_Gd_A32(void) { BTR_M_bit_A32((SignedDword)Gd) asm (".global _OP_BTR_M_Gd_A32_END\n _OP_BTR_M_Gd_A32_END:"); }

void OP_BTC_Rw_Ib(void) { BTC_Rw_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTC_Rw_Ib_END\n _OP_BTC_Rw_Ib_END:"); }
void OP_BTC_Rd_Ib(void) { BTC_Rd_bit((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTC_Rd_Ib_END\n _OP_BTC_Rd_Ib_END:"); }
void OP_BTC_M_Ib_A16(void) { BTC_M_bit_A16((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTC_M_Ib_A16_END\n _OP_BTC_M_Ib_A16_END:"); }
void OP_BTC_M_Ib_A32(void) { BTC_M_bit_A32((SignedDword)(immByte & 0x1f)) asm (".global _OP_BTC_M_Ib_A32_END\n _OP_BTC_M_Ib_A32_END:"); }

void OP_BTC_Rw_Gw(void) { BTC_Rw_bit((SignedWord)Gw) asm (".global _OP_BTC_Rw_Gw_END\n _OP_BTC_Rw_Gw_END:"); }
void OP_BTC_M_Gw_A16(void) { BTC_M_bit_A16((SignedWord)Gw) asm (".global _OP_BTC_M_Gw_A16_END\n _OP_BTC_M_Gw_A16_END:"); }
void OP_BTC_M_Gw_A32(void) { BTC_M_bit_A32((SignedWord)Gw) asm (".global _OP_BTC_M_Gw_A32_END\n _OP_BTC_M_Gw_A32_END:"); }

void OP_BTC_Rd_Gd(void) { BTC_Rd_bit((SignedDword)Gd) asm (".global _OP_BTC_Rd_Gd_END\n _OP_BTC_Rd_Gd_END:"); }
void OP_BTC_M_Gd_A16(void) { BTC_M_bit_A16((SignedDword)Gd) asm (".global _OP_BTC_M_Gd_A16_END\n _OP_BTC_M_Gd_A16_END:"); }
void OP_BTC_M_Gd_A32(void) { BTC_M_bit_A32((SignedDword)Gd) asm (".global _OP_BTC_M_Gd_A32_END\n _OP_BTC_M_Gd_A32_END:"); }

void OP_SETO_Rb(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETO_Rb_END\n _OP_SETO_Rb_END:");
}

void OP_SETNO_Rb(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNO_Rb_END\n _OP_SETNO_Rb_END:");
}

void OP_SETB_Rb(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETB_Rb_END\n _OP_SETB_Rb_END:");
}

void OP_SETNB_Rb(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNB_Rb_END\n _OP_SETNB_Rb_END:");
}

void OP_SETZ_Rb(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETZ_Rb_END\n _OP_SETZ_Rb_END:");
}

void OP_SETNZ_Rb(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNZ_Rb_END\n _OP_SETNZ_Rb_END:");
}

void OP_SETBE_Rb(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETBE_Rb_END\n _OP_SETBE_Rb_END:");
}

void OP_SETNBE_Rb(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNBE_Rb_END\n _OP_SETNBE_Rb_END:");
}

void OP_SETS_Rb(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETS_Rb_END\n _OP_SETS_Rb_END:");
}

void OP_SETNS_Rb(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNS_Rb_END\n _OP_SETNS_Rb_END:");
}

void OP_SETP_Rb(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETP_Rb_END\n _OP_SETP_Rb_END:");
}

void OP_SETNP_Rb(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
		Rb = 1;
	else
		Rb = 0;

	asm (".global _OP_SETNP_Rb_END\n _OP_SETNP_Rb_END:");
}

void OP_SETL_Rb(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Rb = 1;
	}
	else
		Rb = 0;

	asm (".global _OP_SETL_Rb_END\n _OP_SETL_Rb_END:");
}

void OP_SETNL_Rb(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		Rb = 1;
	}
	else
		Rb = 0;

	asm (".global _OP_SETNL_Rb_END\n _OP_SETNL_Rb_END:");
}

void OP_SETLE_Rb(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		Rb = 1;
	}
	else
		Rb = 0;

	asm (".global _OP_SETLE_Rb_END\n _OP_SETLE_Rb_END:");
}

void OP_SETNLE_Rb(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		Rb = 1;
	}
	else
		Rb = 0;

	asm (".global _OP_SETNLE_Rb_END\n _OP_SETNLE_Rb_END:");
}

void OP_SETO_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETO_Mb_A16_END\n _OP_SETO_Mb_A16_END:");
}

void OP_SETNO_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNO_Mb_A16_END\n _OP_SETNO_Mb_A16_END:");
}

void OP_SETB_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETB_Mb_A16_END\n _OP_SETB_Mb_A16_END:");
}

void OP_SETNB_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNB_Mb_A16_END\n _OP_SETNB_Mb_A16_END:");
}

void OP_SETZ_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETZ_Mb_A16_END\n _OP_SETZ_Mb_A16_END:");
}

void OP_SETNZ_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNZ_Mb_A16_END\n _OP_SETNZ_Mb_A16_END:");
}

void OP_SETBE_Mb_A16(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETBE_Mb_A16_END\n _OP_SETBE_Mb_A16_END:");
}

void OP_SETNBE_Mb_A16(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNBE_Mb_A16_END\n _OP_SETNBE_Mb_A16_END:");
}

void OP_SETS_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETS_Mb_A16_END\n _OP_SETS_Mb_A16_END:");
}

void OP_SETNS_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNS_Mb_A16_END\n _OP_SETNS_Mb_A16_END:");
}

void OP_SETP_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETP_Mb_A16_END\n _OP_SETP_Mb_A16_END:");
}

void OP_SETNP_Mb_A16(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
		WriteByte(CalculateAddressA16(), 1);
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNP_Mb_A16_END\n _OP_SETNP_Mb_A16_END:");
}

void OP_SETL_Mb_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		WriteByte(CalculateAddressA16(), 1);
	}
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETL_Mb_A16_END\n _OP_SETL_Mb_A16_END:");
}

void OP_SETNL_Mb_A16(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		WriteByte(CalculateAddressA16(), 1);
	}
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNL_Mb_A16_END\n _OP_SETNL_Mb_A16_END:");
}

void OP_SETLE_Mb_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		WriteByte(CalculateAddressA16(), 1);
	}
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETLE_Mb_A16_END\n _OP_SETLE_Mb_A16_END:");
}

void OP_SETNLE_Mb_A16(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		WriteByte(CalculateAddressA16(), 1);
	}
	else
		WriteByte(CalculateAddressA16(), 0);

	asm (".global _OP_SETNLE_Mb_A16_END\n _OP_SETNLE_Mb_A16_END:");
}

void OP_SETO_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETO_Mb_A32_END\n _OP_SETO_Mb_A32_END:");
}

void OP_SETNO_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_O_FLAG) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNO_Mb_A32_END\n _OP_SETNO_Mb_A32_END:");
}

void OP_SETB_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETB_Mb_A32_END\n _OP_SETB_Mb_A32_END:");
}

void OP_SETNB_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_C_FLAG) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNB_Mb_A32_END\n _OP_SETNB_Mb_A32_END:");
}

void OP_SETZ_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETZ_Mb_A32_END\n _OP_SETZ_Mb_A32_END:");
}

void OP_SETNZ_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_Z_FLAG) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNZ_Mb_A32_END\n _OP_SETNZ_Mb_A32_END:");
}

void OP_SETBE_Mb_A32(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETBE_Mb_A32_END\n _OP_SETBE_Mb_A32_END:");
}

void OP_SETNBE_Mb_A32(void)
{
	if((registers->r_eflags & (EFLAGS_C_FLAG | EFLAGS_Z_FLAG)) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNBE_Mb_A32_END\n _OP_SETNBE_Mb_A32_END:");
}

void OP_SETS_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETS_Mb_A32_END\n _OP_SETS_Mb_A32_END:");
}

void OP_SETNS_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_S_FLAG) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNS_Mb_A32_END\n _OP_SETNS_Mb_A32_END:");
}

void OP_SETP_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) != 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETP_Mb_A32_END\n _OP_SETP_Mb_A32_END:");
}

void OP_SETNP_Mb_A32(void)
{
	if((registers->r_eflags & EFLAGS_P_FLAG) == 0)
		WriteByte(CalculateAddressA32(), 1);
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNP_Mb_A32_END\n _OP_SETNP_Mb_A32_END:");
}

void OP_SETL_Mb_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		WriteByte(CalculateAddressA32(), 1);
	}
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETL_Mb_A32_END\n _OP_SETL_Mb_A32_END:");
}

void OP_SETNL_Mb_A32(void)
{
	if(	((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0))
	{
		WriteByte(CalculateAddressA32(), 1);
	}
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNL_Mb_A32_END\n _OP_SETNL_Mb_A32_END:");
}

void OP_SETLE_Mb_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) !=
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) ||
		(registers->r_eflags & EFLAGS_Z_FLAG) != 0)
	{
		WriteByte(CalculateAddressA32(), 1);
	}
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETLE_Mb_A32_END\n _OP_SETLE_Mb_A32_END:");
}

void OP_SETNLE_Mb_A32(void)
{
	if((((registers->r_eflags & EFLAGS_S_FLAG) != 0) ==
		((registers->r_eflags & EFLAGS_O_FLAG) != 0)) &&
		(registers->r_eflags & EFLAGS_Z_FLAG) == 0)
	{
		WriteByte(CalculateAddressA32(), 1);
	}
	else
		WriteByte(CalculateAddressA32(), 0);

	asm (".global _OP_SETNLE_Mb_A32_END\n _OP_SETNLE_Mb_A32_END:");
}
