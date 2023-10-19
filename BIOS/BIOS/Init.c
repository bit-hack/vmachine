//	VMachine BIOS
//	Initialise the BDA and the hardware
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void Init(void)
{
	//Initialise the BDA
	
	//EBDA segment
	*(Word far *)MK_FP(BDA_SEG, BDA_EBDA_SEG) = EBDA_SEG;

	//Equipment list
	//TODO: Keep this up-to-date
	//0000 0000 0010 0111b
	//\/ | \_/  \/\/  ||`-- Floppy installed
	// | |  |    | |  |`--- 80x87 installed
	// | |  |    | |  `---- PS/2 mouse installed
	// | |  |    | `------- Color, 80x25 display
	// | |  |    `--------- 1 floppy installed
	// | |  `-------------- 0 serial ports installed
	// | `----------------- No game port installed
	// `------------------- 0 parallel ports installed
	*(Word far *)MK_FP(BDA_SEG, BDA_EQUIPMENT_LIST) = 0x0027;
	
	//Memory size (kilobytes of contiguous memory starting at 0x0000)
	*(Word far *)MK_FP(BDA_SEG, BDA_MEMSIZE) = 639;

	//Keyboard flags
	*(Byte far *)MK_FP(BDA_SEG, BDA_KEYBOARD_FLAGS0) = 0x00;
	*(Byte far *)MK_FP(BDA_SEG, BDA_KEYBOARD_FLAGS1) = 0x00;

	//Keyboard buffer read pointer offset
	*(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_READ_PTR) = 0x001e;

	//Keyboard buffer write pointer offset
	*(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_WRITE_PTR) = 0x001e;

	//Keyboard buffer start offset
	*(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_START) = 0x001e;

	//Keyboard buffer end + 1 offset
	*(Word far *)MK_FP(BDA_SEG, BDA_KEYBOARD_BUFFER_END) = 0x003e;

	//Enable the floppy controller
	outportb(0x3f2, 0x0c);
}