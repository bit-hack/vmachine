//	VMachine BIOS
//	Get drive parameters (int 0x13/ah=0x08)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void FDGetDriveParameters(struct Registers * registers)
{
	//If the drive number is zero, return the drive parameters
	if(registers->r_dx.b.l == 0x00)
	{
		registers->r_bx.b.l = 0x04;	//1.44MB drive TODO: Does this change on presence of a 720K disk?
        registers->r_cx.b.h = 0x4f;	//Maximum cylinder number
		registers->r_cx.b.l = 0x12;	//Maximum sector number
		registers->r_dx.b.h = 0x01;	//Maximum head number
		registers->r_dx.b.l = 0x01;	//Number of drives

		//TODO: es:di = pointer to drive parameter table

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;

		return;
	}

	//If the drive number is one, return zero parameters and that there is only one drive
	if(registers->r_dx.b.l == 0x01)
	{
		registers->r_bx.b.l = 0x00;
        registers->r_cx.b.h = 0x00;
		registers->r_cx.b.l = 0x00;
		registers->r_dx.b.h = 0x00;
		registers->r_dx.b.l = 0x01;	//Number of drives

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;

		return;
	}

	//Otherwise, return failure
	registers->r_flags |= FLAGS_C_FLAG;
	registers->r_ax.b.h = 0x01;
}

void HDGetDriveParameters(struct Registers * registers)
{
	//If the drive number is 0x80, return the drive parameters
	if(registers->r_dx.b.l == 0x80)
	{
		Word numCylinders = *(Word far *)MK_FP(BIOS_SEG, HARD_DISK_NUM_CYLINDERS_OFFSET);
		Word numHeads = *(Word far *)MK_FP(BIOS_SEG, HARD_DISK_NUM_HEADS_OFFSET);
		Word numSectors = *(Word far *)MK_FP(BIOS_SEG, HARD_DISK_NUM_SECTORS_OFFSET);

		registers->r_bx.b.l = 0xfe;
        registers->r_cx.b.h = (Byte)(numCylinders - 1);		//Maximum cylinder number [7-0]
		
		//Maximum cyl [9-8], maximum sect
		registers->r_cx.b.l = ((Byte)((numCylinders - 1) >> 8)) << 6 | (Byte)numSectors;
		
		registers->r_dx.b.h = (Byte)(numHeads - 1);			//Maximum head number
		registers->r_dx.b.l = 0x01;							//Number of drives

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;

		return;
	}

	//Otherwise, return failure
	registers->r_flags |= FLAGS_C_FLAG;
	registers->r_ax.b.h = 0x01;
}
