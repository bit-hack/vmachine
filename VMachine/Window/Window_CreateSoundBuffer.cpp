//	VMachine
//	Create the sound buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::CreateSoundBuffer(void)
{
	//Fill in structures describing the sound buffer
	WAVEFORMATEX bufferFormat = {0};
	bufferFormat.wFormatTag		= WAVE_FORMAT_PCM;
	bufferFormat.nChannels		= 2;
	bufferFormat.nSamplesPerSec	= 44100;
	bufferFormat.wBitsPerSample	= 16;
	bufferFormat.nBlockAlign	= bufferFormat.nChannels * bufferFormat.wBitsPerSample / 8;
	bufferFormat.nAvgBytesPerSec= bufferFormat.nSamplesPerSec * bufferFormat.nBlockAlign;

	DSBUFFERDESC bufferDesc = {0};
	bufferDesc.dwSize			= sizeof(DSBUFFERDESC);
	bufferDesc.dwFlags			= DSBCAPS_GLOBALFOCUS;
	bufferDesc.dwBufferBytes	= bufferFormat.nAvgBytesPerSec;
	bufferDesc.lpwfxFormat		= &bufferFormat;

    //Create an IDirectSoundBuffer
    IDirectSoundBuffer * tempSoundBuffer;

	if(FAILED(directSound->CreateSoundBuffer(&bufferDesc, &tempSoundBuffer, 0)))
		throw Ex("Window Error: directSound->CreateSoundBuffer failed");

	//Promote to an IDirectSoundBuffer8
	if(FAILED(tempSoundBuffer->QueryInterface(	IID_IDirectSoundBuffer8,
												reinterpret_cast<LPVOID *>(&soundBuffer))))
	{
		throw Ex("Window Error: tempSoundBuffer->QueryInterface failed");
	}

	//Release the temporary sound buffer
	tempSoundBuffer->Release();

	//Zero the contents of the sound buffer
	Dword numSoundBufferBytes;
	void * soundBufferDataPtr;

	if(FAILED(soundBuffer->Lock(0, 0, &soundBufferDataPtr, &numSoundBufferBytes, 0, 0, DSBLOCK_ENTIREBUFFER)))
		throw Ex("Window Error: soundBuffer->Lock failed");

	ZeroMemory(soundBufferDataPtr, numSoundBufferBytes);

	if(FAILED(soundBuffer->Unlock(soundBufferDataPtr, numSoundBufferBytes, 0, 0)))
		throw Ex("Window Error: soundBuffer->Unlock failed");
}
