//	VMachine VGA BIOS
//	Int 0x10 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "..\Common\BIOS.h"

void SetMode(struct Registers * registers);
void SetCursorPos(Byte cursorX, Byte cursorY);
void ScrollUpWindow(struct Registers * registers);
void ScrollDownWindow(struct Registers * registers);
void ReadCharacterAndAttribute(struct Registers * registers);
void WriteCharacter(Byte character, BOOL writeAttribute, Byte attribute);
void SetAllPaletteRegisters(struct Registers * registers);
void ToggleBlinking(Byte enable);
void SetColorRegisters(struct Registers * registers);

void Int10Handler(struct Registers * registers)
{
	//Call the function for this function code
	switch(registers->r_ax.b.h)
	{
	//int 0x10/ah=0x00: Set video mode
	case 0x00:
		SetMode(registers);
		break;

	//int 0x10/ah=0x01: Set text mode cursor shape - Unimplemented
	case 0x01:
		break;

	//int 0x10/ah=0x02: Set cursor position
	case 0x02:
		SetCursorPos(registers->r_dx.b.l, registers->r_dx.b.h);
		break;

	//int 0x10/ah=0x03: Get cursor position and size
	//TODO: Cursor size
	case 0x03:
		registers->r_dx.w = *(Word far *)MK_FP(BDA_SEG, BDA_CURSOR_X);
		break;

	//int 0x10/ah=0x05: Set active display page - Unimplemented
	case 0x05:
		break;

	//int 0x10/ah=0x06: Scroll up window
	case 0x06:
		ScrollUpWindow(registers);
		break;

	//int 0x10/ah=0x07: Scroll down window
	case 0x07:
		ScrollDownWindow(registers);
		break;

	//int 0x10/ah=0x08: Read character and attribute
	case 0x08:
		ReadCharacterAndAttribute(registers);
		break;

	//int 0x10/ah=0x09: Write character and attribute
	case 0x09:
		{
			Word i;
			for(i = 0; i < registers->r_cx.w; ++i)
				WriteCharacter(registers->r_ax.b.l, TRUE, registers->r_bx.b.l);
		}
		break;

	//int 0x10/ah=0x0a: Write character
	case 0x0a:
		{
			Word i;
			for(i = 0; i < registers->r_cx.w; ++i)
				WriteCharacter(registers->r_ax.b.l, FALSE, 0x00);
		}
		break;

	//int 0x10/ah=0x0b: Function selected by bh - None implemented
	case 0x0b:
		break;

	//int 0x10/ah=0x0e: Teletype output
	case 0x0e:
		WriteCharacter(registers->r_ax.b.l, FALSE, 0x00);
		break;

	//int 0x10/ah=0x0f: Get current video mode
	case 0x0f:
		registers->r_ax.b.h = 0x50;					//AH: Number of character columns - TODO
		registers->r_ax.b.l = *(Byte far *)MK_FP(BDA_SEG, BDA_VIDEO_MODE);	//AL: Video mode
		break;

	//int 0x10/ah=0x10: Function selected by al
	case 0x10:
		switch(registers->r_ax.b.l)
		{
		//int 0x10/ax=0x1000: Set palette register
		case 0x00:
			{
				Word ioPortBase = *(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE);
				inportb(ioPortBase + 6);

				outportb(0x3c0, registers->r_bx.b.l);
				outportb(0x3c0, registers->r_bx.b.h);
			}
			break;

		//int 0x10/ax=0x1001: Set border color - Not supported
		case 0x01:
			break;

		//int 0x10/ax=0x1002: Set all palette registers
		case 0x02:
			SetAllPaletteRegisters(registers);
			break;

		//int 0x10/ax=0x1003: Toggle intensity/blinking bit
		case 0x03:
			ToggleBlinking(registers->r_bx.b.l);
			break;

		//int 0x10/ax=0x1010: Set color register
		case 0x10:
			outportb(0x03c8, registers->r_bx.b.l);
			outportb(0x03c9, registers->r_cx.b.h);
			outportb(0x03c9, registers->r_cx.b.l);
			outportb(0x03c9, registers->r_dx.b.h);
			break;

		//int 0x10/ax=0x1012: Set block of color registers
		case 0x12:
			SetColorRegisters(registers);
			break;
		}
		break;

	//int 0x10/ah=0x11: Character generator functions - Unimplemented
	case 0x11:
		break;

	//int 0x10/ah=0x12: Alternate function select, selected by bl
	case 0x12:
		switch(registers->r_bx.b.l)
		{
		//int 0x10/ah=0x12/bl=0x10: Get EGA info
		case 0x10:
			registers->r_ax.b.l = 0x03;	//AL: Unknown
										//BH: Color/Mono mode in effect
			registers->r_bx.b.h =
				(*(Word far *)MK_FP(BDA_SEG, BDA_VGA_IO_PORT_BASE) == 0x3d4) ? 0x00 : 0x01;
			registers->r_bx.b.l = 0x03;	//BL: 256K memory installed
			registers->r_cx.b.h = 0x00;	//CH: Feature connector bits
			registers->r_cx.b.l = 0x09;	//CL: Switch settings
			break;
		}
		break;

	//int 0x10/ah=0x1a: Display combination code functions, selected by al
	case 0x1a:
		switch(registers->r_ax.b.l)
		{
		//int 0x10/ax=0x1a00: Get display combination code
		case 0x00:
			registers->r_bx.w = *(Word far *)MK_FP(EBDA_SEG, EBDA_DISPLAY_CODE);

			//Return "function supported"
			registers->r_ax.b.l = 0x1a;
			break;

		//int 0x10/ax=0x1a01: Set display combination code
		case 0x01:
			*(Word far *)MK_FP(EBDA_SEG, EBDA_DISPLAY_CODE) = registers->r_bx.w;

			//Return "function supported"
			registers->r_ax.b.l = 0x1a;
            break;
		}
		break;

	//int 0x10/ah=0x1b: Functionality/state information - Unimplemented
	case 0x1b:
		break;

	//int 0x10/ah=0x4f: VESA VBE functions - Unimplemented
	case 0x4f:
		break;

	//int 0x10/ah=0x6f: Video7 VGA functions, selected by al - None supported
	case 0x6f:
		break;
	}
}
