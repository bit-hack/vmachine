//	VMachine
//	Get the override segment register to use, if any
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

SegmentRegister * VMachine::GetOverrideSegReg(PrefixBitset prefixes)
{
	if((prefixes & PREFIX_CS) != 0)
		return &r_cs;

	if((prefixes & PREFIX_DS) != 0)
		return &r_ds;

	if((prefixes & PREFIX_ES) != 0)
		return &r_es;

	if((prefixes & PREFIX_FS) != 0)
		return &r_fs;

	if((prefixes & PREFIX_GS) != 0)
		return &r_gs;

	if((prefixes & PREFIX_SS) != 0)
		return &r_ss;

	return 0;
}