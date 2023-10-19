//	VMachine
//	Write floppy controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "FloppyController.h"

void FloppyController::WritePortByte(Byte offset, Byte data)
{
	assert(offset == 2 || offset == 5);

	//Offset 2: Digital output register
	if(offset == 2)
	{

	}

	//Offset 5: Data register
	else
	{
		//If this is the first command byte
		if(commandBytes.empty())
		{
			//Ensure the command is recognised
			//Read command
			if((data & 0x1f) == 0x06)
			{
				commandBytes.push_back(data);
				numCommandBytes = 9;
			}

			//Write command
			else if((data & 0x3f) == 0x05)
			{
				commandBytes.push_back(data);
				numCommandBytes = 9;
			}
#ifdef DEBUG_OUTPUT
			else
			{
				dbgOut << "Unrecognised command 0x" << static_cast<Dword>(data);
				dbgOut << " written to floppy controller" << std::endl;
			}
#endif
		}
		else
		{
			//Otherwise, add the data to the command bytes
			commandBytes.push_back(data);

			//If this command is now complete, perform it
			if(commandBytes.size() == numCommandBytes)
			{
				PerformCommand();
			}
		}
	}
}