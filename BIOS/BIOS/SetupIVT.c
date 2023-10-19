//	VMachine BIOS
//	Set up the IVT
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "..\Common\BIOS.h"

void NullIntHandler(void);
void flInt08Handler(void);
void flInt09Handler(void);
void flInt0eHandler(void);
void flInt74Handler(void);
void flInt76Handler(void);
void flInt11Handler(void);
void flInt12Handler(void);
void flInt13Handler(void);
void flInt15Handler(void);
void flInt16Handler(void);
void flInt1aHandler(void);
void Int19Handler(void);
void Int1cHandler(void);

void SetupIVT(void)
{
	Word far * ivt;
	Word intNum;

	//Point all IVT entries at NullIntHandler
	ivt = MK_FP(0x0000, 0x0000);

	for(intNum = 0; intNum < 0x100; ++intNum)
	{
		*ivt++ = NullIntHandler;
		*ivt++ = 0xf000;
	}

	//Redirect the entries for:
	//Hardware interrupts: 0x08, 0x09, 0x0e, 0x74, 0x76
	//Todo: 0x70, 0x75
	ivt = MK_FP(0x0000, 0x0008 * 4);
	*ivt = flInt08Handler;

	ivt = MK_FP(0x0000, 0x0009 * 4);
	*ivt = flInt09Handler;

	ivt = MK_FP(0x0000, 0x000e * 4);
	*ivt = flInt0eHandler;

	ivt = MK_FP(0x0000, 0x0074 * 4);
	*ivt = flInt74Handler;

	ivt = MK_FP(0x0000, 0x0076 * 4);
	*ivt = flInt76Handler;

	//Software interrupts: 0x11, 0x12, 0x13, 0x15, 0x16, 0x1a
	//Todo: 0x14, 0x17, 0x18, 0x19, 0x1e, 0x1f
	ivt = MK_FP(0x0000, 0x0012 * 4);
	*ivt = flInt12Handler;

	ivt = MK_FP(0x0000, 0x0011 * 4);
	*ivt = flInt11Handler;

	ivt = MK_FP(0x0000, 0x0013 * 4);
	*ivt = flInt13Handler;

	ivt = MK_FP(0x0000, 0x0015 * 4);
	*ivt = flInt15Handler;

	ivt = MK_FP(0x0000, 0x0016 * 4);
	*ivt = flInt16Handler;

	ivt = MK_FP(0x0000, 0x001a * 4);
	*ivt = flInt1aHandler;

	//Miscellaneous interrupts
	
	//int 0x19, 0x1c
	ivt = MK_FP(0x0000, 0x0019 * 4);
	*ivt = Int19Handler;

	ivt = MK_FP(0x0000, 0x001c * 4);
	*ivt = Int1cHandler;

	//int 0x41 - Hard disk 0 parameter table
	ivt = MK_FP(0x0000, 0x0041 * 4);
	*ivt = 0xe401;

	//Todo: 0x40, 0x43, 0x46
}
