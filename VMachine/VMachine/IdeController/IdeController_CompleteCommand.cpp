//	VMachine
//	Complete performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../Pic/Pic.h"
#include "IdeController.h"

void IdeController::CompleteCommand(void)
{
	assert(readOffset == dataBufferSize);

	switch(command)
	{
	case 0x20:	//Read Sector(s) (w/ retry)
	case 0x21:	//Read Sector(s) (w/o retry)
		{
			if(sectorCount != 0)
			{
				DoRead(1);

				readOffset = dataBufferSize - 0x200;
				slavePic.RaiseIRQ(6);
			}
			else
			{
				dataRequest = false;
				command = 0x00;
			}

			break;
		}

	case 0x30:	//Write Sector(s) (w/ retry)
	case 0x31:	//Write Sector(s) (w/o retry)
		{
			DoWrite(1);
			slavePic.RaiseIRQ(6);

			if(sectorCount != 0)
			{
				writeOffset = dataBufferSize - 0x200;
			}
			else
			{
				dataRequest = false;
				command = 0x00;
			}
			
			break;
		}

	case 0xec:	//Identify Drive
		{
			dataRequest = false;
			command = 0x00;
			break;
		}

#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unexpected command 0x" << static_cast<Dword>(command);
			dbgOut << " in IdeController::ContinueCommand" << std::endl;
		}
#endif
	}
}