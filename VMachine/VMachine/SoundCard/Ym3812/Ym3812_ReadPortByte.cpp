//	VMachine
//	Read YM3812 ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"

Byte Ym3812::ReadPortByte(Byte offset)
{
	assert(offset == 0 || offset == 1);

	//Offset 0: Status register
	//XXX00000
	//||`------ Timer 2 expired
	//|`------- Timer 1 expired
	//`-------- Timer IRQ
	if(offset == 0)
	{
		//Update timer expiries
		//TODO: Use virtual (rather than real) time for the timers?
		Qword currentTime = timer.GetTime();

		if(currentTime > timer1ExpiryTime)
			timer1Expired = true;

		if(currentTime > timer2ExpiryTime)
			timer2Expired = true;

		if((timer1Expired && !timer1Masked) || (timer2Expired && !timer2Masked))
			timerIrq = true;

		//Return the status byte
		Byte status = 0x00;
		
		if(timerIrq)
			status |= 0x80;

		if(timer1Expired)
			status |= 0x40;

		if(timer2Expired)
			status |= 0x20;

		return status;
	}

	//Offset 1: Write-only
	return 0x00;
}
