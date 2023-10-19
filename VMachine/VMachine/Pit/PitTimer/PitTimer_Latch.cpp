//	VMachine
//	PIT timer latch functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "PitTimer.h"

void PitTimer::LatchCounter(void)
{
	//If no bytes are already latched, latch the value of the counter
	if(numBytesLatched == 0)
	{
		if(dataMode == DATA_MODE_LSB)
		{
			latchedData = counter & 0xff;
			numBytesLatched = 1;
		}

		if(dataMode == DATA_MODE_MSB)
		{
			latchedData = counter >> 8;
			numBytesLatched = 1;
		}

		if(dataMode == DATA_MODE_16BIT)
		{
			latchedData = counter;
			numBytesLatched = 2;
		}
	}
}

void PitTimer::LatchStatus(void)
{
	//If no bytes are already latched, latch a status byte
	if(numBytesLatched == 0)
	{
		//status = ABCCDDDE
		//         ||\/\_/`- BCD?
		//         |||  `--- Mode
		//         ||`------ Data mode
		//         |`------- Count not loaded?
		//         `-------- Output level
		Byte status = (outputLevel << 7) | (mode << 1) | (bcd ? 1 : 0);

		if(dataMode == DATA_MODE_LSB)
			status |= 0x10;

		if(dataMode == DATA_MODE_MSB)
			status |= 0x20;

		if(dataMode == DATA_MODE_16BIT)
			status |= 0x30;

		latchedData = status;
		numBytesLatched = 1;
	}
}
