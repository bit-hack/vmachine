//	VMachine
//	Write PIT ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Pit.h"

void Pit::WritePortByte(Byte offset, Byte data)
{
	assert(offset < 4);

	switch(offset)
	{
	//Offset 0: Timer 0
	case 0:
		return timer0.WriteByte(data);

	//Offset 1: Timer 1
	case 1:
		return timer1.WriteByte(data);

	//Offset 2: Timer 2
	case 2:
		return timer2.WriteByte(data);

	//Offset 3: Mode control
	case 3:
		{
			//Extract the command, requested mode and count type
			Byte command = data >> 4;
			Byte mode = (data & 0x0e) >> 1;
			bool bcd = (data & 0x01) != 0;

			//Execute the requested command
			switch(command)
			{
			//0x0: Timer 0 latch counter
			case 0x0:
				timer0.LatchCounter();
				break;

			//0x1: Timer 0 set LSB data mode and mode
			case 0x1:
				timer0.SetMode(PitTimer::DATA_MODE_LSB, mode, bcd);
				break;

			//0x2: Timer 0 set MSB data mode and mode
			case 0x2:
				timer0.SetMode(PitTimer::DATA_MODE_MSB, mode, bcd);
				break;

			//0x3: Timer 0 set 16-bit data mode and mode
			case 0x3:
				timer0.SetMode(PitTimer::DATA_MODE_16BIT, mode, bcd);
				break;

			//0x4: Timer 1 latch counter
			case 0x4:
				timer1.LatchCounter();
				break;

			//0x5: Timer 1 set LSB data mode and mode
			case 0x5:
				timer1.SetMode(PitTimer::DATA_MODE_LSB, mode, bcd);
				break;

			//0x6: Timer 1 set MSB data mode and mode
			case 0x6:
				timer1.SetMode(PitTimer::DATA_MODE_MSB, mode, bcd);
				break;

			//0x7: Timer 1 set 16-bit data mode and mode
			case 0x7:
				timer1.SetMode(PitTimer::DATA_MODE_16BIT, mode, bcd);
				break;

			//0x8: Timer 2 latch counter
			case 0x8:
				timer2.LatchCounter();
				break;

			//0x9: Timer 2 set LSB data mode and mode
			case 0x9:
				timer2.SetMode(PitTimer::DATA_MODE_LSB, mode, bcd);
				break;

			//0xa: Timer 2 set MSB data mode and mode
			case 0xa:
				timer2.SetMode(PitTimer::DATA_MODE_MSB, mode, bcd);
				break;

			//0xb: Timer 2 set 16-bit data mode and mode
			case 0xb:
				timer2.SetMode(PitTimer::DATA_MODE_16BIT, mode, bcd);
				break;

			//0xc: Unused
			case 0xc:
				break;

			//0xd: All timers latch counter
			case 0xd:
				timer0.LatchCounter();
				timer1.LatchCounter();
				timer2.LatchCounter();
				break;

			//0xe: All timers latch status
			case 0xe:
				timer0.LatchStatus();
				timer1.LatchStatus();
				timer2.LatchStatus();
				break;

			//0xf: Unused
			case 0xf:
				break;
			}

			break;
		}
	}
}