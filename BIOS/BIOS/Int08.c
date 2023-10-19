//	VMachine BIOS
//	Int 0x08 (IRQ 0 - PIT timer 0) handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int08Handler(struct Registers * registers)
{
	//Get a pointer to the timer tick count
	Dword far * timerTickCount = MK_FP(BDA_SEG, BDA_TIMER_TICK_COUNT);

	//Increment the tick count
	++(*timerTickCount);

	//If the tick count is 0x1800b2 (24 hours have passed), set the rollover flag
	if(*timerTickCount == 0x1800b2)
	{
		*timerTickCount = 0;
		*(Byte far *)MK_FP(BDA_SEG, BDA_TIMER_ROLLOVER_FLAG) = 1;
	}

	//Call int 0x1c: System timer tick
	asm {
		sti
		int 0x1c
		cli
	}

	//Send end-of-interrupt to the PIC
	outportb(0x20, 0x20);
}
