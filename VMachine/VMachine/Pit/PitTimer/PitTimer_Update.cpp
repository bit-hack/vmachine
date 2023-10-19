//	VMachine
//	Update a PIT timer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "../../Pic/Pic.h"
#include "PitTimer.h"

void PitTimer::Update(void)
{
	switch(mode)
	{
	//Mode 0: Single timeout - unimplemented
	//Mode 1: Retriggerable one-shot - unimplemented

    //Mode 2: Rate generator
	case 2:
	case 6:
		{
			//The counter decrements at 1.193180 MHz
			Word newCounter = counter - 1193;

			//When it hits zero, the output goes high and the counter is reloaded
			if(static_cast<SignedWord>(counter) > 0 && static_cast<SignedWord>(newCounter) <= 0)
			{
				outputLevel = 1;
				counter = initialCounter;
			}
			else
			{
				//Otherwise, the output is low
				outputLevel = 0;
				counter = newCounter;
			}

			break;
		}

	//Mode 3: Square wave
	case 3:
	case 7:
		{
			//The counter decrements at twice 1.193180 MHz
			Word newCounter = counter - 2 * 1193;

			//When it hits zero, the output flips and the counter is reloaded
			if(static_cast<SignedWord>(counter) > 0 && static_cast<SignedWord>(newCounter) <= 0)
			{
				outputLevel = 1 - outputLevel;
				counter = initialCounter;
			}
			else
				counter = newCounter;

			break;
		}

	//Mode 4: Software triggered strobe - unimplemented
	//Mode 5: Hardware retriggerable strobe - unimplemented
	}

	//If this is timer 0, update IRQ0 to reflect the new output level
	assert(outputLevel == 0 || outputLevel == 1);

	if(timer0)
	{
		if(outputLevel == 1)
			masterPic.RaiseIRQ(0);
		else
			masterPic.LowerIRQ(0);
	}
}