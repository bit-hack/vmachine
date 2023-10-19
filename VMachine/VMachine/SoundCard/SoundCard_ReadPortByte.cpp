//	VMachine
//	Read sound card ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "SoundCard.h"
#include "Ym3812/Ym3812.h"
#include "DigitalSoundProcessor/DigitalSoundProcessor.h"

Byte SoundCard::ReadPortByte(Word base, Byte offset)
{
	assert(base == 0x220 || base == 0x388);

	if(base == 0x220)
	{
		assert(offset < 0x10);

		switch(offset)
		{
		case 0x08:	case 0x09:
			return ym3812->ReadPortByte(offset - 8);

		case 0x0a:	case 0x0c:	case 0x0e:
			return dsp->ReadPortByte(offset);
		}

		return 0xff;
	}
	else
	{
		assert(offset == 0 || offset == 1);

		return ym3812->ReadPortByte(offset);
	}
}
