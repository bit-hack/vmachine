//	VMachine
//	Digital Sound Processor constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "DigitalSoundProcessor.h"

//Constructor
DigitalSoundProcessor::DigitalSoundProcessor(	Dword samplingRate_, Pic & masterPic_,
												DmaController & dmaController_)
	:	samplingRate(samplingRate_), masterPic(masterPic_), dmaController(dmaController_),
		inReset(false), command(0x00),
		speakerOn(false), timeConstant(0x00), blockTransferSize(0),
		soundLength(0), commandPlaying(0x00), dmaPaused(false),
		dataBuffer(dataBufferSize), readOffset(dataBufferSize), writeOffset(dataBufferSize)
{

}

//Destructor
DigitalSoundProcessor::~DigitalSoundProcessor()
{

}
