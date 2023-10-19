//	VMachine
//	Sound card Digital Sound Processor class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class Pic;
class DmaController;

class DigitalSoundProcessor : private boost::noncopyable
{
public:

	//Constructor/Destructor
	DigitalSoundProcessor(	Dword samplingRate_, Pic & masterPic_,
							DmaController & dmaController_);
	~DigitalSoundProcessor();

	//Copy constructor/copy assignment operator handled by boost::noncopyable

	//Access ports
	Byte ReadPortByte(Byte offset);
	void WritePortByte(Byte offset, Byte data);

	//Update the sound buffer
	void UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer);

private:

	//Sampling rate
	Dword samplingRate;

	//Master PIC and DMA controller used by this DSP
	Pic & masterPic;
	DmaController & dmaController;

	//Is the DSP in reset?
	bool inReset;

	//Perform a command
	void StartCommand(Byte newCommand);
	void CompleteCommand(void);

	Byte command;

	//Is the speaker on?
	bool speakerOn;

	//Time constant
	Byte timeConstant;

	//Block transfer size
	Dword blockTransferSize;

	//Sound length
	Dword soundLength;

	//Which command is playing?
	Byte commandPlaying;

	//Is DMA mode paused?
	bool dmaPaused;

	//Data buffer
	static const std::vector<Byte>::size_type dataBufferSize = 16;
	std::vector <Byte> dataBuffer;
	std::vector<Byte>::size_type readOffset, writeOffset;
};
