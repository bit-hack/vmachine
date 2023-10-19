//	VMachine
//	Small utility functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

inline SignedByte GetHighestSetBit(Dword data)
{
	__asm
	{
		mov ecx, -1		//If data is zero, return -1
		bsr eax, data
		cmovz eax, ecx
	}
}

inline SignedByte GetLowestSetBit(Dword data)
{
	__asm
	{
		mov ecx, -1		//If data is zero, return -1
		bsf eax, data
		cmovz eax, ecx
	}
}

inline bool IsPowerOf2OrZero(Dword data)
{
	return (data & (data - 1)) == 0;
}

inline Dword RoundUpToPowerOf2(Dword data)
{
	Dword result = data - 1;

	result |= result >> 1;
	result |= result >> 2;
	result |= result >> 4;
	result |= result >> 8;
	result |= result >> 16;

	return result + 1;
}

inline bool GetResourcePtrAndSize(WORD resourceID, Byte *& ptr, Dword & size)
{
	HRSRC hResource = FindResource(0, MAKEINTRESOURCE(resourceID), "BINARY");
	if(!hResource)
		return false;

	HGLOBAL hData = LoadResource(0, hResource);
	if(!hData)
		return false;

	ptr = reinterpret_cast<Byte *>(LockResource(hData));
	if(!ptr)
		return false;

	size = SizeofResource(0, hResource);
	if(size == 0)
		return false;

	return true;
}
