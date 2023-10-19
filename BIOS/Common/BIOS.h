//	VMachine BIOS
//	Defines required by the BIOS functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include <dos.h>

#define Byte unsigned char
#define Word unsigned short
#define Dword unsigned long

#define SignedByte signed char
#define SignedWord signed short
#define SignedDword signed long

#define BOOL int
#define FALSE 0
#define TRUE 1

struct GenRegBytes { Byte l, h; };
union GenReg { Word w; struct GenRegBytes b; };

struct Registers
{
	Word r_di, r_si, r_bp;
	union GenReg r_bx, r_dx, r_cx, r_ax;
	Word r_es, r_ds;
	Word r_ip, r_cs, r_flags;
};

#define FLAGS_NT_FLAG 0x4000
#define FLAGS_IOPL_FLAG 0x3000
#define FLAGS_IOPL_SHIFT 12
#define FLAGS_O_FLAG 0x0800
#define FLAGS_D_FLAG 0x0400
#define FLAGS_I_FLAG 0x0200
#define FLAGS_T_FLAG 0x0100
#define FLAGS_S_FLAG 0x0080
#define FLAGS_Z_FLAG 0x0040
#define FLAGS_A_FLAG 0x0010
#define FLAGS_P_FLAG 0x0004
#define FLAGS_C_FLAG 0x0001

#define VGA_BIOS_SEG 0xc000
#define VGA_REGISTER_VALUES_OFFSET			0x4c00
#define VGA_COLOR_REGISTER_VALUES_OFFSET	0x5000

#define BIOS_SEG 0xf000
#define SCANCODE_TO_ASCII_TABLE_OFFSET		0xe700
#define HARD_DISK_NUM_CYLINDERS_OFFSET		0xe900
#define HARD_DISK_NUM_HEADS_OFFSET			0xe902
#define HARD_DISK_NUM_SECTORS_OFFSET		0xe904
#define HARD_DISK_NUM_SECTORS_TOTAL_OFFSET	0xe906
#define MEMORY_SIZE_OFFSET					0xe90a

#define BDA_SEG 0x0040

#define BDA_EBDA_SEG					0x000e	//Word
#define BDA_EQUIPMENT_LIST				0x0010	//Word
#define BDA_MEMSIZE						0x0013	//Word
#define BDA_KEYBOARD_FLAGS0				0x0017	//Byte
#define BDA_KEYBOARD_FLAGS1				0x0018	//Byte
#define BDA_KEYBOARD_BUFFER_READ_PTR	0x001a	//Word
#define BDA_KEYBOARD_BUFFER_WRITE_PTR	0x001c	//Word
#define BDA_VIDEO_MODE					0x0049	//Byte
#define BDA_CURSOR_X					0x0050	//Byte
#define BDA_CURSOR_Y					0x0051	//Byte
#define BDA_VGA_IO_PORT_BASE			0x0063	//Word
#define BDA_TIMER_TICK_COUNT			0x006c	//Dword
#define BDA_TIMER_ROLLOVER_FLAG			0x0070	//Byte
#define BDA_KEYBOARD_BUFFER_START		0x0080	//Word
#define BDA_KEYBOARD_BUFFER_END			0x0082	//Word
#define BDA_VIDEO_NUM_ROWS				0x0084	//Byte

#define BDA_KEYBOARD_FLAGS0_RIGHT_SHIFT_PRESSED	0x01
#define BDA_KEYBOARD_FLAGS0_LEFT_SHIFT_PRESSED	0x02
#define BDA_KEYBOARD_FLAGS0_CTRL_PRESSED		0x04
#define BDA_KEYBOARD_FLAGS0_ALT_PRESSED			0x08
#define BDA_KEYBOARD_FLAGS0_SCROLL_LOCK_ACTIVE	0x10
#define BDA_KEYBOARD_FLAGS0_NUM_LOCK_ACTIVE		0x20
#define BDA_KEYBOARD_FLAGS0_CAPS_LOCK_ACTIVE	0x40
#define BDA_KEYBOARD_FLAGS0_INSERT_ACTIVE		0x80

#define BDA_KEYBOARD_FLAGS1_LEFT_CTRL_PRESSED	0x01
#define BDA_KEYBOARD_FLAGS1_LEFT_ALT_PRESSED	0x02
#define BDA_KEYBOARD_FLAGS1_SYSTEM_PRESSED		0x04
#define BDA_KEYBOARD_FLAGS1_SUSPEND_PRESSED		0x08
#define BDA_KEYBOARD_FLAGS1_SCROLL_LOCK_PRESSED	0x10
#define BDA_KEYBOARD_FLAGS1_NUM_LOCK_PRESSED	0x20
#define BDA_KEYBOARD_FLAGS1_CAPS_LOCK_PRESSED	0x40
#define BDA_KEYBOARD_FLAGS1_INSERT_PRESSED		0x80

#define EBDA_SEG 0x9fc0

#define EBDA_INT0E_RECVD 				0x0000	//Byte
#define EBDA_INT76_RECVD 				0x0001	//Byte
#define EBDA_MOUSE_HANDLER_SEGMENT		0x0002	//Word
#define EBDA_MOUSE_HANDLER_OFFSET		0x0004	//Word
#define EBDA_DISPLAY_CODE				0x0006	//Word
