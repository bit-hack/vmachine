//	VMachine
//	Update the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "SoundCard.h"
#include "Ym3812/Ym3812.h"
#include "DigitalSoundProcessor/DigitalSoundProcessor.h"

void SoundCard::UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer)
{
	ym3812->UpdateSoundBuffer(frameNumber, soundBuffer);
	dsp->UpdateSoundBuffer(frameNumber, soundBuffer);
}
