//	VMachine
//	Handle an interrupt
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "Pic/Pic.h"

void VMachine::HandleInterrupt(void)
{
	//Get the interrupt number
	Byte intNum = masterPic->AcknowledgeInt();

	if((intNum & 0x07) == 2)
		intNum = slavePic->AcknowledgeInt();

	DoInt(intNum, INT_TYPE_HARDWARE, 0);
}
