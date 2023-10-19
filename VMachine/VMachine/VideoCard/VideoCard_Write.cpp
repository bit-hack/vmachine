//	VMachine
//	Write video memory
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

void VideoCard::WriteByte(Dword base, Dword offset, Byte data)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	//If the write lies outside the video memory area, ignore it
	if(0xa0000 + offset < videoMemoryBase || 0xa0000 + offset > videoMemoryTop)
		return;

	//Calculate the offset into video memory, given the current base
	offset -= (videoMemoryBase - 0xa0000);

	//In write modes 0 and 3, rotate the data
	if(writeMode == 0 || writeMode == 3)
		data = (data << (8 - rotateCount)) | (data >> rotateCount);

	//If chain4 addressing is enabled
	if(chain4)
	{
		//Write to the memory plane given by the bottom 2 bits
		videoMemory[(offset & 3) * videoMemoryPlaneSize + (offset / 4)] = data;
		videoMemoryDirty[(offset & 3) * videoMemoryPlaneSize + (offset / 4)] = true;
	}
	else if(oddEven)
	{
		//Otherwise, if odd/even addressing is enabled, bit 0 distinguishes between odd and
		//even planes
		Dword planeOffset = offset / 2;

		if(planeOffset < videoMemoryPlaneSize)
		{
			videoMemory[(offset & 1) * videoMemoryPlaneSize + planeOffset] = data;
			videoMemoryDirty[(offset & 1) * videoMemoryPlaneSize + planeOffset] = true;
		}
		else
		{
			videoMemory[(2 + (offset & 1) - 1) * videoMemoryPlaneSize + planeOffset] = data;
			videoMemoryDirty[(2 + (offset & 1) - 1) * videoMemoryPlaneSize + planeOffset] = true;
		}
	}
	else
	{
		//Otherwise, write to the memory planes enabled in the map mask register
		if(offset < videoMemoryPlaneSize)
		{
			if(writeMode == 0 || writeMode == 2 || writeMode == 3)
			{
				for(int plane = 0; plane < 4; ++plane)
				{
					if((mapMask & (1 << plane)) != 0)
					{
						Byte aluData;
						Byte mask = bitMask;

						if(writeMode == 0)
						{
							//Apply Set/Reset, if enabled
							aluData = data;

							if((enableSetReset & (1 << plane)) != 0)
								aluData = ((setReset & (1 << plane)) != 0) ? 0xff : 0x00;
						}

						if(writeMode == 2)
						{
							//Convert the data into a set/reset byte
							aluData = ((data & (1 << plane)) != 0) ? 0xff : 0x00;
						}

						if(writeMode == 3)
						{
							//AND the mask with the data
							//TODO: Uncommenting this line breaks highlighted text in Win 95
						//	mask &= data;

							//Apply Set/Reset
							aluData = ((setReset & (1 << plane)) != 0) ? 0xff : 0x00;
						}

						switch(aluFunction)
						{
						case ALU_AND:
							aluData &= readDataLatches[plane];
							break;

						case ALU_OR:
							aluData |= readDataLatches[plane];
							break;

						case ALU_XOR:
							aluData ^= readDataLatches[plane];
							break;
						}

						videoMemory[videoMemoryPlaneSize * plane + offset] =
							(mask & aluData) | (~mask & readDataLatches[plane]);

						videoMemoryDirty[videoMemoryPlaneSize * plane + offset] = true;
					}
				}
			}

			if(writeMode == 1)
			{
				for(int plane = 0; plane < 4; ++plane)
				{
					videoMemory[videoMemoryPlaneSize * plane + offset] =
						readDataLatches[plane];

					videoMemoryDirty[videoMemoryPlaneSize * plane + offset] = true;
				}
			}
		}
	}
}

void VideoCard::WriteWord(Dword base, Dword offset, Word data)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	WriteByte(base, offset, static_cast<Byte>(data));
	
	if(offset + 1 < 0x20000)
		WriteByte(base, offset + 1, static_cast<Byte>(data >> 8));
}

void VideoCard::WriteDword(Dword base, Dword offset, Dword data)
{
	assert(base == 0xa0000);
	assert(offset < 0x20000);

	WriteWord(base, offset, static_cast<Word>(data));
	
	if(offset + 2 < 0x20000)
		WriteWord(base, offset + 2, static_cast<Word>(data >> 16));
}
