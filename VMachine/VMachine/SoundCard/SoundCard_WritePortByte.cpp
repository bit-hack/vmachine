//	VMachine
//	Write sound card ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "SoundCard.h"
#include "Ym3812/Ym3812.h"
#include "DigitalSoundProcessor/DigitalSoundProcessor.h"

void SoundCard::WritePortByte(Word base, Byte offset, Byte data)
{
	assert(base == 0x220 || base == 0x388);

	if(base == 0x220)
	{
		assert(offset < 0x10);

		switch(offset)
		{
		case 0x08:	case 0x09:
			ym3812->WritePortByte(offset - 8, data);
			break;

		case 0x06:	case 0x0c:
			dsp->WritePortByte(offset, data);
			break;
		}
	}
	else
	{
		assert(offset == 0 || offset == 1);

		ym3812->WritePortByte(offset, data);
	}
}
