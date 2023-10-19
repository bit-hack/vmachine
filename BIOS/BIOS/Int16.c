//	VMachine BIOS
//	Int 0x16 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void GetKeystroke(struct Registers * registers);
void CheckForKeystroke(struct Registers * registers);

void Int16Handler(struct Registers * registers)
{
	//Call the function for this function code
	switch(registers->r_ax.b.h)
	{
	//int 0x16/ah=0x00: Get keystroke
	//int 0x16/ah=0x10: Get enhanced keystroke
	case 0x00:
	case 0x10:
		GetKeystroke(registers);
		break;

	//int 0x16/ah=0x01: Check for keystroke
	//int 0x16/ah=0x11: Check for enhanced keystroke
	case 0x01:
	case 0x11:
		CheckForKeystroke(registers);
		break;

	//int 0x16/ah=0x02: Get shift flags
	case 0x02:
		//Return the shift flags from the BDA
		registers->r_ax.b.l = *(Byte far *)MK_FP(BDA_SEG, BDA_KEYBOARD_FLAGS0);
		break;

	//int 0x16/ah=0x03: Set typematic rate and delay - Unimplemented
	case 0x03:
		break;

	//int 0x16/ah=0x6f: HP Vectra functions, selected by al - None supported
	case 0x6f:
		break;
	}
}
