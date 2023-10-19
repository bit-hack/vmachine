//	VMachine
//	Update the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"
#include "../../Pic/Pic.h"
#include "../../DmaController/DmaController.h"

void DigitalSoundProcessor::UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer)
{
	if(!speakerOn || dmaPaused)
		return;

	switch(commandPlaying)
	{
	case 0x14:	//8-bit single-cycle DMA output
	case 0x1c:	//8-bit auto-init DMA output
		{
			double samplingRate = 256000000.0 /
				(0x10000 - (static_cast<Dword>(timeConstant) << 8));
			
			double sampleSize = 44100.0 / samplingRate;

			double samplePos = 0.0;

			Dword samplesToUpdate;

			if(commandPlaying == 0x14)
				samplesToUpdate = std::min(soundLength, soundBuffer.numSamples);
			else if(commandPlaying == 0x1c)
				samplesToUpdate = soundBuffer.numSamples;

			SignedWord data = 0;

			for(Dword i = 0; i < samplesToUpdate; ++i)
			{
				if(samplePos >= 0.0)
				{
					samplePos -= sampleSize;

					Byte data8 = dmaController.ReadByte(1);
					data = (static_cast<SignedWord>(data8) - 0x80) << 8;

					if(--soundLength == 0)
					{
						if(commandPlaying == 0x14)
							commandPlaying = 0x00;
						else if(commandPlaying == 0x1c)
							soundLength = blockTransferSize;

						masterPic.RaiseIRQ(5);
					}
				}

				++samplePos;

				soundBuffer.data[i * 2    ] += data;
				soundBuffer.data[i * 2 + 1] += data;
			}
		
			break;
		}
	}
}
