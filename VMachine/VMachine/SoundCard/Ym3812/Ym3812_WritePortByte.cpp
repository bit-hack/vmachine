//	VMachine
//	Write YM3812 ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"

void Ym3812::WritePortByte(Byte offset, Byte data)
{
	assert(offset == 0 || offset == 1);

	//Offset 0: Address register
	if(offset == 0)
	{
		addressRegister = data;
	}

	//Offset 1: Data register
	else
	{
		switch(addressRegister)
		{
		//0x02: Timer 1 data
		case 0x02:
			timer1Data = data;
			break;

		//0x03: Timer 2 data
		case 0x03:
			timer2Data = data;
			break;

		//0x04: Timer control byte
		//XXX---XX
		//|||   |`- Timer 1 start
		//|||   `-- Timer 2 start
		//||`------ Timer 2 mask
		//|`------- Timer 1 mask
		//`-------- IRQ reset (If set, other bits ignored)
		case 0x04:
			{
				if((data & 0x80) != 0)
				{
					timerIrq = timer1Expired = timer2Expired = false;
					timer1ExpiryTime = timer2ExpiryTime = 0xffffffffffffffff;
					break;
				}

				Qword currentTime = timer.GetTime();

				timer1Masked = (data & 0x40) != 0;
				timer2Masked = (data & 0x20) != 0;

				//TODO: Support pausing of timers?
				if((data & 0x02) != 0)
					timer2ExpiryTime = currentTime + (0x100 - timer2Data) * 320;

				if((data & 0x01) != 0)
					timer1ExpiryTime = currentTime + (0x100 - timer1Data) * 80;

				break;
			}

			//Add non-timer register writes to the vector
		default:
			{
				RegisterWrite r;
				r.time = timer.GetTime();
				r.address = addressRegister;
				r.data = data;

				registerWrites.push_back(r);
			}
		}
	}
}
