//	VMachine
//	Read from a PIT timer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "PitTimer.h"

Byte PitTimer::ReadByte(void)
{
#ifdef PIT_TIMER_READ_RANDOM
	
	return static_cast<Byte>(rand());

#else

	//If any bytes are latched, read one
	if(numBytesLatched > 0)
	{
		Byte data = static_cast<Byte>(latchedData);

		latchedData >>= 8;
		--numBytesLatched;

		return data;
	}

	//Otherwise, read the counter directly
	switch(dataMode)
	{
	case DATA_MODE_LSB:
		return static_cast<Byte>(counter);

	case DATA_MODE_MSB:
		return static_cast<Byte>(counter >> 8);

	case DATA_MODE_16BIT:
		{
			Byte data;

			if(nextByteMsb)
				data = static_cast<Byte>(counter >> 8);
			else
				data = static_cast<Byte>(counter);

			nextByteMsb = !nextByteMsb;

			return data;
		}
	}

	return 0x00;

#endif
}
