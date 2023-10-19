//	VMachine
//	TLB functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

//Flush the TLBs
void VMachine::FlushTLBs(void)
{
	for(Dword i = 0; i < TLB_SIZE; ++i)
	{
		registers->supervisorReadTlb[i].linearPageAndFlags		= TLB_ENTRY_INVALID_FLAG;
		registers->userReadTlb[i].linearPageAndFlags			= TLB_ENTRY_INVALID_FLAG;
		registers->supervisorWriteTlb[i].linearPageAndFlags		= TLB_ENTRY_INVALID_FLAG;
		registers->supervisorWriteWPTlb[i].linearPageAndFlags	= TLB_ENTRY_INVALID_FLAG;
		registers->userWriteTlb[i].linearPageAndFlags			= TLB_ENTRY_INVALID_FLAG;
	}
}

//Set/clear a flag for a given physical page in the write TLBs
void VMachine::SetWriteTLBsFlag(Dword physicalPage, Dword flag)
{
	TlbEntry * const tlbs[3] =
	{
		registers->supervisorWriteTlb,
		registers->supervisorWriteWPTlb,
		registers->userWriteTlb
	};

	for(Dword i = 0; i < 3; ++i)
	{
		for(Dword j = 0; j < TLB_SIZE; ++j)
		{
			TlbEntry * tlbEntry = &tlbs[i][j];

			if(	(tlbEntry->linearPageAndFlags & TLB_ENTRY_LINEAR_PAGE_MASK) +
					tlbEntry->linToHostOffset - registers->memoryPtr ==
						(physicalPage << 12))
			{
				tlbEntry->linearPageAndFlags |= flag;
			}
		}
	}
}

void VMachine::ClearWriteTLBsFlag(Dword physicalPage, Dword flag)
{
	TlbEntry * const tlbs[3] =
	{
		registers->supervisorWriteTlb,
		registers->supervisorWriteWPTlb,
		registers->userWriteTlb
	};

	for(Dword i = 0; i < 3; ++i)
	{
		for(Dword j = 0; j < TLB_SIZE; ++j)
		{
			TlbEntry * tlbEntry = &tlbs[i][j];

			if(	(tlbEntry->linearPageAndFlags & TLB_ENTRY_LINEAR_PAGE_MASK) +
					tlbEntry->linToHostOffset - registers->memoryPtr ==
						(physicalPage << 12))
			{
				tlbEntry->linearPageAndFlags &= ~flag;
			}
		}
	}
}
