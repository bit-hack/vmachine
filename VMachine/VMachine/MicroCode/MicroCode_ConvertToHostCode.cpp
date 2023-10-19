//	VMachine
//	Convert to host code
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "MicroCode.h"

void MicroCode::ConvertToHostCode(	std::vector <Byte> & hostCode,
									std::vector<Dword> & hostImmDwordOffsets,
									std::vector <Dword> & readByteCallOffsets,
									std::vector <Dword> & readWordCallOffsets,
									std::vector <Dword> & readDwordCallOffsets,
									std::vector <Dword> & writeByteCallOffsets,
									std::vector <Dword> & writeWordCallOffsets,
									std::vector <Dword> & writeDwordCallOffsets,
									std::vector <Dword> & setDataSegmentRegisterValueCallOffsets)
{
	//Remove any redundant flag calculations
#ifdef REMOVE_REDUNDANT_FLAGS
	RemoveRedundantFlags();
#endif

	//Convert each MicroOp to host code
	for(std::vector<MicroOp>::const_iterator op = microCode.begin(); op != microCode.end(); ++op)
	{
		Dword immDwordOffset = 0;

		op->ConvertToHostCode(	hostCode,
								immDwordOffset,
								readByteCallOffsets, readWordCallOffsets,
								readDwordCallOffsets, writeByteCallOffsets,
								writeWordCallOffsets, writeDwordCallOffsets,
								setDataSegmentRegisterValueCallOffsets);

		if(microOpContainsImmDword[op - microCode.begin()])
		{
			assert(immDwordOffset != 0);
			hostImmDwordOffsets.push_back(immDwordOffset);
		}
	}
}
