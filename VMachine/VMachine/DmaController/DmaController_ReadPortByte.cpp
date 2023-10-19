//	VMachine
//	Read DMA controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DmaController.h"

Byte DmaController::ReadPortByte(Word base, Byte offset)
{
	assert(base == 0x0000 || base == 0x00c0);

	Byte channelBase = (base == 0x0000) ? 0 : 4;

	switch(offset)
	{
	case 0x0:
	case 0x2:
	case 0x4:
	case 0x6:
		if(!highByte)
		{
			highByte = !highByte;
			return static_cast<Byte>(currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)]);
		}
		else
		{
			highByte = !highByte;
            return static_cast<Byte>(currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)] >> 8);
		}
		break;

	case 0x1:
	case 0x3:
	case 0x5:
	case 0x7:
		if(!highByte)
		{
			highByte = !highByte;
			return static_cast<Byte>(currentCountRegisters[channelBase + ((offset & 0x06) >> 1)]);
		}
		else
		{
			highByte = !highByte;
			return static_cast<Byte>(currentCountRegisters[channelBase + ((offset & 0x06) >> 1)] >> 8);
		}
		break;
	}

	return 0xff;
}
