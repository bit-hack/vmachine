//	VMachine BIOS
//	Int 0x76 (IRQ 14 - IDE controller) handler
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "..\Common\BIOS.h"

void Int76Handler(struct Registers * registers)
{
	//Set the "int 0x76 received" flag in the EBDA
	*(Byte far *)MK_FP(EBDA_SEG, EBDA_INT76_RECVD) = 0x01;

	//Send EOI to the both PICs
	outportb(0xa0, 0x20);
	outportb(0x20, 0x20);
}
