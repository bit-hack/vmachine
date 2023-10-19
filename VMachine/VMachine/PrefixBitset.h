//	VMachine
//	Bitset to hold the prefixes on a given instruction
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

typedef Dword PrefixBitset;

const PrefixBitset PREFIX_CS		= 0x0001;
const PrefixBitset PREFIX_DS		= 0x0002;
const PrefixBitset PREFIX_ES		= 0x0004;
const PrefixBitset PREFIX_FS		= 0x0008;
const PrefixBitset PREFIX_GS		= 0x0010;
const PrefixBitset PREFIX_SS		= 0x0020;
const PrefixBitset PREFIX_REP		= 0x0040;
const PrefixBitset PREFIX_REPNE		= 0x0080;
const PrefixBitset PREFIX_A_SIZE	= 0x0100;
const PrefixBitset PREFIX_O_SIZE	= 0x0200;
