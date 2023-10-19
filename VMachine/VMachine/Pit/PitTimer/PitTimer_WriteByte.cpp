//	VMachine
//	Write to a PIT timer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "PitTimer.h"

void PitTimer::WriteByte(Byte data)
{
	//Update the initial counter value
	switch(dataMode)
	{
	case DATA_MODE_LSB:
		initialCounter = data;
		break;

	case DATA_MODE_MSB:
		initialCounter = static_cast<Word>(data) << 8;
		break;

	case DATA_MODE_16BIT:
		if(nextByteMsb)
		{
			initialCounter &= 0x00ff;
			initialCounter |= static_cast<Word>(data) << 8;
		}
		else
		{
			initialCounter &= 0xff00;
			initialCounter |= data;
		}

		nextByteMsb = !nextByteMsb;

		break;
	}
}