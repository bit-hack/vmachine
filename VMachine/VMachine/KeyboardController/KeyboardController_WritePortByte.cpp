//	VMachine
//	Write keyboard controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"

void KeyboardController::WritePortByte(Byte offset, Byte data)
{
	assert(offset == 0 || offset == 4);

	if(offset == 0)
	{
		//If a command is in progress, add the data to the command buffer
		if(commandBuffer.size() < numExtraCommandBytes)
		{
			commandBuffer.push_back(data);
			
			if(commandBuffer.size() == numExtraCommandBytes)
				CompleteCommand();
		}
		else
		{
			//Otherwise, this is a new command
			StartCommand(data);
		}
	}
	else
	{
		//Offset 4: This is a new command
		StartCommand(PORT_64_COMMAND | data);
	}
}
