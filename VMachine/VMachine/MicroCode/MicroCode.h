//	VMachine
//	Class for a sequence of microcode
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

#include "MicroOp.h"

class MicroCode
{
public:

	//Clear the MicroCode
	void clear(void)
	{
		microCode.resize(0);
		microOpContainsImmDword.resize(0);
	}

	//Append a MicroOp
	void push_back(bool containsImmDword, const MicroOp & microOp)
	{
		microCode.push_back(microOp);
		microOpContainsImmDword.push_back(containsImmDword);
	}

	//Remove a MicroOp
	void pop_back(void)
	{
		microCode.pop_back();
		microOpContainsImmDword.pop_back();
	}

	//Append another sequence of MicroCode
	void push_back(const MicroCode & additionalMicroCode)
	{
		assert(microCode.size() == microOpContainsImmDword.size());
		assert(	additionalMicroCode.microCode.size() ==
				additionalMicroCode.microOpContainsImmDword.size());

		microCode.insert(	microCode.end(), additionalMicroCode.microCode.begin(),
							additionalMicroCode.microCode.end());
		
		microOpContainsImmDword.insert(	microOpContainsImmDword.end(),
										additionalMicroCode.microOpContainsImmDword.begin(),
										additionalMicroCode.microOpContainsImmDword.end());
	}

	//Get the number of MicroOps
	std::size_t size(void)
	{
		return microCode.size();
	}

	//Get the last MicroOp
	const MicroOp & back(void)
	{
		return microCode.back();
	}

	//Convert to host code
	void ConvertToHostCode(	std::vector <Byte> & hostCode,
							std::vector<Dword> & hostImmDwordOffsets,
							std::vector <Dword> & readByteCallOffsets,
							std::vector <Dword> & readWordCallOffsets,
							std::vector <Dword> & readDwordCallOffsets,
							std::vector <Dword> & writeByteCallOffsets,
							std::vector <Dword> & writeWordCallOffsets,
							std::vector <Dword> & writeDwordCallOffsets,
							std::vector <Dword> & setDataSegmentRegisterValueCallOffsets);

private:

	//The microcode itself
	std::vector <MicroOp> microCode;

	//Which MicroOps contain immediate dwords from the guest code?
	std::vector <bool> microOpContainsImmDword;

	//Remove any redundant flag calculations
	void RemoveRedundantFlags(void);

public:

	//Exception thrown by MicroCode functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};
