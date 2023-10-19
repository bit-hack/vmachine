//	VMachine BIOS
//	Int 0x15 handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "../Common/BIOS.h"

void CopyMemory(struct Registers * registers);

void Int15Handler(struct Registers * registers)
{
	//Call the function for this function code
	switch(registers->r_ax.b.h)
	{
	//int 0x15/ah=0x24: A20 functions, selected by al - None implemented
	case 0x24:
		break;

	//int 0x15/ah=0x41: Wait on external event - Not supported
	case 0x41:
		break;

	//int 0x15/ah=0x4d: (al=0xd4) HP 95LX/100LX/200LX installation check - Not supported
	case 0x4d:
		break;

	//int 0x15/ah=0x4f: Keyboard intercept - Unimplemented
	case 0x4f:
		break;

	//int 0x15/ah=0x53: APM/System management bus functions, selected by al - None implemented
	case 0x53:
		break;

	//int 0x15/ah=0x86: Wait - Unimplemented
	case 0x86:
		break;

	//int 0x15/ah=0x87: Copy extended memory
	case 0x87:
		CopyMemory(registers);
		break;

	//int 0x15/ah=0x88: Get extended memory size
	case 0x88:
		{
			//Set ax to the number of contiguous KB starting at 1MB, to a maximum of 15MB
			Word memorySize = *(Word far *)MK_FP(BIOS_SEG, MEMORY_SIZE_OFFSET);
			--memorySize;
		
			if(memorySize > 15)
				memorySize = 15;

			registers->r_ax.w = memorySize * 0x400;

			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
		}
		break;

	//int 0x15/ah=0xbf: DOS/4GW services - Not supported
	case 0xbf:
		break;

	//int 0x15/ah=0xc0: Get system configuration
	case 0xc0:
		//Set es:bx to point to the system configuration table
		registers->r_es = 0xf000;
		registers->r_bx.w = 0xe6f5;

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;
		registers->r_ax.b.h = 0x00;
		break;

	//int 0x15/ah=0xc1: Get EBDA segment
	case 0xc1:
		//Set es to the EBDA segment
		registers->r_es = EBDA_SEG;

		//Return success
		registers->r_flags &= ~FLAGS_C_FLAG;
		break;

	//int 0x15/ah=0xc2: Pointing device functions, selected by al
	case 0xc2:
		switch(registers->r_ax.b.l)
		{
		//int 0x15/ax=0xc200: Enable/Disable mouse
		case 0x00:
			if(registers->r_bx.b.h == 0)
			{
				Byte commandByte;

				//Disable the mouse by clearing bits 5 and 1 of the keyboard controller's
				//command byte
				outportb(0x64, 0x20);
				commandByte = inportb(0x60);
				
				outportb(0x64, 0x60);
				outportb(0x60, commandByte & ~0x22);
			}
			else if(registers->r_bx.b.h == 1)
			{
				Byte commandByte;

				//Enable the mouse by setting bits 5 and 1 of the keyboard controller's
				//command byte
				outportb(0x64, 0x20);
				commandByte = inportb(0x60);
				
				outportb(0x64, 0x60);
				outportb(0x60, commandByte | 0x22);
			}
			break;

		//int 0x15/ax=0xc201: Reset mouse
		case 0x01:
			//Return a zero device ID
			registers->r_bx.w = 0x00aa;
			
			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		//int 0x15/ax=0xc202: Set sample rate
		case 0x02:
			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		//int 0x15/ax=0xc203: Set resolution
		case 0x03:
			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		//int 0x15/ax=0xc204: Get device ID
		case 0x04:
			//Return a zero device ID
			registers->r_bx.b.h = 0x00;
			
			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		//int 0x15/ax=0xc205: Initialise mouse
		case 0x05:
			//Data package size = 3 bytes
			registers->r_bx.b.h = 3;
			
			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		//int 0x15/ax=0xc206: Extended commands, selected by bh
		case 0x06:
			switch(registers->r_bx.b.h)
			{
			//int 0x15/ax=0xc206/bh=0x00: Return mouse status
			case 0x00:
				//TODO: Correct values for status byte, resolution and sample rate
				registers->r_bx.b.l = 0x00;
				registers->r_cx.b.l = 0x00;
				registers->r_dx.b.l = 0x64;

				//Return success
				registers->r_flags &= ~FLAGS_C_FLAG;
				registers->r_ax.b.h = 0x00;
				break;

			//int 0x15/ax=0xc206/bh=0x01: Set scaling factor to 1:1
			case 0x01:
				//Return success
				registers->r_flags &= ~FLAGS_C_FLAG;
				registers->r_ax.b.h = 0x00;
				break;

			//int 0x15/ax=0xc206/bh=0x01: Set scaling factor to 2:1
			case 0x02:
				//Return success
				registers->r_flags &= ~FLAGS_C_FLAG;
				registers->r_ax.b.h = 0x00;
				break;

			default:
				//Return failure due to invalid subfunction call
				registers->r_flags |= FLAGS_C_FLAG;
				registers->r_ax.b.h = 0x01;
			}
			break;

		//int 0x15/ax=0xc207: Set mouse handler address
		case 0x07:
			//Save the address into the EBDA
			*(Word far *)MK_FP(EBDA_SEG, EBDA_MOUSE_HANDLER_SEGMENT) = registers->r_es;
			*(Word far *)MK_FP(EBDA_SEG, EBDA_MOUSE_HANDLER_OFFSET) = registers->r_bx.w;

			//Return success
			registers->r_flags &= ~FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x00;
			break;

		default:
			//Return failure due to invalid subfunction call
			registers->r_flags |= FLAGS_C_FLAG;
			registers->r_ax.b.h = 0x01;
		}
		break;

	//int 0x15/ah=0xc4: Programmable option select - Not supported
	case 0xc4:
		break;

	//int 0x15/ah=0xd8: EISA system ROM functions, selected by al - None supported
	case 0xd8:
		break;

	//int 0x15/ah=0xe8: Misc functions, selected by al - None implemented
	//Due to a bug, Linux (from at least version 2.4.18 onwards) requires int 0x15/ax=0xe801
	//to explicitly set the carry flag if the function is not supported
	case 0xe8:
		registers->r_flags |= FLAGS_C_FLAG;
		break;

	//int 0x15/ah=0xf9: Unknown - Not supported
	case 0xf9:
		break;
	}
}
