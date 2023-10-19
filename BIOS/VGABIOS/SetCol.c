//	VMachine VGA BIOS
//	Set block of color registers (int 0x10/ax=0x1012)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void SetColorRegisters(struct Registers * registers)
{
	Word i;

	//Get a pointer to the color register values
	Byte far * colorRegisterValues = MK_FP(registers->r_es, registers->r_dx.w);

	//Set the color register index
	outportb(0x03c8, registers->r_bx.b.l);

	//Loop through the color registers to set
	for(i = 0; i < registers->r_cx.w; ++i)
	{
		//Set the color register
        outportb(0x03c9, *colorRegisterValues++);
		outportb(0x03c9, *colorRegisterValues++);
		outportb(0x03c9, *colorRegisterValues++);
	}
}
