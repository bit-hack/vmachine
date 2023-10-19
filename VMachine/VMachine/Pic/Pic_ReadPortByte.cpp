//	VMachine
//	Read programmable interrupt controller ports
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Pic.h"

Byte Pic::ReadPortByte(Byte offset)
{
	assert(offset == 0 || offset == 1);

	//If the offset is 0, return irr or isr
	if(offset == 0)
		return readIsr ? isr : irr;

	//If the offset is 1, return the imr
	else
		return imr;
}
