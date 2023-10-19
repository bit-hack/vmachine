//	VMachine
//	Is an I/O port access permitted by the I/O permission bitmap?
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

bool VMachine::IsIoPermittedByBitmap(Dword port, Byte accessSize)
{
	if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
														DESCRIPTOR_FLAGS_TYPE_TSS_16_BIT_BUSY)
	{
		//I/O permission bitmap only supported by 32-bit TSS
		return false;
	}
	else if((registers->r_tr.descriptorFlags & DESCRIPTOR_FLAGS_TYPE_MASK) ==
														DESCRIPTOR_FLAGS_TYPE_TSS_32_BIT_BUSY)
	{
		//Get the offset of the base of the I/O permission bitmap
		Word bitmapBaseOffset;
		if(!ReadWordLinear(registers->r_tr.base + 102, bitmapBaseOffset))
			return false;

		//Loop through byte ports to be accessed
		for(Dword bytePort = port; bytePort < port + accessSize; ++bytePort)
		{
			//If this port is beyond the I/O permission bitmap, return false
			if(bitmapBaseOffset + (bytePort / 8) > registers->r_tr.limit)
				return false;

			//Read the byte containing the I/O permission bitmap bit for this port
			Byte bitmapByte;

			if(!ReadByteLinear(	registers->r_tr.base + bitmapBaseOffset + (bytePort / 8),
								bitmapByte))
			{
				return false;
			}

			//If the I/O permission bitmap bit for this port is set, return false
			if((bitmapByte & (1 << (bytePort % 8))) != 0)
				return false;
		}

		return true;
	}
	else
		throw Ex("CPU Error: Unexpected task register type in IsIoPermittedByBitmap");
}
