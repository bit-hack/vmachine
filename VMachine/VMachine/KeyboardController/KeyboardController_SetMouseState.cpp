//	VMachine
//	Set the state of the mouse
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"

void KeyboardController::SetMouseState(	SignedWord dx, SignedWord dy,
										bool lButtonDown, bool mButtonDown,	bool rButtonDown)
{
	//Invert dy since Windows places the origin in the top-left, but for hardware mouse
	//movement, positive = up
	dy = -dy;

	//If the mouse is enabled
	if(mouseEnabled)
	{
		//Add the mouse data to the data buffer
		
		//Button data - Yv Xv Ys Xs 1 M R L
		Byte buttonData = 0x08;
		
		if(	((dy & 0x0100) == 0x0000 && (dy & 0xff00) != 0x0000) ||
			((dy & 0x0100) == 0x0100 && (dy & 0xff00) != 0xff00))
		{
			buttonData |= 0x80;
		}

		if(	((dx & 0x0100) == 0x0000 && (dx & 0xff00) != 0x0000) ||
			((dx & 0x0100) == 0x0100 && (dx & 0xff00) != 0xff00))
		{
			buttonData |= 0x40;
		}

		if((dy & 0x0100) == 0x0100)
			buttonData |= 0x20;

		if((dx & 0x0100) == 0x0100)
			buttonData |= 0x10;

		if(mButtonDown)
			buttonData |= 0x04;

		if(rButtonDown)
			buttonData |= 0x02;

		if(lButtonDown)
			buttonData |= 0x01;

		dataBuffer.push_back(std::make_pair(buttonData, true));
		dataBuffer.push_back(std::make_pair(static_cast<Byte>(dx), true));
		dataBuffer.push_back(std::make_pair(static_cast<Byte>(dy), true));

		//Raise any appropriate IRQ
		RaiseIRQ();
	}
}
