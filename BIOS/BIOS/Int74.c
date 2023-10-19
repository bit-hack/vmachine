//	VMachine BIOS
//	Int 0x74 (IRQ 12 - PS/2 Mouse) handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Int74Handler(struct Registers * registers)
{
	Byte buttonData, xData, yData;
	Word mouseHandlerData0, mouseHandlerData1, mouseHandlerData2, mouseHandlerData3;
	Word mouseHandlerPointer[2];

	//Read the mouse data from the keyboard controller
	buttonData = inportb(0x60);
	xData = inportb(0x60);
	yData = inportb(0x60);

	//Send end-of-interrupt to the PICs
	outportb(0xa0, 0x20);
	outportb(0x20, 0x20);

	//Enable interrupts
	asm sti;

	//Assemble the data to pass to the mouse handler
	mouseHandlerData0 = buttonData;
	mouseHandlerData1 = xData;
	mouseHandlerData2 = yData;
	mouseHandlerData3 = 0x0000;

	//Get a pointer to the mouse handler
	mouseHandlerPointer[1] = *(Word far *)MK_FP(EBDA_SEG, EBDA_MOUSE_HANDLER_SEGMENT);
	mouseHandlerPointer[0] = *(Word far *)MK_FP(EBDA_SEG, EBDA_MOUSE_HANDLER_OFFSET);
	
	//Call the mouse handler
	if(mouseHandlerPointer[0] != 0x0000 || mouseHandlerPointer[1] != 0x0000)
	{
		asm {
			push mouseHandlerData0
			push mouseHandlerData1
			push mouseHandlerData2
			push mouseHandlerData3
			call mouseHandlerPointer
			add sp, 8
		}
	}
}
