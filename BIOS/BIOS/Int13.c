//	VMachine BIOS
//	Int 0x13 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void FDReadSectors(struct Registers * registers);
void HDReadSectors(struct Registers * registers);
void FDWriteSectors(struct Registers * registers);
void HDWriteSectors(struct Registers * registers);
void FDGetDriveParameters(struct Registers * registers);
void HDGetDriveParameters(struct Registers * registers);
void FDGetDiskType(struct Registers * registers);
void HDGetDiskType(struct Registers * registers);

void Int13Handler(struct Registers * registers)
{
	//Call the function for this function code
	switch(registers->r_ax.b.h)
	{
	//int 0x13/ah=0x00: Reset disk system - Unimplemented
	case 0x00:
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;
		break;

	//int 0x13/ah=0x02: Read sectors
	case 0x02:
		if((registers->r_dx.b.l & 0x80) == 0x00)
			FDReadSectors(registers);
		else
			HDReadSectors(registers);
		break;

	//int 0x13/ah=0x03: Write sectors
	case 0x03:
		if((registers->r_dx.b.l & 0x80) == 0x00)
			FDWriteSectors(registers);
		else
			HDWriteSectors(registers);
		break;

	//int 0x13/ah=0x04: Verify sectors - Unimplemented
	case 0x04:
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;
		break;

	//int 0x13/ah=0x05: Format track - Unimplemented
	case 0x05:
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;
		break;

	//int 0x13/ah=0x08: Get drive parameters
	case 0x08:
		if((registers->r_dx.b.l & 0x80) == 0x00)
			FDGetDriveParameters(registers);
		else
			HDGetDriveParameters(registers);
		break;

	//int 0x13/ah=0x15: Get disk type
	case 0x15:
		if((registers->r_dx.b.l & 0x80) == 0x00)
			FDGetDiskType(registers);
		else
			HDGetDiskType(registers);
		break;

	//int 0x13/ah=0x18: Set media type for format - Unimplemented
	case 0x18:
		registers->r_ax.b.h = 0x00;
		break;

	//int 0x13/ah=0x20: Get media format - Unimplemented
	case 0x20:
		registers->r_flags &= ~FLAGS_C_FLAG;
		break;

	//int 0x13/ah=0x41/bx=0x55aa: IBM/MS int 0x13 extensions installation check
	case 0x41:
		if(registers->r_bx.w == 0x55aa)
		{
			//Return "extensions not supported"
			registers->r_flags |= FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x01;
			break;
		}
	}
}
