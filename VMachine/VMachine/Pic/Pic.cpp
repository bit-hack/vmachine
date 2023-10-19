//	VMachine
//	Programmable interrupt controller constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Pic.h"

//Constructor
Pic::Pic(bool isMaster_, VMachine & vmachine_, Byte vectorOffset_) :
	isMaster(isMaster_), vmachine(vmachine_), otherPic(0), vectorOffset(vectorOffset_),
	irqsRaised(0), irr(0), imr(0), isr(0), levelTriggered(false),
	nextIcw(1), icw4Required(false), readIsr(false)
{
	
}

//Destructor
Pic::~Pic()
{

}
