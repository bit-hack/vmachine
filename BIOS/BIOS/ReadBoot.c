//	VMachine BIOS
//	Read the boot sector into memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void FDReadSectors(struct Registers * registers);
void HDReadSectors(struct Registers * registers);

Byte ReadBootSector(void)
{
	struct Registers registers;

	//Try to read the boot sector from the floppy disk
	registers.r_ax.w = 0x0201;
	registers.r_cx.w = 0x0001;
	registers.r_dx.w = 0x0000;

	registers.r_es = 0x0000;
	registers.r_bx.w = 0x7c00;

	FDReadSectors(&registers);

	//If successful, return drive 0x00
	if((registers.r_flags & FLAGS_C_FLAG) == 0)
		return 0x00;

	//Otherwise, read the boot sector from the hard disk
	registers.r_ax.w = 0x0201;
	registers.r_cx.w = 0x0001;
	registers.r_dx.w = 0x0080;

	registers.r_es = 0x0000;
	registers.r_bx.w = 0x7c00;

	HDReadSectors(&registers);

	//Return drive 0x80
	return 0x80;
}
