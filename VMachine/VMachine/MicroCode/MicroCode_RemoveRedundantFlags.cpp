//	VMachine
//	Remove any redundant flag calculations
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "MicroCode.h"

void MicroCode::RemoveRedundantFlags(void)
{
	FlagSet requiredFlags = FLAGSET_STATUS_FLAGS;

	//Loop through MicroOps in reverse
	for(std::vector<MicroOp>::reverse_iterator op = microCode.rbegin();
		op != microCode.rend(); ++op)
	{
		//If the MicroOp does not write any of the required flags
		if((op->flagsWritten & requiredFlags) == 0)
		{
			//Then it may as well not write any flags at all
			op->flagsWritten = 0;
		}

		//Remove any flags written from requiredFlags
		requiredFlags &= ~op->flagsWritten;

		//Add any flags read to requiredFlags
		requiredFlags |= op->flagsRead;
	}
}
