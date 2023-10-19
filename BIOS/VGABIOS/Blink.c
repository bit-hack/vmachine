//	VMachine VGA BIOS
//	Toggle intensity/blinking bit (int 0x10/ax=0x1003)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void ToggleBlinking(Byte enable)
{
	Byte data;

	//Get the I/O port base
	Word ioPortBase = *(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE);

	//Clear the attribute registers address/data flip-flop
	inportb(ioPortBase + 6);

	//Update attribute register 0x10
	outportb(0x3c0, 0x10);
	data = inportb(0x3c1);

	if(enable)
		data |= 0x08;
	else
		data &= ~0x08;

	outportb(0x3c0, data);
}
