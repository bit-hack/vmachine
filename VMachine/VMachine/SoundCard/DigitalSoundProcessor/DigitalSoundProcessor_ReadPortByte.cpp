//	VMachine
//	Read Digital Sound Processor ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"
#include "../../Pic/Pic.h"

Byte DigitalSoundProcessor::ReadPortByte(Byte offset)
{
	assert(offset == 0x0a || offset == 0x0c || offset == 0x0e);

	switch(offset)
	{
	//Offset 0x0a: Read data
	case 0x0a:
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
				dbgOut << "Unexpected read from sound card DSP read data register" << std::endl;
#endif
				return 0x00;
			}
		}

	//Offset 0x0c: Write buffer status
	case 0x0c:
		{
			//Return "ready for write"
			return 0x00;
		}

	//Offset 0x0e: Read buffer status
	case 0x0e:
		{
			//Reading the "read buffer status" register clears any interrupt
			masterPic.LowerIRQ(5);

			return (readOffset < dataBufferSize) ? 0x80 : 0x00;
		}
	}

	//Shouldn't reach here
	return 0x00;
}
