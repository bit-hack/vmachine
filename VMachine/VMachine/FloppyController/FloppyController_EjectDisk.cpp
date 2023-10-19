//	VMachine
//	Floppy controller eject disk function
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "FloppyController.h"

void FloppyController::EjectDisk(void)
{
	if(imageFile != 0)
		CloseHandle(imageFile);

	imageFile = 0;
}