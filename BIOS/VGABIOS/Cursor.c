//	VMachine VGA BIOS
//	Set cursor position
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void SetCursorPos(Byte cursorX, Byte cursorY)
{
	Word ioPortBase;
	Word cursorPos;

	*(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_X) = cursorX;
	*(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_Y) = cursorY;

	//Get the I/O port base
	ioPortBase = *(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE);

	//Update the VGA cursor position (CRTC registers 0xe & 0xf)
	cursorPos = cursorY * 80 + cursorX;

	outportb(ioPortBase, 0x0e);
	outportb(ioPortBase + 1, (Byte)(cursorPos >> 8));

	outportb(ioPortBase, 0x0f);
	outportb(ioPortBase + 1, (Byte)cursorPos);
}