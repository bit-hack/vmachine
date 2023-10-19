//	VMachine
//	Write IDE controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "IdeController.h"

void IdeController::WritePortByte(Word base, Byte offset, Byte data)
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
				if(writeOffset < dataBufferSize)
				{
					dataBuffer[writeOffset++] = data;

					if(writeOffset == dataBufferSize)
						CompleteCommand();
				}
				else
				{
#ifdef DEBUG_OUTPUT
					dbgOut << "Unexpected write to IDE controller data register" << std::endl;
#endif
				}

				break;
			}

		//Offset 1: Write precompensation - unimplemented
		case 1:
			break;

		//Offset 2: Sector count
		case 2:
			sectorCount = (data == 0) ? 0x100 : data;
			break;

		//Offset 3: Sector number
		case 3:
			sector = data;
			break;

		//Offset 4: Cylinder low
		case 4:
			cylinder &= 0xff00;
			cylinder |= data;
			break;

		//Offset 5: Cylinder high
		case 5:
			cylinder &= 0x00ff;
			cylinder |= static_cast<Word>(data) << 8;
			break;

		//Offset 6: Drive/Head
		case 6:
			drive = (data & 0x10) >> 4;
			head = data & 0x0f;
			break;

		//Offset 7: Command
		case 7:
			StartCommand(data);
			break;
		}
	}

	//Base 0x3f0: Control block registers
	else
	{
		assert(offset == 6 || offset == 7);

		switch(offset)
		{
		//Offset 6: Device control - TODO
		case 6:
			break;

		//Offset 7: Unused
		case 7:
			break;
		}
	}
}

void IdeController::WritePortWord(Word base, Byte offset, Word data)
{
	assert(base == 0x1f0 || base == 0x3f0);

	Byte lowByte = static_cast<Byte>(data);
	Byte highByte = static_cast<Byte>(data >> 8);
	
	WritePortByte(base, offset, lowByte);

	if(base == 0x1f0 && offset == 0)
		WritePortByte(base, offset, highByte);
	else if((base == 0x1f0 && offset < 7) || (base == 0x3f0 && offset == 6))
		WritePortByte(base, offset + 1, highByte);
}

void IdeController::WritePortDword(Word base, Byte offset, Dword data)
{
	assert(base == 0x1f0 || base == 0x3f0);

	Word lowWord = static_cast<Word>(data);
	Word highWord = static_cast<Word>(data >> 16);
	
	WritePortWord(base, offset, lowWord);

	if(base == 0x1f0 && offset == 0)
		WritePortWord(base, offset, highWord);
	else if(base == 0x1f0 && offset < 5)
		WritePortWord(base, offset + 2, highWord);
}
