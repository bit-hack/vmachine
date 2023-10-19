//	VMachine
//	Read video memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

Byte VideoCard::ReadByte(Dword base, Dword offset)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	//If the read lies outside the video memory area, ignore it
	if(0xa0000 + offset < videoMemoryBase || 0xa0000 + offset > videoMemoryTop)
		return 0xff;

	//Calculate the offset into video memory, given the current base
	offset -= (videoMemoryBase - 0xa0000);

	//If chain4 addressing is enabled
	if(chain4)
	{
		//Read from the memory plane given by the bottom 2 bits
		return videoMemory[(offset & 3) * videoMemoryPlaneSize + (offset / 4)];
	}
	else if(oddEven)
	{
		//Otherwise, if odd/even addressing is enabled, bit 0 distinguishes between odd and
		//even planes
		Dword planeOffset = offset / 2;

		if(planeOffset < videoMemoryPlaneSize)
			return videoMemory[(offset & 1) * videoMemoryPlaneSize + planeOffset];
		else
			return videoMemory[(2 + (offset & 1) - 1) * videoMemoryPlaneSize + planeOffset];
	}
	else
	{
		//Otherwise, read from each plane into the corresponding read data latch
		if(offset < videoMemoryPlaneSize)
		{
			readDataLatches[0] = videoMemory[offset];
			readDataLatches[1] = videoMemory[videoMemoryPlaneSize + offset];
			readDataLatches[2] = videoMemory[videoMemoryPlaneSize * 2 + offset];
			readDataLatches[3] = videoMemory[videoMemoryPlaneSize * 3 + offset];
		}

		//In read mode 0
		if(readMode == 0)
		{
			//Return the value from the memory plane given by the read map select register
			return readDataLatches[readMapSelect];
		}
		else	//In read mode 1
		{
			Byte data = 0x00;

			//Calculate the color register indices for each pixel
			Byte indices[8];

			indices[0] =	(readDataLatches[3] & 0x80) >> 4 | (readDataLatches[2] & 0x80) >> 5 | 
							(readDataLatches[1] & 0x80) >> 6 | (readDataLatches[0] & 0x80) >> 7;
			indices[1] =	(readDataLatches[3] & 0x40) >> 3 | (readDataLatches[2] & 0x40) >> 4 |
							(readDataLatches[1] & 0x40) >> 5 | (readDataLatches[0] & 0x40) >> 6;
			indices[2] =	(readDataLatches[3] & 0x20) >> 2 | (readDataLatches[2] & 0x20) >> 3 |
							(readDataLatches[1] & 0x20) >> 4 | (readDataLatches[0] & 0x20) >> 5;
			indices[3] =	(readDataLatches[3] & 0x10) >> 1 | (readDataLatches[2] & 0x10) >> 2 |
							(readDataLatches[1] & 0x10) >> 3 | (readDataLatches[0] & 0x10) >> 4;
			indices[4] =	(readDataLatches[3] & 0x08) << 0 | (readDataLatches[2] & 0x08) >> 1 |
							(readDataLatches[1] & 0x08) >> 2 | (readDataLatches[0] & 0x08) >> 3;
			indices[5] =	(readDataLatches[3] & 0x04) << 1 | (readDataLatches[2] & 0x04) << 0 |
							(readDataLatches[1] & 0x04) >> 1 | (readDataLatches[0] & 0x04) >> 2;
			indices[6] =	(readDataLatches[3] & 0x02) << 2 | (readDataLatches[2] & 0x02) << 1 |
							(readDataLatches[1] & 0x02) << 0 | (readDataLatches[0] & 0x02) >> 1;
			indices[7] =	(readDataLatches[3] & 0x01) << 3 | (readDataLatches[2] & 0x01) << 2 |
							(readDataLatches[1] & 0x01) << 1 | (readDataLatches[0] & 0x01) << 0;

			//Loop through the pixels
			for(int i = 0; i < 8; ++i)
			{
				//If the index, masked by the "Color Don't Care" register, equals the
				//"Color Compare" register, set the corresponding bit in the return value
				if((indices[i] & colorDontCare) == (colorCompare & colorDontCare))
					data |= (1 << i);
			}

			return data;
		}
	}
}

Word VideoCard::ReadWord(Dword base, Dword offset)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	Byte dataLow = ReadByte(base, offset);
	
	Byte dataHigh = (offset + 1 < 0x20000) ? ReadByte(base, offset + 1) : 0xff;

	return (static_cast<Word>(dataHigh) << 8) | dataLow;
}

Dword VideoCard::ReadDword(Dword base, Dword offset)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	Word dataLow = ReadWord(base, offset);
	
	Word dataHigh = (offset + 2 < 0x20000) ? ReadWord(base, offset + 2) : 0xffff;

	return (static_cast<Dword>(dataHigh) << 16) | dataLow;
}
