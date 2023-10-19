//	VMachine
//	Declarations used in instruction implementations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

//External references, to be patched into the instruction implementations
extern Dword segRegBase;
extern Dword baseReg, indexReg, indexShift;
extern Dword displacement;
extern Byte Rb;
extern Byte Gb;
extern Word Rw;
extern Word Gw;
extern Dword Rd;
extern Dword Gd;
extern struct SegmentRegisterData Sw;
extern Byte immByte;
extern Word immWord;
extern Dword immDword;
extern Dword instrLength;

//Calculate an effective address
static inline Dword CalculateAddressA16(void)
{ return segRegBase + ((baseReg + indexReg + displacement) & 0xffff); }

static inline Dword CalculateAddressA32(void)
{ return segRegBase + baseReg + (indexReg << indexShift) + displacement; }

//Helper functions

//Memory access
Dword __fastcall ReadByte(Dword address);
Dword __fastcall ReadWord(Dword address);
Dword __fastcall ReadDword(Dword address);
void __fastcall WriteByte(Dword address, Dword data);
void __fastcall WriteWord(Dword address, Dword data);
void __fastcall WriteDword(Dword address, Dword data);

//Set the value of a segment register
void SetDataSegmentRegisterValue(Dword reg, Dword selector);
void SetDataSegmentRegisterValuePtr(struct SegmentRegisterData * segReg, Dword selector);

#endif
