//	VMachine
//	Add the translation for an instruction to a translation
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "PatchInfo.h"

namespace
{
	const Dword A32 = OPCODE_A32;
	const Dword O32 = OPCODE_O32;
	const Dword REP = OPCODE_REP;
	const Dword REPNE = OPCODE_REPNE;
}

bool VMachine::AddInstructionToTranslation(	bool addIfCrossesPageBoundary,
											Dword opcode, OpcodeProperties properties,
											PrefixBitset prefixes,
											Dword startEip, Dword & r_eip,
											std::vector <Byte> & translation,
											std::vector <Dword> & readByteCallOffsets, std::vector <Dword> & readWordCallOffsets, std::vector <Dword> & readDwordCallOffsets,
											std::vector <Dword> & writeByteCallOffsets, std::vector <Dword> & writeWordCallOffsets, std::vector <Dword> & writeDwordCallOffsets,
											std::vector <Dword> & setDataSegmentRegisterValueCallOffsets,
											std::vector <Dword> & setDataSegmentRegisterValuePtrCallOffsets,
											std::vector <PatchInfo> & patches)
{
	//If the opcode has a ModR/M byte, read and interpret it
	bool hasModRM = (properties & OPCODE_HAS_MOD_RM) != 0;
	Byte modRM = 0x00;
	SegmentRegister * defaultSegReg = &r_ds;
	Dword * baseReg = 0; Dword * indexReg = 0; Dword indexShift = 0; Dword displacement = 0;
	Byte * Rb = 0; Byte * Gb = 0;
	Word * Rw = 0; Word * Gw = 0;
	Dword * Rd = 0; Dword * Gd = 0;
	SegmentRegisterData * Sw = 0;

	if(hasModRM)
	{
		Dword modRMSize;
		
		if((opcode & A32) == 0)
		{
			if(!DecodeModRMByteA16(	r_eip, modRM, 
									defaultSegReg, baseReg, indexReg, displacement,
									Rb, Gb, Rw, Gw, Rd, Gd, Sw, modRMSize))
			{
				return false;
			}
		}
		else
		{
			if(!DecodeModRMByteA32(	r_eip, modRM, 
									defaultSegReg, baseReg, indexReg, indexShift, displacement,
									Rb, Gb, Rw, Gw, Rd, Gd, Sw, modRMSize))
			{
				return false;
			}
		}

		r_eip += modRMSize;
	}

	//Calculate the segment register to use
	SegmentRegister * segReg = GetOverrideSegReg(prefixes);
	if(!segReg)
		segReg = defaultSegReg;

	//If the opcode has an immediate dword, read it
	bool hasImmDword =	(opcode & O32) != 0 && (properties & OPCODE_HAS_IMMEDIATE_O_SIZE) != 0 ||
						(opcode & A32) != 0 && (properties & OPCODE_HAS_IMMEDIATE_A_SIZE) != 0 ||
						(hasModRM && (opcode & O32) != 0 && (properties & OPCODE_HAS_IMMEDIATE_O_SIZE_IF_REG_0) != 0 && (modRM & 0x38) == 0);
	Dword immDword = 0x00000000;

    if(hasImmDword)
	{
		if(!ReadCodeDword(r_eip, immDword))
			return false;
		
		r_eip += 4;
	}

	//If the opcode has an immediate word, read it
	bool hasImmWord =	(properties & OPCODE_HAS_IMMEDIATE_WORD) != 0 ||
						(opcode & O32) == 0 && (properties & OPCODE_HAS_IMMEDIATE_O_SIZE) != 0 ||
						(opcode & A32) == 0 && (properties & OPCODE_HAS_IMMEDIATE_A_SIZE) != 0 ||
						(hasModRM && (opcode & O32) == 0 && (properties & OPCODE_HAS_IMMEDIATE_O_SIZE_IF_REG_0) != 0 && (modRM & 0x38) == 0);
	Word immWord = 0x0000;

    if(hasImmWord)
	{
		if(!ReadCodeWord(r_eip, immWord))
			return false;
		
		r_eip += 2;
	}

	//If the opcode has an immediate byte, read it
	bool hasImmByte =	(properties & OPCODE_HAS_IMMEDIATE_BYTE) != 0 ||
						(hasModRM && (properties & OPCODE_HAS_IMMEDIATE_BYTE_IF_REG_0) != 0 && (modRM & 0x38) == 0);
	Byte immByte = 0x00;

    if(hasImmByte)
	{
		if(!ReadCodeByte(r_eip++, immByte))
			return false;
	}

	//If addIfCrossesPageBoundary is false, and this instruction crosses a page boundary,
	//do not add it
	Dword startPage = (registers->r_cs.base + startEip) >> 12;
	Dword endPage = (registers->r_cs.base + r_eip - 1) >> 12;
	
	if(!addIfCrossesPageBoundary && startPage != endPage)
		return false;

	//If the opcode is a valid register FPU instruction, add it to the translation and return
	if((properties & OPCODE_COPROC) && (modRM & 0xc0) == 0xc0)
	{
		if(coprocRegisterInstructionValid[((opcode & 0x0700) >> 2) | (opcode & 0x003f)])
		{
			translation.push_back(static_cast<Byte>((opcode & 0xff00) >> 8));
			translation.push_back(static_cast<Byte>(opcode & 0xff));

			//If the instruction is "FSTSW AX", append an instruction to move the contents
			//of the host's ax register into the VMachine's ax register
			//mov [ebp + offsetof(Registers, r_ax)], ax
			if((opcode & 0xffff) == 0xdfe0)
			{
				translation.push_back(0x66);
				translation.push_back(0x89);
				translation.push_back(0x45);
				translation.push_back(offsetof(Registers, r_ax));
			}

			return true;
		}
		else
		{
#ifdef INVALID_OPCODES_FATAL
			std::stringstream ss;
			ss << "CPU Error: Invalid Instruction, opcode = 0x" << std::hex << opcode;
			throw Ex(ss.str());
#else
			readCodeException = EXCEPTION_INVALID_OPCODE;
			return false;
#endif
		}
	}

	//Add any repeat prefixes to the opcode to look up
	if((properties & OPCODE_STRING) != 0)
	{
		if((prefixes & PREFIX_REP) != 0)
			opcode |= REP;

		if((prefixes & PREFIX_REPNE) != 0)
			opcode |= REPNE;
	}

	//Get the translation for this opcode
	std::map<Dword, Translation>::const_iterator translationIt = translations.find(opcode);

	if(translationIt == translations.end())
	{
#ifdef INVALID_OPCODES_FATAL
			std::stringstream ss;
			ss << "CPU Error: Unimplemented Instruction, opcode = 0x" << std::hex << opcode;
			throw Ex(ss.str());
#else
			readCodeException = EXCEPTION_INVALID_OPCODE;
			return false;
#endif
	}

	//Append the instruction to the translation
	Dword translationSize = translation.size();

	std::copy(	translationIt->second.translation.begin(),
				translationIt->second.translation.end(),
				std::back_inserter(translation));

	//Add the offsets for any relocations to the vectors
	//Subtract 1 from the offset to get the offset of the call instruction
	const int numHelperFunctions = 8;

	const std::vector <Dword> * const helperFunctionOffsetVectors[numHelperFunctions] =
	{
		&translationIt->second.ReadByteOffsets, &translationIt->second.ReadWordOffsets,
		&translationIt->second.ReadDwordOffsets, &translationIt->second.WriteByteOffsets,
		&translationIt->second.WriteWordOffsets, &translationIt->second.WriteDwordOffsets,
		&translationIt->second.SetDataSegmentRegisterValueOffsets,
		&translationIt->second.SetDataSegmentRegisterValuePtrOffsets
	};

	std::vector <Dword> * const helperFunctionCallOffsetVectors[numHelperFunctions] =
	{
		&readByteCallOffsets, &readWordCallOffsets, &readDwordCallOffsets,
		&writeByteCallOffsets, &writeWordCallOffsets, &writeDwordCallOffsets,
		&setDataSegmentRegisterValueCallOffsets,
		&setDataSegmentRegisterValuePtrCallOffsets
	};

	for(int i = 0; i < numHelperFunctions; ++i)
	{
		for(std::vector<Dword>::const_iterator it = helperFunctionOffsetVectors[i]->begin();
			it != helperFunctionOffsetVectors[i]->end(); ++it)
		{
			helperFunctionCallOffsetVectors[i]->push_back(translationSize + *it - 1);
		}
	}

	//Add any accesses to external data to the patches
	if(!translationIt->second.RbOffsets.empty()) assert(hasModRM && Rb != 0);
	if(!translationIt->second.GbOffsets.empty()) assert(hasModRM && Gb != 0);
	if(!translationIt->second.RwOffsets.empty()) assert(hasModRM && Rw != 0);
	if(!translationIt->second.GwOffsets.empty()) assert(hasModRM && Gw != 0);
	if(!translationIt->second.RdOffsets.empty()) assert(hasModRM && Rd != 0);
	if(!translationIt->second.GdOffsets.empty()) assert(hasModRM && Gd != 0);
	if(!translationIt->second.SwOffsets.empty()) assert(hasModRM && Sw != 0);
	assert(hasImmByte != translationIt->second.immByteOffsets.empty());
	assert(hasImmWord != translationIt->second.immWordOffsets.empty());
	assert(hasImmDword != translationIt->second.immDwordOffsets.empty());

	const int numDataItems = 16;

	const PatchInfoType patchTypes[numDataItems] =
	{
		PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA, PATCH_TYPE_DATA,
		PATCH_TYPE_DATA_POINTER, PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA_POINTER, PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA_POINTER, PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA_POINTER,
		PATCH_TYPE_DATA, PATCH_TYPE_DATA, PATCH_TYPE_DATA,
		PATCH_TYPE_DATA
	};

	const Dword patchData[numDataItems] =
	{
		reinterpret_cast<Dword>(&segReg->data.base),
		(baseReg != 0) ? reinterpret_cast<Dword>(baseReg) : reinterpret_cast<Dword>(&registers->zero),
		(indexReg != 0) ? reinterpret_cast<Dword>(indexReg) : reinterpret_cast<Dword>(&registers->zero),
		indexShift, displacement,
		reinterpret_cast<Dword>(Rb), reinterpret_cast<Dword>(Gb),
		reinterpret_cast<Dword>(Rw), reinterpret_cast<Dword>(Gw),
		reinterpret_cast<Dword>(Rd), reinterpret_cast<Dword>(Gd),
		reinterpret_cast<Dword>(Sw),
		immByte, immWord, immDword,
		r_eip - startEip
	};

	const std::vector <Dword> * const patchOffsetVectors[numDataItems] =
	{
		&translationIt->second.segRegBaseOffsets,
		&translationIt->second.baseRegOffsets, &translationIt->second.indexRegOffsets,
		&translationIt->second.indexShiftOffsets, &translationIt->second.displacementOffsets,
		&translationIt->second.RbOffsets, &translationIt->second.GbOffsets,
		&translationIt->second.RwOffsets, &translationIt->second.GwOffsets,
		&translationIt->second.RdOffsets, &translationIt->second.GdOffsets,
		&translationIt->second.SwOffsets,
		&translationIt->second.immByteOffsets, &translationIt->second.immWordOffsets,
		&translationIt->second.immDwordOffsets,
		&translationIt->second.instrLengthOffsets
	};

	for(int i = 0; i < numDataItems; ++i)
	{
		if(patchOffsetVectors[i]->empty())
			continue;

		PatchInfo patchInfo;
		patchInfo.type = patchTypes[i];
		patchInfo.data = patchData[i];

		for(std::vector<Dword>::const_iterator it = patchOffsetVectors[i]->begin();
			it != patchOffsetVectors[i]->end(); ++it)
		{
			patchInfo.offsets.push_back(translationSize + *it);
		}

		patches.push_back(patchInfo);
	}

	return true;
}
