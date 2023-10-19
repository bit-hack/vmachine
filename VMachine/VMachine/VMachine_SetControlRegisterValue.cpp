//	VMachine
//	Set the value of a control register
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::SetCR0Value(Dword newCr0)
{
	//If paging is being toggled, flush the TLBs
	if((newCr0 & CR0_PG_FLAG) != (registers->r_cr0 & CR0_PG_FLAG))
		FlushTLBs();

	//Set the new CR0 value
	registers->r_cr0 = newCr0 & CR0_MASK;
}

void VMachine::SetCR3Value(Dword newCr3)
{
	//Set the new CR3 value
	registers->r_cr3 = newCr3 & CR3_MASK;

	//If paging is enabled, flush the TLBs
	if((registers->r_cr0 & CR0_PG_FLAG) != 0)
		FlushTLBs();
}
