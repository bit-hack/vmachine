//	VMachine
//	Set the state of a key
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController/KeyboardController.h"
#include "VMachine.h"

void VMachine::SetKeyState(Byte scancode, bool pressed)
{
	keyboardController->SetKeyState(scancode, pressed);
}
