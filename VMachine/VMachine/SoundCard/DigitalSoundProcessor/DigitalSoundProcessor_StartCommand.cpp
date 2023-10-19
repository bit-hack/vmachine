//	VMachine
//	Start performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"

void DigitalSoundProcessor::StartCommand(Byte newCommand)
{
	switch(newCommand)
	{
	case 0x14:	//8-bit single-cycle DMA output
		{
			command = newCommand;

			readOffset = dataBufferSize;
			writeOffset = dataBufferSize - 2;

			break;
		}

	case 0x1c:	//8-bit auto-init DMA output
		{
			commandPlaying = newCommand;
			command = 0x00;
			dmaPaused = false;

			break;
		}

	case 0x40:	//Set time constant
		{
			command = newCommand;

			readOffset = dataBufferSize;
			writeOffset = dataBufferSize - 1;

			break;
		}

	case 0x48:	//Set DSP block transfer size
		{
			command = newCommand;

			readOffset = dataBufferSize;
			writeOffset = dataBufferSize - 2;

			break;
		}

	case 0xaa:	//Unknown. Windows 95 requires this to return 0x55
		{
			command = newCommand;

			readOffset = dataBufferSize - 1;
			writeOffset = dataBufferSize;

			dataBuffer[readOffset] = 0x55;

			break;
		}

	case 0xd0:	//Pause DMA mode
		{
			dmaPaused = true;
			break;
		}

	case 0xd1:	//Turn on speaker
		{
			speakerOn = true;
			break;
		}

	case 0xd3:	//Turn off speaker
		{
			speakerOn = false;
			break;
		}

	case 0xd4:	//Continue DMA mode
		{
			dmaPaused = false;
			break;
		}

	case 0xe1:	//Get DSP version number
		{
			command = newCommand;

			//Return 2.0
			readOffset = dataBufferSize - 2;
			writeOffset = dataBufferSize;

			dataBuffer[readOffset] = 0x02;
			dataBuffer[readOffset + 1] = 0x00;

			break;
		}

#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unsupported command 0x" << static_cast<Dword>(newCommand);
			dbgOut << " sent to sound card DSP" << std::endl;
		}
#endif
	}
}
