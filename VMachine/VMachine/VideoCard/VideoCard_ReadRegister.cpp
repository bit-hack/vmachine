//	VMachine
//	Read video card registers
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

Byte VideoCard::ReadMiscOutputRegister(void)
{
	return miscOutputRegister;
}

Byte VideoCard::ReadFeatureControlRegister(void)
{
#ifdef DEBUG_OUTPUT
	dbgOut << "Read of unimplemented video card feature control register" << std::endl;
#endif

	return 0x00;
}

Byte VideoCard::ReadInputStatus0Register(void)
{
#ifdef DEBUG_OUTPUT
	dbgOut << "Read of unimplemented video card input status 0 register" << std::endl;
#endif

	return 0x00;
}

Byte VideoCard::ReadInputStatus1Register(void)
{
	//Clear the attribute register flip-flop
	attributeRegisterFlipFlopData = false;

	//0000X10X
	//  \/|||`- Display Enable (0 => display mode, 1 => h/v-blank)
	//  | ||`-- Light pen not triggered
	//  | |`--- Light pen switch not pushed
	//  | `---- Vertical Retrace
	//  `------ Diagnostic (?)

	//TODO: Correct timing for vertical retrace; Horizontal retrace
#ifdef DEBUG_OUTPUT
	dbgOut << "Read of video card input status 1 register - implementation incomplete" << std::endl;
#endif

	return ((rand() % 2) == 0) ? 0x04 : 0x0d;
}

Byte VideoCard::ReadSequencerRegisterIndex(void)
{
	return sequencerIndexRegister;
}

Byte VideoCard::ReadSequencerRegister(void)
{
	if(sequencerIndexRegister < numSequencerRegisters)
		return sequencerRegisters[sequencerIndexRegister];
	
	return 0xff;
}


Byte VideoCard::ReadCRTCRegisterIndex(void)
{
	return CRTCIndexRegister;
}

Byte VideoCard::ReadCRTCRegister(void)
{
	if(CRTCIndexRegister < numCRTCRegisters)
		return CRTCRegisters[CRTCIndexRegister];
	
	return 0xff;
}

Byte VideoCard::ReadGraphicsRegisterIndex(void)
{
	return graphicsIndexRegister;
}

Byte VideoCard::ReadGraphicsRegister(void)
{
	if(graphicsIndexRegister < numGraphicsRegisters)
		return graphicsRegisters[graphicsIndexRegister];
	
	return 0xff;
}

Byte VideoCard::ReadAttributeRegister(void)
{
	Byte retVal = 0xff;

	if(!attributeRegisterFlipFlopData)
		retVal = attributeIndexRegister;
	else
	{
		if(attributeIndexRegister < numAttributeRegisters)
			retVal = attributeRegisters[attributeIndexRegister];
	}

	return retVal;
}

Byte VideoCard::ReadColorRegister(void)
{
	assert(colorRegisterComponent < 3);

	//TODO: What if colorRegisterWriteMode == true?
	Byte data;
	
	switch(colorRegisterComponent)
	{
	case 0:
		data = static_cast<Byte>((colorRegisters[colorRegisterIndex] & 0xff0000) >> 18);
		break;

	case 1:
		data = static_cast<Byte>((colorRegisters[colorRegisterIndex] & 0x00ff00) >> 10);
		break;

	case 2:
		data = static_cast<Byte>((colorRegisters[colorRegisterIndex] & 0x0000ff) >> 2);
		break;
	}

	colorRegisterComponent = (colorRegisterComponent + 1) % 3;
	
	if(colorRegisterComponent == 0)
		++colorRegisterIndex;

	return data;
}