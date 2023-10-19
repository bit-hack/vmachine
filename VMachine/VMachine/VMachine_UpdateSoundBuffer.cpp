//	VMachine
//	Update the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "SoundCard/SoundCard.h"
#include "VMachine.h"

void VMachine::UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer)
{
	soundCard->UpdateSoundBuffer(frameNumber, soundBuffer);
}
