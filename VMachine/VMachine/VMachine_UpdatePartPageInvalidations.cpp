//	VMachine
//	Update the partPageInvalidations array
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::UpdatePartPageInvalidations(void)
{
	//Loop through physical pages
	for(Dword i = 0; i < (registers->memorySize >> 12); ++i)
	{
		//If this page has been invalidated 10 times in the last second
		if(pageInvalidationCounts[i] >= 10)
		{
			//Mark it as part page invalidation pending
			partPageInvalidationPendings[i] = true;
		}

		pageInvalidationCounts[i] = 0;
	}
}
