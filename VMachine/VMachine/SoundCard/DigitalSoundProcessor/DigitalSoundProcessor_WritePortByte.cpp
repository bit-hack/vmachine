//	VMachine
//	Write Digital Sound Processor ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"

void DigitalSoundProcessor::WritePortByte(Byte offset, Byte data)
{
	assert(offset == 0x06 || offset == 0x0c);

	switch(offset)
	{
	case 0x06:	//DSP reset
		{
			if(data != 0)
			{
				commandPlaying = 0x00;

				inReset = true;
			}
			else
			{
				if(inReset)
				{
					//If we are coming out of reset, return a data byte of 0xaa
					readOffset = dataBufferSize - 1;
					writeOffset = dataBufferSize;

					dataBuffer[readOffset] = 0xaa;
				}

				inReset = false;
			}

			break;
		}

	case 0x0c:	//Write command/data
		{
			//If data is expected, add it to the buffer
			if(writeOffset < dataBufferSize)
			{
				dataBuffer[writeOffset++] = data;

				if(writeOffset == dataBufferSize)
					CompleteCommand();
			}
			else
			{
				//Otherwise, this is a command
				StartCommand(data);
			}
		}
	}
}
