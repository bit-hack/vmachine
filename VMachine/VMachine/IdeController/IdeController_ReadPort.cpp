//	VMachine
//	Read IDE controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../Pic/Pic.h"
#include "IdeController.h"

Byte IdeController::ReadPortByte(Word base, Byte offset)
{
	assert(base == 0x1f0 || base == 0x3f0);

	//Base 0x1f0: Command block registers
	if(base == 0x1f0)
	{
		assert(offset < 8);

		switch(offset)
		{
		//Offset 0: Data
		case 0:
			{
				if(readOffset < dataBufferSize)
				{
					Byte data = dataBuffer[readOffset++];

					if(readOffset == dataBufferSize)
						CompleteCommand();

					return data;
				}
				else
				{
#ifdef DEBUG_OUTPUT
					dbgOut << "Unexpected read from IDE controller data register" << std::endl;
#endif
					return 0x00;
				}
			}

		//Offset 1: Error register
		case 1:
			return 0x00;	//TODO

		//Offset 2: Sector count
		case 2:
			return static_cast<Byte>(sectorCount);

		//Offset 3: Sector number
		case 3:
			return sector;

		//Offset 4: Cylinder low
		case 4:
			return (cylinder & 0xff);

		//Offset 5: Cylinder high
		case 5:
			return static_cast<Byte>(cylinder >> 8);

		//Offset 6: Drive/Head
		case 6:
			return 0xa0 | (drive << 4) | head;

		//Offset 7: Status
		case 7:
			{
				//Reading the status register clears any interrupt
				slavePic.LowerIRQ(6);

				return dataRequest ? 0x58 : 0x50;	//TODO
			}
		}
	}

	//Base 0x3f0: Control block registers
	else
	{
		assert(offset == 6 || offset == 7);

		switch(offset)
		{
		//Offset 6: Alternate status
		case 6:
			return dataRequest ? 0x58 : 0x50;	//TODO

		//Offset 7: Drive address
		case 7:
			return 0xc0 | ((~head & 0x0f) << 2) | ((drive == 1) ? 0x02 : 0x01);
		}
	}

	//Shouldn't reach here
	return 0x00;
}

Word IdeController::ReadPortWord(Word base, Byte offset)
{
	assert(base == 0x1f0 || base == 0x3f0);

	Byte lowByte, highByte;
	
	lowByte = ReadPortByte(base, offset);

	if(base == 0x1f0 && offset == 0)
		highByte = ReadPortByte(base, offset);
	else if((base == 0x1f0 && offset < 7) || (base == 0x3f0 && offset == 6))
		highByte = ReadPortByte(base, offset + 1);
	else
		highByte = 0x00;
	
	return (static_cast<Word>(highByte) << 8) | lowByte;
}

Dword IdeController::ReadPortDword(Word base, Byte offset)
{
	assert(base == 0x1f0 || base == 0x3f0);

	Word lowWord, highWord;
	
	lowWord = ReadPortWord(base, offset);

	if(base == 0x1f0 && offset == 0)
		highWord = ReadPortWord(base, offset);
	else if(base == 0x1f0 && offset < 5)
		highWord = ReadPortWord(base, offset + 2);
	else
		highWord = 0x0000;
	
	return (static_cast<Word>(highWord) << 16) | lowWord;
}
