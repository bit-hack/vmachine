//	VMachine
//	Start performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../Pic/Pic.h"
#include "IdeController.h"

namespace
{
	const Word driveIdentity[256] =
	{
		0x0040, 0x0000, 0x0000, 0x0000, 0x0000, 0x0200, 0x0000, 0x0000,
		0x0000, 0x0000, 'VM', 'HD', '00', '01', '  ', '  ',
		'  ', '  ', '  ', '  ', 0x0003, 0x0010, 0x0004, '1.',
		'0 ', '  ', '  ', 'VM', 'ac', 'hi', 'ne', ' H',
		'ar', 'd ', 'Di', 'sk', '  ', '  ', '  ', '  ',
		'  ', '  ', '  ', '  ', '  ', '  ', '  ', 0x0001,
		0x0001,
		0x0000, //TODO: Capabilities - LBA & DMA
		0x0000, 0x0200, 0x0200, 0x0001, 0x0000, 0x0000,
		0x0000, 0x0000, 0x0000, 0x0100,	0x0000, 0x0000, 0x0000, 0x0000
	};
}

void IdeController::StartCommand(Byte newCommand)
{
	switch(newCommand)
	{
	case 0x10:	//Recalibrate - Unimplemented
		{
			slavePic.RaiseIRQ(6);

			break;
		}

	case 0x20:	//Read Sector(s) (w/ retry)
	case 0x21:	//Read Sector(s) (w/o retry)
		{
			if(drive == 0)
			{
				command = newCommand;

				DoRead(1);

				readOffset = dataBufferSize - 0x200;
				writeOffset = dataBufferSize;
				dataRequest = true;
				slavePic.RaiseIRQ(6);
			}

			break;
		}
	
	case 0x30:	//Write Sector(s) (w/ retry)
	case 0x31:	//Write Sector(s) (w/o retry)
		{
			if(drive == 0)
			{
				command = newCommand;

				readOffset = dataBufferSize;
				writeOffset = dataBufferSize - 0x200;
				dataRequest = true;
			}
			break;
		}

	case 0x91:	//Initialise Drive Parameters - Unimplemented
		{
			slavePic.RaiseIRQ(6);

			break;
		}

	case 0xc6:	//Set Multiple Mode
		{
			//TODO: Raise an error is sectorCount is incorrect
			if(sectorCount <= 16 && IsPowerOf2OrZero(sectorCount))
				multipleSectorCount = static_cast<Byte>(sectorCount);

			slavePic.RaiseIRQ(6);

			break;
		}

	case 0xec:	//Identify Drive
		{
			if(drive == 0)
			{
				command = newCommand;

				//Copy the drive identity to the data buffer
				for(int i = 0; i < 256; ++i)
				{
					Word data = driveIdentity[i];

					//Patch in the number of cylinders, heads and sectors etc
					if(i == 1 || i == 54)
						data = static_cast<Word>(hdGeometry.numCylinders);

					if(i == 3 || i == 55)
						data = static_cast<Word>(hdGeometry.numHeads);

					if(i == 4)
						data = static_cast<Word>(hdGeometry.numSectors) * 0x200;	//Bytes per track

					if(i == 5 || i == 56)
						data = static_cast<Word>(hdGeometry.numSectors);

					if(i == 57 || i == 60)
						data = static_cast<Word>(hdGeometry.numSectorsTotal);

					if(i == 58 || i == 61)
						data = static_cast<Word>(hdGeometry.numSectorsTotal >> 16);

					if(i == 59)
						data = 0x100 | multipleSectorCount;

					dataBuffer[dataBufferSize - 0x200 + i * 2    ] = static_cast<Byte>(data);
					dataBuffer[dataBufferSize - 0x200 + i * 2 + 1] = static_cast<Byte>(data >> 8);
				}
	            
				readOffset = dataBufferSize - 0x200;
				writeOffset = dataBufferSize;
				dataRequest = true;
				slavePic.RaiseIRQ(6);
			}

			break;
		}
	
#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unsupported command 0x" << static_cast<Dword>(newCommand);
			dbgOut << " sent to IDE controller" << std::endl;
		}
#endif
	}
}