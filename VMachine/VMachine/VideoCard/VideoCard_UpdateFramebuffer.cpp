//	VMachine
//	Update the framebuffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "CharacterSets.h"
#include "VideoCard.h"

void VideoCard::UpdateFramebuffer(Dword frameNumber, FramebufferData & framebuffer)
{
	assert(framebuffer.width == width);
	assert(framebuffer.height == height);

	//Default to no scaling of the framebuffer
	framebuffer.hScale = framebuffer.vScale = 1;

	if(graphicsMode)
	{
		//Loop through rows
		Dword numRows = height / characterHeight;

		for(Dword row = 0; row < numRows; ++row)
		{
			if(shiftRegister == 0)
			{
				//Loop through groups of 8 pixels
				for(Dword i = 0; i < width / 8; ++i)
				{
					//If these pixels do not require updating, move on to the next
					if(	!regenerateFramebuffer &&
						!videoMemoryDirty[startAddress + row * (displayPitch * 2) + i] &&
						!videoMemoryDirty[videoMemoryPlaneSize +
											startAddress + row * (displayPitch * 2) + i] &&
						!videoMemoryDirty[2 * videoMemoryPlaneSize +
											startAddress + row * (displayPitch * 2) + i] &&
						!videoMemoryDirty[3 * videoMemoryPlaneSize +
											startAddress + row * (displayPitch * 2) + i])
					{
						continue;
					}

					//Extract the data from the video memory
					Byte data[4] = {0};
                    
					for(int j = 0; j < 4; ++j)
					{
						if(	colorPlaneEnabled[j] &&
							startAddress + row * (displayPitch * 2) + i < videoMemoryPlaneSize)
						{
							data[j] = videoMemory[j * videoMemoryPlaneSize +
													startAddress + row * (displayPitch * 2) + i];
						}
					}

					//Calculate the color register indices for each pixel
					Byte indices[8];

					indices[0] =	(data[3] & 0x80) >> 4 |
									(data[2] & 0x80) >> 5 |
									(data[1] & 0x80) >> 6 |
									(data[0] & 0x80) >> 7;

					indices[1] =	(data[3] & 0x40) >> 3 |
									(data[2] & 0x40) >> 4 |
									(data[1] & 0x40) >> 5 |
									(data[0] & 0x40) >> 6;

					indices[2] =	(data[3] & 0x20) >> 2 |
									(data[2] & 0x20) >> 3 |
									(data[1] & 0x20) >> 4 |
									(data[0] & 0x20) >> 5;

					indices[3] =	(data[3] & 0x10) >> 1 |
									(data[2] & 0x10) >> 2 |
									(data[1] & 0x10) >> 3 |
									(data[0] & 0x10) >> 4;

					indices[4] =	(data[3] & 0x08) << 0 |
									(data[2] & 0x08) >> 1 |
									(data[1] & 0x08) >> 2 |
									(data[0] & 0x08) >> 3;

					indices[5] =	(data[3] & 0x04) << 1 |
									(data[2] & 0x04) << 0 |
									(data[1] & 0x04) >> 1 |
									(data[0] & 0x04) >> 2;

					indices[6] =	(data[3] & 0x02) << 2 |
									(data[2] & 0x02) << 1 |
									(data[1] & 0x02) << 0 |
									(data[0] & 0x02) >> 1;

					indices[7] =	(data[3] & 0x01) << 3 |
									(data[2] & 0x01) << 2 |
									(data[1] & 0x01) << 1 |
									(data[0] & 0x01) << 0;

					//Use the indices to index into the color registers
					if(internalPalette4Bit)
					{
						for(int j = 0; j < 8; ++j)
							framebuffer.data[row * framebuffer.pitch + i * 8 + j] =
								colorRegisters[colorSelect76 | colorSelect54 |
												(attributeRegisters[indices[j]] & 0x0f)];
					}
					else
					{
						for(int j = 0; j < 8; ++j)
							framebuffer.data[row * framebuffer.pitch + i * 8 + j] =
								colorRegisters[colorSelect76 | attributeRegisters[indices[j]]];
					}
				}
			}
			else if(shiftRegister == 1)
			{
				//TODO
			}
			else	//shiftRegister == 2
			{
				//Loop through pairs of pixels
				Dword numPairs = width / 2;
				Dword * pFramebuffer = &framebuffer.data[row * framebuffer.pitch];

				for(Dword pixel = 0; pixel < numPairs; ++pixel)
				{
					//Extract the data from the video memory
					//TODO: Where does this x8 come from?
					Dword pixelOffset = row * displayPitch * 8 + pixel;
					Byte data = 0x00;

					if(startAddress + (pixelOffset / 4) < videoMemoryPlaneSize)
						data = videoMemory[(pixelOffset & 3) * videoMemoryPlaneSize +
												startAddress + (pixelOffset / 4)];
					
					//Use the data to index into the color registers
					Dword color = colorRegisters[data];

					*pFramebuffer++ = color;
				}

				//Scale the display horixontally by 2
				framebuffer.hScale = 2;
			}
		}

		//Scale the display vertically by the character height
		framebuffer.vScale = (characterHeight != 0) ? characterHeight : 1;
	}
	else
	{
        //Alphanumeric mode
		//Loop through the characters on screen
		Dword charNum = 0;

		for(Byte row = 0; row < 25; ++row)
		{
			for(Byte column = 0; column < charactersPerLine; ++column, ++charNum)
			{
				//Read the character and attribute byte
				Byte character = 0x00, attribute = 0x00;
				
				if(startAddress + charNum < videoMemoryPlaneSize)
				{
					character = videoMemory[startAddress + charNum];
					attribute = videoMemory[videoMemoryPlaneSize + startAddress + charNum];
				}

				//Does this character blink?
				bool blink = blinkEnabled && (attribute & 0x80) == 0x80;

				//If this character does not require updating, move on to the next
				if(	!blink &&
					!regenerateFramebuffer &&
					!videoMemoryDirty[startAddress + charNum] &&
					!videoMemoryDirty[videoMemoryPlaneSize + startAddress + charNum])
				{
					continue;
				}

				videoMemoryDirty[startAddress + charNum] = false;
				videoMemoryDirty[videoMemoryPlaneSize + startAddress + charNum] = false;

                //Get the foreground and background colors to use
				Dword foregroundColor, backgroundColor;

				Byte backgroundMask = blinkEnabled ? 0x70 : 0xf0;

				if(internalPalette4Bit)
				{
					foregroundColor =
						colorRegisters[colorSelect76 | colorSelect54 |
							(attributeRegisters[attribute & 0x0f] & 0x0f)];
					backgroundColor =
						colorRegisters[colorSelect76 | colorSelect54 |
							(attributeRegisters[(attribute & backgroundMask) >> 4] & 0x0f)];
				}
				else
				{
					foregroundColor =
						colorRegisters[colorSelect76 | attributeRegisters[attribute & 0x0f]];
					backgroundColor =
						colorRegisters[colorSelect76 | attributeRegisters[(attribute & backgroundMask) >> 4]];
				}

				//Loop through lines in the character
				for(Byte line = 0; line < characterHeight; ++line)
				{
					//Get the data for this line of this character
					Byte data = 0x00;

					if(characterHeight == 8)
						data = CharacterSets::set8x8[character * 8 + line];
					
					else if(characterHeight == 16)
						data = CharacterSets::set8x16[character * 16 + line];

					//Loop through pixels in the line
					Dword * pPixel = &framebufferCache[
										(row * characterHeight + line) * 1024 +
											column * characterWidth];

					for(Byte pixel = 0; pixel < 8; ++pixel)
					{
						//If the character blinks and is currently off, draw the background color,
						//otherwise calculate which color to draw
						if(blink && (frameNumber & 0x10) == 0x00)
							*pPixel++ = backgroundColor;
						else
							*pPixel++ = ((data & (1 << (7-pixel))) == (1 << (7-pixel))) ?
											foregroundColor : backgroundColor;
					}

					//If the character width is 9, draw the ninth pixel
					if(characterWidth == 9)
					{
						//If line graphics characters are enabled, and the character
						//is between 0xc0 and 0xdf, replicate the 8th pixel, otherwise
						//fill with the background color
						if(lineGraphicsEnabled && character >= 0xc0 && character < 0xe0)
							*pPixel++ = (data & 1) == 1 ? foregroundColor : backgroundColor;
						else
							*pPixel++ = backgroundColor;
					}
				}
			}
		}

		//Copy the framebuffer cache into the frame buffer
		for(Dword line = 0; line < static_cast<Dword>(25 * characterHeight); ++line)
		{
			memcpy(	&framebuffer.data[line * framebuffer.pitch],
					&framebufferCache[line * 1024],
					charactersPerLine * characterWidth * 4);
		}

		regenerateFramebuffer = false;
	}

	//Convert the first half of the screen to fill the full screen if necessary
	if(convert200To400)
		framebuffer.vScale *= 2;
}
