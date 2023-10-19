//	VMachine
//	Do a write
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "IdeController.h"

void IdeController::DoWrite(Word numSectors)
{
	assert(numSectors <= 256);

	//Get the offset into the disk image file to use
	Dword imageFileOffset =
		((cylinder * hdGeometry.numHeads + head) * hdGeometry.numSectors +
			(sector - 1)) *	0x200;

	//Write the data
	//TODO: Overlapped I/O
	SetFilePointer(hdImageFile, imageFileOffset, 0, FILE_BEGIN);

	Dword numBytesWritten;
	WriteFile(	hdImageFile, &dataBuffer[dataBufferSize - numSectors * 0x200],
				numSectors * 0x200, &numBytesWritten, 0);

	//Advance cylinder, head and sector
	for(Dword i = 0; i < numSectors; ++i)
	{
		++sector;

		if(sector == hdGeometry.numSectors + 1)
		{
			sector = 1;
			++head;

			if(head == hdGeometry.numHeads)
			{
				head = 0;
				++cylinder;
			}
		}
	}

	sectorCount -= numSectors;
}