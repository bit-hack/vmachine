//	VMachine VGA BIOS
//	Scroll up window (int 0x10/ah=0x06)
//	Scroll down window (int 0x10/ah=0x07)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void ScrollUpWindow(struct Registers * registers)
{
	Byte numLines	= registers->r_ax.b.l;
	Byte attribute	= registers->r_bx.b.h;
	Byte top		= registers->r_cx.b.h;
	Byte left		= registers->r_cx.b.l;
	Byte bottom		= registers->r_dx.b.h;
	Byte right		= registers->r_dx.b.l;
	Word far * videoMem;
	Word i, j, k;
	
	//Handle values beyond the edges of the screen
	if(top > 24)
		top = 24;

	if(left > 79)
		left = 79;

	if(bottom > 24)
		bottom = 24;

	if(right > 79)
		right = 79;

	//Window is bottom-right inclusive
	++bottom;
	++right;

	//numLines == 0 => Scroll whole window
	if(numLines == 0)
		numLines = bottom - top;

	//Get a pointer to the video memory
	videoMem = MK_FP(0xb800, 0x0000);

	//Scroll the display up
	for(i = 0; i < numLines; ++i)
	{
		for(j = top; j < (bottom - 1); ++j)
			for(k = left; k < right; ++k)
				videoMem[j * 80 + k] = videoMem[(j + 1) * 80 + k];

		//Fill the bottom row with spaces
		for(k = left; k < right; ++k)
			videoMem[(bottom - 1) * 80 + k] = ((Word)attribute << 8) | 0x20;
	}
}

void ScrollDownWindow(struct Registers * registers)
{
	Byte numLines	= registers->r_ax.b.l;
	Byte attribute	= registers->r_bx.b.h;
	Byte top		= registers->r_cx.b.h;
	Byte left		= registers->r_cx.b.l;
	Byte bottom		= registers->r_dx.b.h;
	Byte right		= registers->r_dx.b.l;
	Word far * videoMem;
	Word i, j, k;

	//Handle values beyond the edges of the screen
	if(top > 24)
		top = 24;

	if(left > 79)
		left = 79;

	if(bottom > 24)
		bottom = 24;

	if(right > 79)
		right = 79;
	
	//Window is bottom-right inclusive
	++bottom;
	++right;

	//numLines == 0 => Scroll whole window
	if(numLines == 0)
		numLines = bottom - top;

	//Get a pointer to the video memory
	videoMem = MK_FP(0xb800, 0x0000);

	//Scroll the display up
	for(i = 0; i < numLines; ++i)
	{
		for(j = top; j < (bottom - 1); ++j)
			for(k = left; k < right; ++k)
				videoMem[(j + 1) * 80 + k] = videoMem[j * 80 + k];

		//Fill the top row with spaces
		for(k = left; k < right; ++k)
			videoMem[top * 80 + k] = ((Word)attribute << 8) | 0x20;
	}
}
