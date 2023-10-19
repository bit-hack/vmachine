//	VMachine
//	Complete performing a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"

void DigitalSoundProcessor::CompleteCommand(void)
{
	assert(readOffset == dataBufferSize);
	assert(writeOffset == dataBufferSize);

	switch(command)
	{
	case 0x14:	//8-bit single-cycle DMA output
		{
			soundLength = *reinterpret_cast<Word *>(&dataBuffer[dataBufferSize - 2]);

			commandPlaying = command;
			command = 0x00;
			dmaPaused = false;
			break;
		}

	case 0x40:	//Set time constant
		{
			timeConstant = dataBuffer[dataBufferSize - 1];

			command = 0x00;
			break;
		}

	case 0x48:	//Set DSP block transfer size
		{
			blockTransferSize = *reinterpret_cast<Word *>(&dataBuffer[dataBufferSize - 2]);
			soundLength = blockTransferSize;

			command = 0x00;
			break;
		}

	case 0xaa:	//Unknown
		{
			command = 0x00;
			break;
		}

	case 0xe1:	//Get DSP version number
		{
			command = 0x00;
			break;
		}

#ifdef DEBUG_OUTPUT
	default:
		{
			dbgOut << "Unexpected command 0x" << static_cast<Dword>(command);
			dbgOut << " in DigitalSoundProcessor::CompleteCommand" << std::endl;
		}
#endif
	}
}
