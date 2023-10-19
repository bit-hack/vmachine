//	VMachine
//	Set a PIT timer's mode
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "PitTimer.h"

void PitTimer::SetMode(DataMode newDataMode, Byte newMode, bool newBcd)
{
	dataMode = newDataMode;
	mode = newMode;
	bcd = newBcd;	//TODO: Handle bcd = true

	nextByteMsb = false;

#ifdef DEBUG_OUTPUT
	if(mode == 0 || mode == 1 || mode == 4 || mode == 5)
	{
		dbgOut << "Unimplemented PIT timer mode " << static_cast<Dword>(mode);
		dbgOut << " selected" << std::endl;
	}
#endif
}