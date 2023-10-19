//	VMachine
//	Advance the emulation by one millisecond
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "Pit/Pit.h"

void VMachine::AdvanceOneMs(Dword msNumber, Dword instrPerMs)
{
	//Once per second, update the partPageInvalidations array
	if((msNumber % 1000) == 0)
		UpdatePartPageInvalidations();

	//Execute the instructions for this millisecond
	registers->numInstr = instrPerMs;
	ExecuteInstructions();

	//Update the PIT
	pit->Update();
}
