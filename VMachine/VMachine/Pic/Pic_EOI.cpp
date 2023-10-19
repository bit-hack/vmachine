//	VMachine
//	Handle an EOI
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pic.h"

void Pic::NonSpecificEOI(void)
{
	//Clear the highest priority bit in the isr register
	SignedByte lowestIsrBit = GetLowestSetBit(isr);

	if(lowestIsrBit != -1)
		isr &= ~(1 << lowestIsrBit);

	//If the irr has an unmasked request of higher priority than any remaining in the isr,
	//raise the interrupt line
	if((irr & ~imr) != 0)
	{
		if(isr == 0 || GetLowestSetBit((irr & ~imr)) < GetLowestSetBit(isr))
		{
			if(isMaster)
				vmachine.RaiseINT();
			else
				otherPic->RaiseIRQ(2);
		}
	}
}

void Pic::SpecificEOI(Byte level)
{
	//Clear the isr register bit correcponding to level
	isr &= ~(1 << level);

	//If the irr has an unmasked request of higher priority than any remaining in the isr,
	//raise the interrupt line
	if((irr & ~imr) != 0)
	{
		if(isr == 0 || GetLowestSetBit((irr & ~imr)) < GetLowestSetBit(isr))
		{
			if(isMaster)
				vmachine.RaiseINT();
			else
				otherPic->RaiseIRQ(2);
		}
	}
}
