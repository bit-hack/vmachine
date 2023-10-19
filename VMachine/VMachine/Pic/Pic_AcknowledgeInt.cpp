//	VMachine
//	Acknowledge an interrupt
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pic.h"

Byte Pic::AcknowledgeInt(void)
{
	//Move the highest priority bit in the irr register to the isr register
	SignedByte lowestIrrBit = GetLowestSetBit(irr);

	if(lowestIrrBit == -1)
	{
#ifdef DEBUG_OUTPUT
		dbgOut << "irr = 0 in Pic::AcknowledgeInt" << std::endl;
#endif
		lowestIrrBit = 7;
	}

	isr = 1 << lowestIrrBit;
	irr &= ~(1 << lowestIrrBit);

	//Lower the interrupt line
	if(isMaster)
		vmachine.LowerINT();
	else
		otherPic->LowerIRQ(2);

	//Return the interrupt vector
	return vectorOffset | lowestIrrBit;
}