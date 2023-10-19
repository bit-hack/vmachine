//	VMachine
//	Read floppy controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "FloppyController.h"

Byte FloppyController::ReadPortByte(Byte offset)
{
	assert(offset == 4 || offset == 5);

	//Offset 4: Main status register
	if(offset == 4)
		return 0x00;

	//Offset 5: Data register
	else
	{
		//If result bytes are present, return the first, otherwise return 0xff
		if(!resultBytes.empty())
		{
			Byte retVal = resultBytes.front();
			resultBytes.pop_front();

			return retVal;
		}
		else
			return 0xff;
	}
}
