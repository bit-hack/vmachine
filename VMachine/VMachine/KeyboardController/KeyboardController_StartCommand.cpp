//	VMachine
//	Start performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"
#include "../VMachine.h"

void KeyboardController::StartCommand(Word newCommand)
{
	switch(newCommand)
	{
	//Port 0x60, 0xf4: Enable keyboard
	case (0xf4):
		{
			keyboardEnabled = true;

			break;
		}

	//Port 0x64, 0x20: Get command byte
	case (PORT_64_COMMAND | 0x20):
		{
			dataBuffer.resize(0);
			dataBuffer.push_back(std::make_pair(commandByte, false));

			break;
		}

	//Port 0x64, 0x60: Write command byte
	case (PORT_64_COMMAND | 0x60):
		{
			command = newCommand;
			
			numExtraCommandBytes = 1;

			break;
		}

	//Port 0x64, 0xa7: Disable mouse
	case (PORT_64_COMMAND | 0xa7):
		{
			mouseEnabled = false;

			break;
		}

	//Port 0x64, 0xa8: Enable mouse
	case (PORT_64_COMMAND | 0xa8):
		{
			mouseEnabled = true;

			break;
		}

	//Port 0x64, 0xad: Disable keyboard
	case (PORT_64_COMMAND | 0xad):
		{
			keyboardEnabled = false;

			break;
		}

	//Port 0x64, 0xae: Enable keyboard
	case (PORT_64_COMMAND | 0xae):
		{
			keyboardEnabled = true;

			break;
		}

	//Port 0x64, 0xd0: Read output byte
	case (PORT_64_COMMAND | 0xd0):
		{
			command = newCommand;

			Byte outputByte = vmachine.GetA20LineStatus() ? 0xdf : 0xdd;

			dataBuffer.resize(0);
			dataBuffer.push_back(std::make_pair(outputByte, false));

			break;
		}

	//Port 0x64, 0xd1: Write output byte
	case (PORT_64_COMMAND | 0xd1):
		{
			command = newCommand;

			numExtraCommandBytes = 1;

			break;
		}

	//Port 0x64, 0xd4: Write to mouse
	case (PORT_64_COMMAND | 0xd4):
		{
			command = newCommand;

			numExtraCommandBytes = 1;

			break;
		}

#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unsupported command 0x" << static_cast<Dword>(newCommand);
			dbgOut << " sent to keyboard controller" << std::endl;
		}
#endif
	}
}
