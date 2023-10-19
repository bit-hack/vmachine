//	VMachine
//	Write video card registers
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

void VideoCard::WriteMiscOutputRegister(Byte data)
{
	//Misc output register
	//XXX-XXXXb
	//||| |||`-- I/O address
	//||| ||`--- Enable RAM
	//||| ``---- Clock select
	//||`------- Page bit
	//``-------- Sync polarities => framebuffer height
	miscOutputRegister = data;

	height = 400;

	if((data & 0xc0) == 0x80)
		height = 350;

	if((data & 0xc0) == 0xc0)
		height = 480;
}

void VideoCard::WriteFeatureControlRegister(Byte data)
{
#ifdef DEBUG_OUTPUT
	dbgOut << "Byte 0x" << static_cast<Dword>(data);
	dbgOut << " written to unimplemented video card feature control register" << std::endl;
#endif
}

void VideoCard::WriteSequencerRegisterIndex(Byte data)
{
	sequencerIndexRegister = data;
}

void VideoCard::WriteSequencerRegister(Byte data)
{
	if(sequencerIndexRegister < numSequencerRegisters)
		sequencerRegisters[sequencerIndexRegister] = data;

	switch(sequencerIndexRegister)
	{
	//Clocking mode register
	//--XXXX-Xb
	//  |||| `-- Characters 8/9 pixels wide
	//  |||`---- Shift load
	//  ||`----- Dot clock
	//  |`------ Shift 4
	//  `------- Screen off
	case 1:
		characterWidth = ((data & 0x01) == 0) ? 9 : 8;
		width = charactersPerLine * characterWidth;
		break;

	//Map mask register
	//----XXXX
	//    ````- Map mask
	case 2:
		mapMask = data & 0x0f;
		break;

	//Memory mode register
	//----XXX-
	//    ||`-- Extended memory (Unused)
	//    |`--- Odd/Even
	//    `---- Chain 4
	case 4:
		chain4 = (data & 0x08) != 0x00;
		oddEven = (data & 0x04) == 0x00;
		break;
	}
}

void VideoCard::WriteCRTCRegisterIndex(Byte data)
{
	CRTCIndexRegister = data;
}

void VideoCard::WriteCRTCRegister(Byte data)
{
	if(CRTCIndexRegister < numCRTCRegisters)
		CRTCRegisters[CRTCIndexRegister] = data;

	switch(CRTCIndexRegister)
	{
	//Horizontal desplay end (characters per line)
	case 0x01:
		charactersPerLine = data + 1;
		width = charactersPerLine * characterWidth;
		break;

	//Maximum scan lines
	//XXXXXXXXb
	//|||`````-- Character height - 1
	//||`------- Start vertical blanking bit 9
	//|`-------- Line compare bit 9
	//`--------- 200 to 400 line conversion
	case 0x09:
		characterHeight = (data & 0x1f) + 1;
		convert200To400 = (data & 0x80) != 0;
		break;

	//Start address high
	case 0x0c:
		startAddress &= 0x00ff;
		startAddress |= static_cast<Word>(data) << 8;
		break;

	//Start address low
	case 0x0d:
		startAddress &= 0xff00;
		startAddress |= data;
		break;

	//Offset
	case 0x13:
		displayPitch = data;
		break;
	}
}

void VideoCard::WriteGraphicsRegisterIndex(Byte data)
{
	graphicsIndexRegister = data;
}

