//	VMachine
//	Perform a command
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../Pic/Pic.h"
#include "../DmaController/DmaController.h"
#include "FloppyController.h"

void FloppyController::PerformCommand(void)
{
	assert(!commandBytes.empty());

	//Is this a read command?
	if((commandBytes[0] & 0x1f) == 0x06)
	{
		assert(commandBytes.size() == 9);

		//TODO: Drive number checking, data length usage

		//If so, ensure the head number is consistent
#ifdef DEBUG_OUTPUT
		if(commandBytes[3] != (commandBytes[1] & 0x04) >> 2)
		{
			dbgOut << "Read command sent to floppy controller has inconsistent head numbers";
			dbgOut << std::endl;
		}
#endif

		bool success = false;

		//If a disk is inserted
		if(imageFile != 0)
		{
			//Get the offset into the disk image file to use
			//[(cylinder number * 2 + head number) * 18 + (sector number - 1)] * bytes per sector
			Dword imageFileOffset = ((commandBytes[2] * 2 + commandBytes[3]) * 18 +
									(commandBytes[4] - 1)) * 0x200;

			//Get the number of bytes to transfer
			//(final sector + 1 - first sector) * bytes per sector
			Dword numBytes = (commandBytes[6] + 1 - commandBytes[4]) * 0x200;

			//Read this data into a buffer
			//TODO: Overlapped I/O
			std::vector<Byte> dataBuffer(numBytes);

			SetFilePointer(imageFile, imageFileOffset, 0, FILE_BEGIN);

			DWORD numBytesRead;
			ReadFile(imageFile, &dataBuffer[0], numBytes, &numBytesRead, 0);

			//Copy the data to memory
			for(Dword i = 0; i < numBytes; ++i)
				dmaController.WriteByte(2, dataBuffer[i]);

			success = true;
		}

		//Set the result bytes
		//TODO: Correct result byte values
		resultBytes.clear();
		
		resultBytes.push_back(success ? 0x00 : 0x80);

		for(int i = 0; i < 6; ++i)
			resultBytes.push_back(0x00);

		//Clear the command bytes
		commandBytes.clear();

		//Pulse IRQ 6
		masterPic.RaiseIRQ(6);
		masterPic.LowerIRQ(6);
	}

	//Is this a write command?
	else if((commandBytes[0] & 0x3f) == 0x05)
	{
		assert(commandBytes.size() == 9);

		//TODO: Drive number checking, data length usage

		//If so, ensure the head number is consistent
#ifdef DEBUG_OUTPUT
		if(commandBytes[3] != (commandBytes[1] & 0x04) >> 2)
		{
			dbgOut << "Write command sent to floppy controller has inconsistent head numbers";
			dbgOut << std::endl;
		}
#endif

		bool success = false;

		//If a disk is inserted
		if(imageFile != 0)
		{
			//Get the offset into the disk image file to use
			//[(cylinder number * 2 + head number) * 18 + (sector number - 1)] * bytes per sector
			Dword imageFileOffset = ((commandBytes[2] * 2 + commandBytes[3]) * 18 +
									(commandBytes[4] - 1)) * 0x200;

			//Get the number of bytes to transfer
			//(final sector + 1 - first sector) * bytes per sector
			Dword numBytes = (commandBytes[6] + 1 - commandBytes[4]) * 0x200;

			//Copy this data into a buffer
			std::vector<Byte> dataBuffer(numBytes);

			for(Dword i = 0; i < numBytes; ++i)
				dataBuffer[i] = dmaController.ReadByte(2);

			//Write the data to the disk
			//TODO: Overlapped I/O
			SetFilePointer(imageFile, imageFileOffset, 0, FILE_BEGIN);

			DWORD numBytesWritten;
			WriteFile(imageFile, &dataBuffer[0], numBytes, &numBytesWritten, 0);

			success = true;
		}

		//Set the result bytes
		//TODO: Correct result byte values
		resultBytes.clear();
		
		resultBytes.push_back(success ? 0x00 : 0x80);

		for(int i = 0; i < 6; ++i)
			resultBytes.push_back(0x00);

		//Clear the command bytes
		commandBytes.clear();

		//Pulse IRQ 6
		masterPic.RaiseIRQ(6);
		masterPic.LowerIRQ(6);
	}
}
