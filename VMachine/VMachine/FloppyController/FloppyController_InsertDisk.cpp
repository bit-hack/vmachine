//	VMachine
//	Floppy controller insert disk function
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "FloppyController.h"

void FloppyController::InsertDisk(const std::string & imageFilename)
{
	//Eject the current disk, if any
	EjectDisk();

	//Open the disk image file
	HANDLE tempImageFile = CreateFile(	imageFilename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if(tempImageFile == INVALID_HANDLE_VALUE)
		throw Ex("Internal Error: CreateFile [" + imageFilename + "] failed");

	//Ensure the image is of the correct size
	//TODO: 720K images
	LARGE_INTEGER fileSize;

	if(!GetFileSizeEx(tempImageFile, &fileSize))
	{
		CloseHandle(tempImageFile);
		throw Ex("Internal Error: GetFileSizeEx [" + imageFilename + "] failed");
	}

	if(fileSize.QuadPart != 1440 * 1024)
	{
		CloseHandle(tempImageFile);
		throw ImageIncorrectSizeEx(
			"Floppy Disk Error: " + imageFilename + " is incorrectly sized");
	}

	imageFile = tempImageFile;
}
