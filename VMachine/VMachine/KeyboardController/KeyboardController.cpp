//	VMachine
//	Keyboard controller constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "KeyboardController.h"

//Constructor
KeyboardController::KeyboardController(VMachine & vmachine_, Pic & masterPic_, Pic & slavePic_)
	:	vmachine(vmachine_), masterPic(masterPic_), slavePic(slavePic_),
		lastDataByteRead(0x00), command(0x00), numExtraCommandBytes(0),
		commandByte(0x11), keyboardEnabled(true), mouseEnabled(false),
		keyboardIntEnabled(true), mouseIntEnabled(false)
{

}

//Destructor
KeyboardController::~KeyboardController()
{

}
