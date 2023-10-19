//	VMachine
//	Apply a register write
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"
#include "fmopl/fmopl.h"
#include "Ym3812Channel/Ym3812Channel.h"

namespace
{
	const Byte offsetToChannel[0x20] =
	{
		0, 1, 2, 0, 1, 2, 0xff, 0xff,
		3, 4, 5, 3, 4, 5, 0xff, 0xff,
		6, 7, 8, 6, 7, 8, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};

	const Byte offsetToOperator[0x20] =
	{
		0, 0, 0, 1, 1, 1, 0xff, 0xff,
		0, 0, 0, 1, 1, 1, 0xff, 0xff,
		0, 0, 0, 1, 1, 1, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
	};
}

void Ym3812::ApplyRegisterWrite(Byte address, Byte data)
{
#ifdef USE_FMOPL
	YM3812Write(opl, 0, address);
	YM3812Write(opl, 1, data);
#endif

#ifdef USE_YM3812_CHANNEL
	Byte opChannel = offsetToChannel[address & 0x1f];
	Byte op = offsetToOperator[address & 0x1f];

	switch(address & 0xf0)
	{
	case 0x20:	case 0x30:
		if(opChannel != 0xff)
			channels[opChannel]->WriteMultipleRegister(op, data);
		break;

	case 0x40:	case 0x50:
		if(opChannel != 0xff)
			channels[opChannel]->WriteTotalLevelRegister(op, data);
		break;

	case 0x60:	case 0x70:
		if(opChannel != 0xff)
			channels[opChannel]->WriteARDRRegister(op, data);
		break;

	case 0x80:	case 0x90:
		if(opChannel != 0xff)
			channels[opChannel]->WriteSLRRRegister(op, data);
		break;

	case 0xa0:
		if((address & 0x0f) < 9)
			channels[address & 0x0f]->WriteFNumberLowRegister(data);
		break;

	case 0xb0:
		if((address & 0x0f) < 9)
			channels[address & 0x0f]->WriteFNumberHighRegister(data);
		break;

	case 0xc0:
		if((address & 0x0f) < 9)
			channels[address & 0x0f]->WriteAlgorithmRegister(data);
		break;

	case 0xe0:	case 0xf0:
		if(opChannel != 0xff)
			channels[opChannel]->WriteWaveformRegister(op, data);
		break;

#ifdef DEBUG_OUTPUT
	default:
		dbgOut << "Unrecognised register 0x" << (Dword)address;
		dbgOut << " in Ym3812::ApplyRegisterWrite" << std::endl;
#endif
	}
#endif
}
