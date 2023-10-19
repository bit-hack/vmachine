//	VMachine BIOS
//	Int 0x1a handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int1aHandler(struct Registers * registers)
{
	//Call the function for this function code
	switch(registers->r_ax.b.h)
	{
	//int 0x15/ah=0x00: Get system time
	case 0x00:
		registers->r_dx.w = *(Word far *)MK_FP(BDA_SEG, BDA_TIMER_TICK_COUNT);
		registers->r_cx.w = *(Word far *)MK_FP(BDA_SEG, BDA_TIMER_TICK_COUNT + 2);
		registers->r_ax.b.l = *(Byte far *)MK_FP(BDA_SEG, BDA_TIMER_ROLLOVER_FLAG);
		break;

	//int 0x15/ah=0x01: Set system time
	case 0x01:
		 *(Word far *)MK_FP(BDA_SEG, BDA_TIMER_TICK_COUNT) = registers->r_dx.w;
		 *(Word far *)MK_FP(BDA_SEG, BDA_TIMER_TICK_COUNT + 2) = registers->r_cx.w;
		 *(Byte far *)MK_FP(BDA_SEG, BDA_TIMER_ROLLOVER_FLAG) = registers->r_ax.b.l;
		break;
	}
}
