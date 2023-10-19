//	VMachine
//	Raise any appropriate IRQ
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"
#include "../Pic/Pic.h"

void KeyboardController::RaiseIRQ(void)
{
	if(!dataBuffer.empty())
	{
		if(!dataBuffer[0].second && keyboardIntEnabled)
			masterPic.RaiseIRQ(1);

		else if(dataBuffer[0].second && mouseIntEnabled)
			slavePic.RaiseIRQ(4);
	}
}
