//	VMachine VGA BIOS
//	Set video mode (int 0x10/ah=0x00)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void SetMode(struct Registers * registers)
{
	Byte mode;
	BOOL clearMemory = FALSE;
	Byte far * vgaRegisterValues;
	Word ioPortBase;
	Byte far * vgaColorRegisterValues;
	Word i;

	//Get the mode number
	mode = registers->r_ax.b.l;

	//If the top bit of the mode number is clear, need to clear the contents of video memory
	if((mode & 0x80) == 0)
		clearMemory = TRUE;

	mode &= 0x7f;

	//Ensure the mode number is valid (0x00 - 0x07, 0x0d - 0x13)
	if(!(mode < 0x08 | (mode >= 0x0d && mode < 0x14)))
		return;

	//Save the mode number to the BDA
	*(Byte far *)MK_FP(BDA_SEG, BDA_VIDEO_MODE) = mode;

	//Get a pointer to the VGA registers for this mode
	vgaRegisterValues = MK_FP(VGA_BIOS_SEG, VGA_REGISTER_VALUES_OFFSET);

    if(mode < 0x08)
		vgaRegisterValues += (mode * 61);
	else
		vgaRegisterValues += ((mode - 5) * 61);

	//Get the new I/O port base and save to the BDA
	ioPortBase = ((*vgaRegisterValues & 0x01) != 0) ? 0x3d4 : 0x3b4;

	*(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE) = ioPortBase;

    //Set the new VGA register values

	//Misc Output Register
	outportb(0x3c2, *vgaRegisterValues++);

	//Sequencer Registers
	for(i = 0; i < 5; ++i)
	{
		outportb(0x3c4, i);
		outportb(0x3c5, *vgaRegisterValues++);
	}

	//CRTC Registers
	for(i = 0; i < 25; ++i)
	{
		outportb(ioPortBase, i);
		outportb(ioPortBase + 1, *vgaRegisterValues++);
	}

	//Graphics Registers
	for(i = 0; i < 9; ++i)
	{
		outportb(0x3ce, i);
		outportb(0x3cf, *vgaRegisterValues++);
	}
	
	//Attribute Registers
	
	//Clear the address/data flip-flop
	inportb(ioPortBase + 6);

	for(i = 0; i < 21; ++i)
	{
		outportb(0x3c0, i);
		outportb(0x3c0, *vgaRegisterValues++);
	}

	//Set the new VGA color register values
	vgaColorRegisterValues = MK_FP(VGA_BIOS_SEG, VGA_COLOR_REGISTER_VALUES_OFFSET);

	if(mode < 0x08)
		vgaColorRegisterValues += (mode * 256 * 3);
	else
		vgaRegisterValues += ((mode - 5) * 256 * 3);

	outportb(0x03c8, 0x00);

	for(i = 0; i < 256 * 3; ++i)
		outportb(0x03c9, *vgaColorRegisterValues++);
	
	//Clear video memory if requested
	//TODO: Other modes
	if(clearMemory)
	{
		if(mode == 0x02 || mode == 0x03)
		{
			//Clear the video memory to 0x0720 - white on black, ' ' character
			Word far * videoMem = MK_FP(0xb800, 0x0000);

			for(i = 0; i < 0x4000; ++i)
				*videoMem++ = 0x0720;
		}
	}

	//Reset the cursor position to the top-left
	SetCursorPos(0, 0);
}
