//	VMachine
//	Sound card class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Pic;
class DmaController;

class Ym3812;
class DigitalSoundProcessor;

class SoundCard
{
public:

	//Constructor/Destructor
	SoundCard(Pic & masterPic_, DmaController & dmaController_);
	~SoundCard();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Access ports
	Byte ReadPortByte(Word base, Byte offset);
	void WritePortByte(Word base, Byte offset, Byte data);

	//Update the sound buffer
	void UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer);

private:

	//YM3812 FM synthesizer
	std::unique_ptr<Ym3812> ym3812;

	//Digital Audio Controller
	std::unique_ptr<DigitalSoundProcessor> dsp;
};
