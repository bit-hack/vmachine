//	VMachine VGA BIOS
//	Set all palette registers (int 0x10/ax=0x1002)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void SetAllPaletteRegisters(struct Registers * registers)
{
	Word i;

	//Get the I/O port base
	Word ioPortBase = *(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE);

	//Get a pointer to the palette register values
	Byte far * paletteRegisterValues = MK_FP(registers->r_es, registers->r_dx.w);

	//Clear the attribute registers address/data flip-flop
	inportb(ioPortBase + 6);

	//Loop through palette registers
    for(i = 0; i < 16; ++i)
	{
		//Set the palette register
		outportb(0x3c0, i);
		outportb(0x3c0, *paletteRegisterValues++);
	}
}
