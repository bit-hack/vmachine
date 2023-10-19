//	VMachine
//	IDE controller constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "IdeController.h"

//Constructor
IdeController::IdeController(	const std::string & hdImageFilename, 
								Pic & slavePic_)
									:	slavePic(slavePic_), hdImageFile(0),
										sectorCount(1), sector(1), cylinder(0),
										drive(0), head(0), command(0),
										dataRequest(false),
										multipleSectorCount(0),
										dataBuffer(dataBufferSize),
										readOffset(dataBufferSize),
										writeOffset(dataBufferSize)
{
	//Open the hard disk image file
	hdImageFile = CreateFileA(	hdImageFilename.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, nullptr,
								OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	
	if(hdImageFile == INVALID_HANDLE_VALUE)
		throw Ex("Internal Error: CreateFile [" + hdImageFilename + "] failed");

	//Ensure the image is of a legal size (contains a whole number of cylinders)
	LARGE_INTEGER fileSize;

	if(!GetFileSizeEx(hdImageFile, &fileSize))
	{
		CloseHandle(hdImageFile);
		throw Ex("Internal Error: GetFileSizeEx [" + hdImageFilename + "] failed");
	}

	//Calculate the disk geometry
	//TODO: Add a more general algorithm
	if(fileSize.QuadPart % (4 * 17 * 512) == 0)
	{
		hdGeometry.numCylinders = static_cast<Dword>(fileSize.QuadPart / (4 * 17 * 512));
		hdGeometry.numHeads = 4;
		hdGeometry.numSectors = 17;
	}
	else
	{
		//Round up numCylinders
		hdGeometry.numCylinders = static_cast<Dword>(
			(fileSize.QuadPart + (16 * 63 * 512) - 1) / (16 * 63 * 512));
		hdGeometry.numHeads = 16;
		hdGeometry.numSectors = 63;
	}

	hdGeometry.numSectorsTotal =
		hdGeometry.numCylinders * hdGeometry.numHeads * hdGeometry.numSectors;
}

//Destructor
IdeController::~IdeController()
{
	assert(hdImageFile);
	CloseHandle(hdImageFile);
}
