//	VMachine
//	Set the state of the mouse
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController/KeyboardController.h"
#include "VMachine.h"

void VMachine::SetMouseState(	SignedWord dx, SignedWord dy,
								bool lButtonDown, bool mButtonDown,	bool rButtonDown)
{
	keyboardController->SetMouseState(dx, dy, lButtonDown, mButtonDown, rButtonDown);
}
