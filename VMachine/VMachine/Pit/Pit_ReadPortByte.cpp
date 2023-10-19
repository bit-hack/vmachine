//	VMachine
//	Read PIT ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Pit.h"

Byte Pit::ReadPortByte(Byte offset)
{
	assert(offset < 3);

	switch(offset)
	{
	//Offset 0: Timer 0
	case 0:
		return timer0.ReadByte();

	//Offset 1: Timer 1
	case 1:
		return timer1.ReadByte();

	//Offset 2: Timer 2
	case 2:
		return timer2.ReadByte();
	}

	//Shouldn't reach here
	return 0x00;
}
