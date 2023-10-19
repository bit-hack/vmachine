//	VMachine
//	Read video card ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

Byte VideoCard::ReadPortByte(Word address)
{
	//0x03b4 & 0x03b5 / 0x03d4 & 0x03d5: CRTC registers
	//TODO: Distinguish between the two sets of addresses
	if(address == 0x03b4 || address == 0x03d4)
		return ReadCRTCRegisterIndex();

	if(address == 0x03b5 || address == 0x03d5)
		return ReadCRTCRegister();

	//0x03ba / 0x03da: Input status 1 register
	if(address == 0x03ba || address == 0x03da)
		return ReadInputStatus1Register();

	//0x03c1: Attribute registers
	if(address == 0x03c1)
		return ReadAttributeRegister();

	//0x03c2: Input status 0 register
	if(address == 0x03c2)
		return ReadInputStatus0Register();

	//0x03c4 & 0x03c5: Sequencer registers
	if(address == 0x03c4)
		return ReadSequencerRegisterIndex();

	if(address == 0x03c5)
		return ReadSequencerRegister();

	//0x03c9: Color register
	if(address == 0x03c9)
		return ReadColorRegister();

	//0x03ca: Feature control register
	if(address == 0x03ca)
		return ReadFeatureControlRegister();

	//0x03cc: Miscellaneous output register
	if(address == 0x03cc)
		return ReadMiscOutputRegister();

	//0x03ce & 0x03cf: Graphics registers
	if(address == 0x03ce)
		return ReadGraphicsRegisterIndex();

	if(address == 0x03cf)
		return ReadGraphicsRegister();

	return 0xff;
}
