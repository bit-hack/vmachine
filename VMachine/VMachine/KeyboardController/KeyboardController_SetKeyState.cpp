//	VMachine
//	Set the state of a key
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"

void KeyboardController::SetKeyState(Byte scancode, bool pressed)
{
	//If the keyboard is enabled
	if(keyboardEnabled)
	{
		//Set bit 7 of the scancode if the key was released
		if(!pressed)
			scancode |= 0x80;

		//Add the scancode to the data buffer
		dataBuffer.push_back(std::make_pair(scancode, false));

		//Raise any appropriate IRQ
		RaiseIRQ();
	}
}
