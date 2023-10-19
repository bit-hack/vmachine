//	VMachine
//	Sound card constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "SoundCard.h"
#include "Ym3812/Ym3812.h"
#include "DigitalSoundProcessor/DigitalSoundProcessor.h"

//Constructor
SoundCard::SoundCard(Pic & masterPic_, DmaController & dmaController_)
	:	ym3812(new Ym3812(3579545, 44100)),
		dsp(new DigitalSoundProcessor(44100, masterPic_, dmaController_))
{

}

//Destructor
SoundCard::~SoundCard()
{

}
