//	VMachine
//	Lock/unlock the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

SoundBufferData Window::LockSoundBuffer(Dword frameNumber)
{
	//If this is the first call
	if(frameNumber == 1)
	{
		//Initialise the write cursor
		writeCursor = 2205;

		//Initialise the play cursor
		lastLockPlayCursor = 0;
		if(FAILED(soundBuffer->SetCurrentPosition(0)))
			throw Ex("Window Error: soundBuffer->SetCurrentPosition");

		//Begin playing the buffer
		soundBuffer->Play(0, 0, DSBPLAY_LOOPING);
		soundBufferStarted = true;

		//Return 0 samples locked
		return SoundBufferData(0);
	}

	//Otherwise, lock the number of samples played since the last call
	DWORD playCursor;
	soundBuffer->GetCurrentPosition(&playCursor, 0);
	playCursor /= 4;

	Dword samplesToLock;
	
	if(playCursor >= lastLockPlayCursor)
		samplesToLock = playCursor - lastLockPlayCursor;
	else
		samplesToLock = 44100 + playCursor - lastLockPlayCursor;

	//Update the last lock play cursor
	lastLockPlayCursor = playCursor;

	//If no samples to lock, return that fact
	if(samplesToLock == 0)
		return SoundBufferData(0);

	//Lock the sound buffer
	if(FAILED(soundBuffer->Lock(writeCursor * 4, samplesToLock * 4,
								&lockPtr1, &lockNumBytes1, &lockPtr2, &lockNumBytes2, 0)))
	{
		//If the lock failed, 0 samples locked
		return SoundBufferData(0);
	}

	//Update the write cursor
	writeCursor += samplesToLock;
	writeCursor %= 44100;

	//Return a buffer to fill
	return SoundBufferData((lockNumBytes1 + lockNumBytes2) / 4);
}

void Window::UnlockSoundBuffer(const SoundBufferData & soundBufferData)
{
	if(soundBufferData.numSamples != 0)
	{
		assert(soundBufferData.numSamples == (lockNumBytes1 + lockNumBytes2) / 4);

		//Copy the data into the sound buffer
		SignedWord * samplePtr1 = reinterpret_cast<SignedWord *>(lockPtr1);
		SignedWord * samplePtr2 = reinterpret_cast<SignedWord *>(lockPtr2);

		for(Dword i = 0; i < lockNumBytes1 / 2; ++i)
		{
			SignedDword data = soundBufferData.data[i];

			if(data > 0x7fff)
				data = 0x7fff;

			if(data < -0x8000)
				data = -0x8000;

			*samplePtr1++ = static_cast<SignedWord>(data);
		}

		for(Dword i = 0; i < lockNumBytes2 / 2; ++i)
		{
			SignedDword data = soundBufferData.data[lockNumBytes1 / 2 + i];

			if(data > 0x7fff)
				data = 0x7fff;

			if(data < -0x8000)
				data = -0x8000;

			*samplePtr2++ = static_cast<SignedWord>(data);
		}

        soundBuffer->Unlock(lockPtr1, lockNumBytes1, lockPtr2, lockNumBytes2);
	}
}
