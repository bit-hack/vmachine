//	VMachine
//	Write to DMA controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DmaController.h"

void DmaController::WritePortByte(Word base, Byte offset, Byte data)
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
			baseAddressRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0xff00;
			baseAddressRegisters[channelBase + ((offset & 0x06) >> 1)] |= data;

			currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0xff00;
			currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)] |= data;

			highByte = !highByte;
		}
		else
		{
			baseAddressRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0x00ff;
			baseAddressRegisters[channelBase + ((offset & 0x06) >> 1)] |= static_cast<Word>(data) << 8;

			currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0x00ff;
			currentAddressRegisters[channelBase + ((offset & 0x06) >> 1)] |= static_cast<Word>(data) << 8;

			highByte = !highByte;
		}
		break;

	case 0x1:
	case 0x3:
	case 0x5:
	case 0x7:
		if(!highByte)
		{
			baseCountRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0xff00;
			baseCountRegisters[channelBase + ((offset & 0x06) >> 1)] |= data;

			currentCountRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0xff00;
			currentCountRegisters[channelBase + ((offset & 0x06) >> 1)] |= data;

			highByte = !highByte;
		}
		else
		{
			baseCountRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0x00ff;
			baseCountRegisters[channelBase + ((offset & 0x06) >> 1)] |= static_cast<Word>(data) << 8;

			currentCountRegisters[channelBase + ((offset & 0x06) >> 1)] &= 0x00ff;
			currentCountRegisters[channelBase + ((offset & 0x06) >> 1)] |= static_cast<Word>(data) << 8;

			highByte = !highByte;
		}
		break;

	case 0xb:
		autoInitEnabled[channelBase + (data & 0x03)] = (data & 0x10) != 0;
		break;
	}
}