void VideoCard::WriteGraphicsRegister(Byte data)
{
	if(graphicsIndexRegister < numGraphicsRegisters)
		graphicsRegisters[graphicsIndexRegister] = data;

	switch(graphicsIndexRegister)
	{
	//Set/Reset register
	//----XXXX
	case 0:
		setReset = data & 0x0f;
		break;

	//Enable Set/Reset register
	//----XXXX
	case 1:
		enableSetReset = data & 0x0f;
		break;

	//Color compare register
	//----XXXX
	case 2:
		colorCompare = data & 0x0f;
		break;

	//Data rotate register
	//---XXXXX
	//   ||```- Rotate count
	//   ``---- ALU function
	case 3:
		rotateCount = data & 0x07;
		aluFunction = AluFunction((data & 0x18) >> 3);
		break;

	//Read map select register
	//------XX
	//      ``- Read map select
	case 4:
		readMapSelect = data & 0x03;
		break;

	//Mode register
	//-XXXXXXX
	// |||| ``- Write mode
	// |||`---- Read mode
	// ||`----- Odd/Even
	// ``------ Shift register
	case 5:
		shiftRegister = (data & 0x60) >> 5;
		readMode = (data & 0x80) >> 3;
		writeMode = data & 0x03;
		break;

	//Miscellaneous register
	//----XXXX
	//    \/|`- Graphics/Alphanumeric mode
	//    | `-- Chain odd-even
	//    `---- Memory map
	case 6:
		switch((data & 0x0c) >> 2)
		{
		case 0: videoMemoryBase = 0xa0000; videoMemoryTop = 0xbffff; break;
		case 1: videoMemoryBase = 0xa0000; videoMemoryTop = 0xaffff; break;
		case 2: videoMemoryBase = 0xb0000; videoMemoryTop = 0xb7fff; break;
		case 3: videoMemoryBase = 0xb8000; videoMemoryTop = 0xbffff; break;
		};
		graphicsMode = (data & 0x01) != 0x00;
		break;

	//Color don't care register
	//----XXXX
	case 7:
		colorDontCare = data & 0x0f;
		break;

	//Bit Mask Register
	//XXXXXXXX
	case 8:
		bitMask = data;
		break;
	}
}

void VideoCard::WriteAttributeRegister(Byte data)
{
	if(!attributeRegisterFlipFlopData)
		attributeIndexRegister = data;
	else
	{
		if(attributeIndexRegister < numAttributeRegisters)
		{
			//Palette registers are only 6 bits
			if(attributeIndexRegister < 0x10)
				attributeRegisters[attributeIndexRegister] = data & 0x3f;
			else
				attributeRegisters[attributeIndexRegister] = data;
		}

		switch(attributeIndexRegister)
		{
		//Mode control register
		//XXX-XXXX
		//||| |||`- Graphics/Alphanumeric mode (ignored)
		//||| ||`-- Monochrome/Color attributes
		//||| |`--- Enable line graphics
		//||| `---- Enable blink (vs intensity)
		//||`------ Pixel panning
		//|`------- Pixel clock select
		//`-------- Internal palette 4 bit
		case 0x10:
			lineGraphicsEnabled = (data & 0x04) != 0;
			blinkEnabled = (data & 0x08) != 0;
			internalPalette4Bit = (data & 0x80) != 0;
			break;

		//Color plane enable register
		//----XXXX
		//    ````- Color plane enables
		case 0x12:
			colorPlaneEnabled[0] = (data & 0x01) != 0;
			colorPlaneEnabled[1] = (data & 0x02) != 0;
			colorPlaneEnabled[2] = (data & 0x04) != 0;
			colorPlaneEnabled[3] = (data & 0x08) != 0;
			break;

		//Color select register
		//----XXXX
		//    ||``- Color select bits 5, 4
		//    ``--- Color select bits 7, 6
		case 0x14:
			colorSelect76 = (data & 0x0c) << 4;
			colorSelect54 = (data & 0x03) << 4;
			break;
		}
	}

	attributeRegisterFlipFlopData = !attributeRegisterFlipFlopData;
}

void VideoCard::WriteColorRegisterReadIndex(Byte data)
{
	colorRegisterIndex = data;
	colorRegisterComponent = 0;
	colorRegisterWriteMode = false;
}

void VideoCard::WriteColorRegisterWriteIndex(Byte data)
{
	colorRegisterIndex = data;
	colorRegisterComponent = 0;
	colorRegisterWriteMode = true;
}

void VideoCard::WriteColorRegister(Byte data)
{
	assert(colorRegisterComponent < 3);

	//TODO: What if colorRegisterWriteMode == false?
	switch(colorRegisterComponent)
	{
	case 0:
		colorRegisters[colorRegisterIndex] &= 0x00ffff;
		colorRegisters[colorRegisterIndex] |= static_cast<Dword>(data & 0x3f) << 18;
		break;

	case 1:
		colorRegisters[colorRegisterIndex] &= 0xff00ff;
		colorRegisters[colorRegisterIndex] |= static_cast<Dword>(data & 0x3f) << 10;
		break;

	case 2:
		colorRegisters[colorRegisterIndex] &= 0xffff00;
		colorRegisters[colorRegisterIndex] |= static_cast<Dword>(data & 0x3f) << 2;
		break;
	}

	colorRegisterComponent = (colorRegisterComponent + 1) % 3;
	
	if(colorRegisterComponent == 0)
		++colorRegisterIndex;
}
