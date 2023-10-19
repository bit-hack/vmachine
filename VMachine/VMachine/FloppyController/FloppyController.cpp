//	VMachine
//	Floppy controller constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "FloppyController.h"

//Constructor
FloppyController::FloppyController(	const std::string & imageFilename, Pic & masterPic_,
									DmaController & dmaController_)
									:	masterPic(masterPic_), dmaController(dmaController_),
										imageFile(0)
{
	if(imageFilename != "")
		InsertDisk(imageFilename);
}

//Destructor
FloppyController::~FloppyController()
{
	EjectDisk();
}
