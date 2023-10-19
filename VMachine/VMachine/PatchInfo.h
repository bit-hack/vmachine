//	VMachine
//	Information about a patch to be made to a translation
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

enum PatchInfoType
{
	PATCH_TYPE_DATA,
	PATCH_TYPE_DATA_POINTER
};

struct PatchInfo
{
	PatchInfoType type;
	std::vector <Dword> offsets;
	Dword data;
};
