//	VMachine VGA BIOS
//	Initialise the VGA
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "..\Common\BIOS.h"

void SetMode(struct Registers * registers);

void Init(void)
{
	struct Registers registers;

	//Set mode 3
	registers.r_ax.b.l = 0x03;
    SetMode(&registers);

	//Initialise the BDA

	//Number of video rows (minus 1)
	//TODO: Write this value in SetMode
	*(Byte far *)MK_FP(BDA_SEG, BDA_VIDEO_NUM_ROWS) = 24;

	//Initialise the EBDA

	//Display combination code (VGA, color display)
	*(Word far *)MK_FP(EBDA_SEG, EBDA_DISPLAY_CODE) = 0x0008;
}
