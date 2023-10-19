//	VMachine
//	Update the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Ym3812.h"

void Ym3812::UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer)
{
	Qword currentTime = timer.GetTime();

	//If this is the first call to UpdateSoundBuffer, or there are no samples to update
	if(frameNumber == 1 || soundBuffer.numSamples == 0)
	{
		//Apply all of the register writes
		for(std::vector <RegisterWrite>::const_iterator i = registerWrites.begin();
			i != registerWrites.end(); ++i)
		{
			ApplyRegisterWrite(i->address, i->data);
		}
	}
	else
	{
		//Otherwise, convert the times stored with the register writes into sample numbers
		for(std::vector <RegisterWrite>::iterator i = registerWrites.begin();
			i != registerWrites.end(); ++i)
		{
			i->numSamples = static_cast<Dword>(	((i->time - lastUpdateTime) * soundBuffer.numSamples) /
												(currentTime - lastUpdateTime));
		}

		//Allocate a temporary data buffer
#ifdef USE_FMOPL
		std::vector <SignedWord> dataBuffer(soundBuffer.numSamples);
#endif

#ifdef USE_YM3812_CHANNEL
		std::vector <SignedDword> dataBuffer(soundBuffer.numSamples);
#endif

		//Loop through the register writes
		Dword sample = 0;

		for(std::vector <RegisterWrite>::const_iterator i = registerWrites.begin();
			i != registerWrites.end(); ++i)
		{
			//Generate samples up to the point of this write
			Dword samplesToGenerate = i->numSamples - sample;
			
			GenerateSamples(samplesToGenerate, &dataBuffer[sample]);

			sample += samplesToGenerate;

			//Apply the register write
			ApplyRegisterWrite(i->address, i->data);
		}

		//Generate the remaining samples
		GenerateSamples(soundBuffer.numSamples - sample, &dataBuffer[sample]);

		//Copy the data to the sound buffer
		for(Dword i = 0; i < soundBuffer.numSamples; ++i)
		{
			soundBuffer.data[i * 2    ] += dataBuffer[i];
			soundBuffer.data[i * 2 + 1] += dataBuffer[i];
		}
	}

	registerWrites.clear();
	lastUpdateTime = currentTime;
}
