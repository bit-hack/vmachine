//	VMachine BIOS
//	Get disk type (int 0x13/ah=0x15)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void FDGetDiskType(struct Registers * registers)
{
	//If the drive number is zero, return the disk type
	if(registers->r_dx.b.l == 0x00)
	{
		//TODO: Support change-line?
		registers->r_ax.b.h = 0x01;	//Floppy without change-line support

		//TODO: Return number of sectors?

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;

		return;
	}

	//If the drive number is one, return "no such drive"
	else if(registers->r_dx.b.l == 0x01)
	{
		registers->r_ax.b.h = 0x00;

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;

		return;
	}

	//Otherwise, return failure
	registers->r_flags |= FLAGS_C_FLAG;
}

void HDGetDiskType(struct Registers * registers)
{
	//If the drive number is 0x80, return the disk type and number of sectors
	if(registers->r_dx.b.l == 0x80)
	{
		Dword numSectorsTotal = *(Dword far *)MK_FP(BIOS_SEG, HARD_DISK_NUM_SECTORS_TOTAL_OFFSET);

		registers->r_ax.b.h = 0x03;
        
		registers->r_cx.w = (Word)(numSectorsTotal >> 16);
		registers->r_dx.w = (Word)numSectorsTotal;

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;

		return;
	}

	//Otherwise, return failure
	registers->r_flags |= FLAGS_C_FLAG;
}
