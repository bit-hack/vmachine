//	VMachine VGA BIOS
//	Write a character and an optional attribute at the cursor position
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void SetCursorPos(Byte cursorX, Byte cursorY);

//Scroll the display up
static void ScrollUp(void)
{
	Word far * videoMem;
	Word i;

	//Get a pointer to the video memory
	videoMem = MK_FP(0xb800, 0x0000);

	//Scroll the display up
	for(i = 0; i < 80 * 24; ++i)
		videoMem[i] = videoMem[80 + i];

	//Fill the last row with spaces
	for(i = 0; i < 80; ++i)
		videoMem[80 * 24 + i] = 0x0720;
}

void WriteCharacter(Byte character, BOOL writeAttribute, Byte attribute)
{
	Byte far * videoMem;
	Byte cursorX;
	Byte cursorY;

	//Get a pointer to the video memory
	videoMem = MK_FP(0xb800, 0x0000);

	//Get the cursor position
	cursorX = *(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_X);
	cursorY = *(Byte far *)MK_FP(BDA_SEG, BDA_CURSOR_Y);

	switch(character)
	{
		//CR
		case '\r':
			cursorX = 0x00;
			break;

		//LF
		case '\n':
			if(cursorY == 24)
			{
				//TODO: Scrolling for "Write character and attribute" breaks DOS setup
				if(!writeAttribute)
					ScrollUp();
			}
			else
				++cursorY;
			break;

		//BS
		case '\b':
			if(cursorX == 0)
			{
				cursorX = 80 - 1;
				--cursorY;
			}
			else
				--cursorX;

			videoMem[(cursorY * 80 + cursorX) * 2] = ' ';
				
			break;

		//TODO: BEL
		case 0x07:
			break;

		default:
			videoMem[(cursorY * 80 + cursorX) * 2] = character;

			if(writeAttribute)
				videoMem[(cursorY * 80 + cursorX) * 2 + 1] = attribute;

			if(cursorX == 79)
			{
				cursorX = 0x00;

				if(cursorY == 24)
				{
					//TODO: Scrolling for "Write character and attribute" breaks DOS setup
					if(!writeAttribute)
						ScrollUp();
				}
				else
					++cursorY;
			}
			else
				++cursorX;
	}

	//Save the new cursor position
	SetCursorPos(cursorX, cursorY);
}
