//	VMachine
//	Programmable interrupt controller IRQ control functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../VMachine.h"
#include "Pic.h"

void Pic::RaiseIRQ(Byte irq)
{
	assert(irq < 8);

	//If we are configured for level triggering, or if this IRQ was previously not raised,
	//set the corresponding bit in the irr
	//HACK: Force level-triggering of the mouse IRQ
	if((isMaster && irq == 2) || (!isMaster && irq == 4) || levelTriggered || (irqsRaised & (1 << irq)) == 0)
		irr |= (1 << irq);

	//Update irqsRaised
	irqsRaised |= (1 << irq);

	//If the irr has an unmasked request of higher priority than any in the isr,
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

void Pic::LowerIRQ(Byte irq)
{
	assert(irq < 8);

	//Update irqsRaised
	irqsRaised &= ~(1 << irq);

	//HACK: Force level-triggering of the mouse IRQ
	if((isMaster && irq == 2) || (!isMaster && irq == 4) || levelTriggered)
		irr &= ~(1 << irq);
}