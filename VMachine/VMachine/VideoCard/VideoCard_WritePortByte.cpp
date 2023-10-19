//	VMachine
//	Write video card ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

void VideoCard::WritePortByte(Word address, Byte data)
{
	//Any register write causes the framebuffer to be regenerated
	regenerateFramebuffer = true;

	//0x03b4 & 0x03b5 / 0x03d4 & 0x03d5: CRTC registers
	//TODO: Distinguish between the two sets of addresses
	if(address == 0x03b4 || address == 0x03d4)
	{
		WriteCRTCRegisterIndex(data);
		return;
	}

	if(address == 0x03b5 || address == 0x03d5)
	{
		WriteCRTCRegister(data);
		return;
	}

	//0x03c0: Attribute registers
	if(address == 0x03c0)
	{
		WriteAttributeRegister(data);
		return;
	}

	//0x03c4 & 0x03c5: Sequencer registers
	if(address == 0x03c4)
	{
		WriteSequencerRegisterIndex(data);
		return;
	}

	if(address == 0x03c5)
	{
		WriteSequencerRegister(data);
		return;
	}

	//0x03ba / 0x03da: Feature control register
	if(address == 0x03ba || address == 0x03da)
	{
		WriteFeatureControlRegister(data);
		return;
	}

	//0x03c2: Miscellaneous output register
	if(address == 0x03c2)
	{
		WriteMiscOutputRegister(data);
		return;
	}

	//0x03c7: Color register read index
	if(address == 0x03c7)
	{
		WriteColorRegisterReadIndex(data);
		return;
	}

	//0x03c8: Color register write index
	if(address == 0x03c8)
	{
		WriteColorRegisterWriteIndex(data);
		return;
	}

	//0x03c9: Color register
	if(address == 0x03c9)
	{
		WriteColorRegister(data);
		return;
	}

	//0x03c9: Color register
	if(address == 0x03c9)
	{
		WriteColorRegister(data);
		return;
	}

	//0x03ce & 0x03cf: Graphics registers
	if(address == 0x03ce)
	{
		WriteGraphicsRegisterIndex(data);
		return;
	}

	if(address == 0x03cf)
	{
		WriteGraphicsRegister(data);
		return;
	}
}
