//	VMachine
//	Structures used within an ELF object file
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

struct ElfHeader
{
	Byte id[16];
	Word type;			//1 = Relocatable file
	Word machine;		//3 = 386
	Dword version;
	Dword entry;
	Dword phOffset;
	Dword shOffset;		//Section header table offset
	Dword flags;
	Word ehSize;		//Elf header size
	Word phEntrySize;
	Word phNumEntries;
	Word shEntrySize;	//Section header table entry size
	Word shNumEntries;	//Number of section header table entries
	Word shStrTabIndex;	//Index of the section name string table section
};

struct ElfSectionHeader
{
	Dword name;
	Dword type;
	Dword flags;
	Dword address;
	Dword offset;	//Offset of the section within the file
	Dword size;		//Section size
	Dword link;
	Dword info;
	Dword addrAlign;
	Dword entrySize;
};

struct ElfSymbolTableEntry
{
	Dword name;
	Dword value;
	Dword size;
	Byte info;
	Byte other;
	Word sectionIndex;
};

#pragma pack(push)
#pragma pack(1)
struct ElfRelocation
{
	Dword offset;
	Byte type;
	Word symbol;
	Byte padding[1];
};
#pragma pack(pop)
