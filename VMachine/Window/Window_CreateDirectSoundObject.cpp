//	VMachine
//	Create the DirectSound object
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::CreateDirectSoundObject(void)
{
	//Create the DirectSound object
	if(FAILED(DirectSoundCreate8(0, &directSound, 0)))
		throw Ex("Window Error: DirectSoundCreate8 failed");

	//Set normal cooperative level
	if(FAILED(directSound->SetCooperativeLevel(hWnd, DSSCL_NORMAL)))
		throw Ex("Window Error: directSound->SetCooperativeLevel failed");
}
