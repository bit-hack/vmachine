//	VMachine
//	Insert/eject a floppy disk
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "FloppyController/FloppyController.h"

void VMachine::InsertFloppyDisk(const std::string & imageFilename)
{
	floppyController->InsertDisk(imageFilename);
}

void VMachine::EjectFloppyDisk(void)
{
	floppyController->EjectDisk();
}

bool VMachine::IsFloppyDiskInserted(void)
{
	return floppyController->IsDiskInserted();
}
