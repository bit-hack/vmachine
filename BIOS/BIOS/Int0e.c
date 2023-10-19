//	VMachine BIOS
//	Int 0x0e (IRQ 6 - floppy controller) handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "..\Common\BIOS.h"

void Int0eHandler(struct Registers * registers)
{
	//Set the "int 0x0e received" flag in the EBDA
	*(Byte far *)MK_FP(EBDA_SEG, EBDA_INT0E_RECVD) = 0x01;

	//Send EOI to the master PIC
	outportb(0x20, 0x20);
}
