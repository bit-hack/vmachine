//	VMachine VGA BIOS
//	Read character and attribute (int 0x10/ah=0x08)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void ReadCharacterAndAttribute(struct Registers * registers)
{
	Word far * videoMem;
	Byte cursorX;
	Byte cursorY;

	//Get a pointer to the video memory
	videoMem = MK_FP(0xb800, 0x0000);

	//Get the cursor position
	cursorX = *(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_X);
	cursorY = *(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_Y);

	//Read the character into ax
	registers->r_ax.w = videoMem[cursorY * 80 + cursorX];
}
