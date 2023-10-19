//	VMachine
//	Load the helper functions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "ElfStructures.h"

void VMachine::LoadHelperFunctions(WORD resourceID)
{
	//Open the resource
	Byte * helperData;
	Dword helperSize;

	if(!GetResourcePtrAndSize(resourceID, helperData, helperSize))
		throw Ex("Internal Error: Unable to access resource " +
			boost::lexical_cast<std::string>(resourceID));

	//Copy the data into a temporary vector
	std::vector <char> tempHelperFunctions;
	std::copy(	helperData, helperData + helperSize,
				std::back_inserter(tempHelperFunctions));

	//Extract the header
	if(tempHelperFunctions.size() < sizeof(ElfHeader))
	{
		throw Ex("Internal Error: File " +
			boost::lexical_cast<std::string>(resourceID) +
				" is not a valid ELF object file");
	}

	ElfHeader header;
	
	memcpy(&header, &tempHelperFunctions[0], sizeof(ElfHeader));

	//Ensure this is a valid 32-bit, little-endian, i386 ELF object file
	if(	header.id[0] != 0x7f || header.id[1] != 'E' ||
		header.id[2] != 'L' || header.id[3] != 'F' ||
		header.id[4] != 0x01 || header.id[5] != 0x01 ||
		header.type != 0x0001 || header.machine != 0x0003)
	{
		throw Ex("Internal Error: File " +
			boost::lexical_cast<std::string>(resourceID) +
				" is not a valid ELF object file");
	}

	//Allocate memory for the helper functions and copy the file data to it
	assert(!helperFunctions);
	helperFunctionsSize = tempHelperFunctions.size();
	helperFunctions = reinterpret_cast<Byte *>(
						VirtualAlloc(	0, helperFunctionsSize,
										MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE));

	memcpy(helperFunctions, &tempHelperFunctions[0], tempHelperFunctions.size());

	//Extract the section headers
	if(header.shEntrySize != sizeof(ElfSectionHeader))
	{
		throw Ex("Internal Error: File " +
			boost::lexical_cast<std::string>(resourceID) +
				" has an unsupported section header entry size");
	}

	std::vector <ElfSectionHeader> sectionHeaders(header.shNumEntries);
	
	memcpy(	&sectionHeaders[0], &helperFunctions[header.shOffset],
			header.shNumEntries * sizeof(ElfSectionHeader));

	//Extract the section name strings
	std::vector <Byte> sectionNameStrings(sectionHeaders[header.shStrTabIndex].size);

	memcpy(	&sectionNameStrings[0],
			&helperFunctions[sectionHeaders[header.shStrTabIndex].offset],
			sectionHeaders[header.shStrTabIndex].size);

	//Extract the string and symbol tables and get the offset of the start of the text section
	Dword stringTableSection = 0xffffffff, symbolTableSection = 0xffffffff;
	Dword textStart = 0;
	
	for(Dword section = 0; section < header.shNumEntries; ++section)
	{
		std::string name = reinterpret_cast<char *>(
							&sectionNameStrings[sectionHeaders[section].name]);

		if(name == ".strtab")
			stringTableSection = section;
		
		if(name == ".symtab")
			symbolTableSection = section;

		if(name == ".text")
			textStart = sectionHeaders[section].offset;
	}

	if(stringTableSection == 0xffffffff)
	{
		throw Ex("Internal Error: File " +
			boost::lexical_cast<std::string>(resourceID) +
				" has no \".strtab\" section");
	}

	if(symbolTableSection == 0xffffffff)
	{
		throw Ex("Internal Error: File " +
			boost::lexical_cast<std::string>(resourceID) +
				" has no \".symtab\" section");
	}

	std::vector <Byte> stringTable(sectionHeaders[stringTableSection].size);

	memcpy(	&stringTable[0],
			&helperFunctions[sectionHeaders[stringTableSection].offset],
			sectionHeaders[stringTableSection].size);

	Dword numSymbols = sectionHeaders[symbolTableSection].size / sizeof(ElfSymbolTableEntry);
	std::vector <ElfSymbolTableEntry> symbolTable(numSymbols);

	memcpy(	&symbolTable[0],
			&helperFunctions[sectionHeaders[symbolTableSection].offset],
			sectionHeaders[symbolTableSection].size);

	//Loop through the functions whose addresses are required
	const int numHelperFunctions = 21;

	std::string functionNames[numHelperFunctions] =
	{
		"@ReadByteSupervisorMode@4", "@ReadByteUserMode@4",
		"@ReadWordSupervisorMode@4", "@ReadWordUserMode@4",
		"@ReadDwordSupervisorMode@4", "@ReadDwordUserMode@4",
		"@WriteByteSupervisorMode@8", "@WriteByteSupervisorModeWriteProtect@8", "@WriteByteUserMode@8",
		"@WriteWordSupervisorMode@8", "@WriteWordSupervisorModeWriteProtect@8", "@WriteWordUserMode@8",
		"@WriteDwordSupervisorMode@8", "@WriteDwordSupervisorModeWriteProtect@8", "@WriteDwordUserMode@8",
		"_SetDataSegmentRegisterValueSupervisorMode", "_SetDataSegmentRegisterValueUserMode",
		"_SetDataSegmentRegisterValuePtrSupervisorMode", "_SetDataSegmentRegisterValuePtrUserMode",
		"_JumpToNextTranslation", "_JumpToNextTranslationWithPatch"
	};

	Dword * functionAddressPointers[numHelperFunctions] =
	{
		&readByteSupervisorModeAddress, &readByteUserModeAddress,
		&readWordSupervisorModeAddress, &readWordUserModeAddress,
		&readDwordSupervisorModeAddress, &readDwordUserModeAddress,
		&writeByteSupervisorModeAddress, &writeByteSupervisorModeWriteProtectAddress, &writeByteUserModeAddress,
		&writeWordSupervisorModeAddress, &writeWordSupervisorModeWriteProtectAddress, &writeWordUserModeAddress,
		&writeDwordSupervisorModeAddress, &writeDwordSupervisorModeWriteProtectAddress, &writeDwordUserModeAddress,
		&setDataSegmentRegisterValueSupervisorModeAddress, &setDataSegmentRegisterValueUserModeAddress,
		&setDataSegmentRegisterValuePtrSupervisorModeAddress, &setDataSegmentRegisterValuePtrUserModeAddress,
		&jumpToNextTranslationAddress, &jumpToNextTranslationWithPatchAddress
	};

    //Loop through symbols
	for(Dword symbol = 0; symbol < numSymbols; ++symbol)
	{
		//Get the name of this symbol
		std::string name = reinterpret_cast<char *>(&stringTable[symbolTable[symbol].name]);

		//If the name is one of the helper function names
		for(int i = 0; i < numHelperFunctions; ++i)
		{
			if(name == functionNames[i])
			{
				//Save the address of this function
				*functionAddressPointers[i] =	reinterpret_cast<Dword>(helperFunctions) +
												textStart + symbolTable[symbol].value;
			}
		}
	}

	//Ensure all helper functions were found
	for(int i = 0; i < numHelperFunctions; ++i)
		assert(*functionAddressPointers[i]);

	//Patch the addresses of the TLBs into the helper functions
	const int numTlbs = 5;

	std::string tlbPatchNames[numTlbs] =
	{
		"supervisorReadTlbPatch", "userReadTlbPatch",
		"supervisorWriteTlbPatch", "supervisorWriteWPTlbPatch", "userWriteTlbPatch"
	};

	Dword tlbAddresses[numTlbs] =
	{
		reinterpret_cast<Dword>(registers->supervisorReadTlb),
		reinterpret_cast<Dword>(registers->userReadTlb),
		reinterpret_cast<Dword>(registers->supervisorWriteTlb),
		reinterpret_cast<Dword>(registers->supervisorWriteWPTlb),
		reinterpret_cast<Dword>(registers->userWriteTlb),
	};

	//Loop through symbols
	for(Dword symbol = 0; symbol < numSymbols; ++symbol)
	{
		//Get the name of this symbol
		std::string name = reinterpret_cast<char *>(&stringTable[symbolTable[symbol].name]);

		//If the name is one of the TLB patch names, patch in the TLB address
		for(int i = 0; i < numTlbs; ++i)
		{
			if(name.compare(0, tlbPatchNames[i].length(), tlbPatchNames[i]) == 0)
			{
				*reinterpret_cast<Dword *>(
					helperFunctions + textStart + symbolTable[symbol].value + 2) +=
						tlbAddresses[i] - 0x80000000;
			}
		}
	}
}
