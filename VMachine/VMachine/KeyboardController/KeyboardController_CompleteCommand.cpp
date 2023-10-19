//	VMachine
//	Complete performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"
#include "../VMachine.h"

void KeyboardController::CompleteCommand(void)
{
	switch(command)
	{
	//Port 0x64, 0x60: Write command byte
	case (PORT_64_COMMAND | 0x60):
		{
			assert(numExtraCommandBytes == 1);
			assert(commandBuffer.size() == 1);

			commandByte = commandBuffer[0];

			keyboardEnabled = (commandByte & 0x10) != 0;
			mouseEnabled = (commandByte & 0x20) != 0;
			keyboardIntEnabled = (commandByte & 0x01) != 0;
			mouseIntEnabled = (commandByte & 0x02) != 0;

			command = 0x00;

			commandBuffer.clear();
			numExtraCommandBytes = 0;

			break;
		}

	//Port 0x64, 0xd1: Write output byte
	case (PORT_64_COMMAND | 0xd1):
		{
			assert(numExtraCommandBytes == 1);
			assert(commandBuffer.size() == 1);

			Byte data = commandBuffer[0];

			if((data & 0x02) == 0x00)
				vmachine.DisableA20Line();
			else
				vmachine.EnableA20Line();

			command = 0x00;

			commandBuffer.clear();
			numExtraCommandBytes = 0;

			break;
		}

	//Port 0x64, 0xd4: Write to mouse
	case (PORT_64_COMMAND | 0xd4):
		{
			assert(numExtraCommandBytes == 1);
			assert(commandBuffer.size() == 1);

			Byte data = commandBuffer[0];

			switch(data)
			{
			//0xf2: Read mouse ID
			case 0xf2:
				dataBuffer.clear();

				//Return a zero mouse ID
				dataBuffer.push_back(std::make_pair(0x00, false));
				dataBuffer.push_back(std::make_pair(0x00, false));

				break;

#ifdef DEBUG_OUTPUT
			default:
				dbgOut << "Unsupported data 0x" << static_cast<Dword>(data);
				dbgOut << " sent to mouse" << std::endl;
#endif
			}

			command = 0x00;

			commandBuffer.clear();
			numExtraCommandBytes = 0;

			break;
		}

#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unexpected command 0x" << static_cast<Dword>(command);
			dbgOut << " in KeyboardController::CompleteCommand" << std::endl;
		}
#endif
	}
}
