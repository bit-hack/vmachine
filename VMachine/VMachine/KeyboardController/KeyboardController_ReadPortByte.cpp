//	VMachine
//	Read keyboard controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"
#include "../Pic/Pic.h"

Byte KeyboardController::ReadPortByte(Byte offset)
{
	assert(offset == 0 || offset == 4);

	//Offset 0: Data read
	if(offset == 0)
	{
		if(!dataBuffer.empty())
		{
			//Get the first byte from the data buffer
			Byte data = dataBuffer[0].first;
			dataBuffer.pop_front();

			//Lower the IRQs
			masterPic.LowerIRQ(1);
			slavePic.LowerIRQ(4);

			//Raise any appropriate IRQ
			RaiseIRQ();

			lastDataByteRead = data;

			return data;
		}
		else
		{
			return lastDataByteRead;
		}
	}

	//Offset 4: Status read
	else
	{
		//Return controller status byte
		//00X1XX0X
		//\/|||||`- Output buffer status (0 = empty)
		//| ||||`-- Input buffer empty
		//| |||`--- Self test completed successfully (TODO)
		//| ||`---- Data/Command sent to controller last (TODO)
		//| |`----- Keyboard not inhibited
		//| `------ Byte in output buffer is from mouse
		//`-------- No parity/timeout errors
		if(dataBuffer.empty())
			return 0x14;

		return dataBuffer[0].second ? 0x35 : 0x15;
	}
}
