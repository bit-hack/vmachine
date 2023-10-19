//	VMachine
//	Execute instructions
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::ExecuteInstructions(void)
{
	SwitchToVMachineFpuContext();

	//Loop until the requested number of instructions have been executed
	while(registers->numInstr > 0)
	{
		Dword opcode;
		OpcodeProperties properties;
		PrefixBitset prefixes;
		Dword numBytes;

		//Free any translations which need it
		for(std::vector<Byte *>::const_iterator i = translationsNeedingFreeing.begin();
			i != translationsNeedingFreeing.end(); ++i)
		{
			TranslationCacheFree(*i);
		}
		translationsNeedingFreeing.resize(0);

		//If the I flag is set and the interrupt line is raised, handle the interrupt
		if((registers->r_systemFlags & EFLAGS_I_FLAG) != 0 && intRaised)
			HandleInterrupt();

		//Print the values of the VMachine registers, if requested
#ifdef DEBUG_OUTPUT_REGISTERS
		static bool debugOutputRegisters = false;

		if(GetAsyncKeyState(VK_PRIOR))
			debugOutputRegisters = true;

		if(debugOutputRegisters)
		{
			dbgOut << registers->r_cs.selector << ":" << registers->r_eip << " - ";
			dbgOut << registers->r_eax << " " << registers->r_ebx << " ";
			dbgOut << registers->r_ecx << " " << registers->r_edx << " ";
			dbgOut << registers->r_esp << " " << registers->r_ebp << " ";
			dbgOut << registers->r_esi << " " << registers->r_edi << " - ";
			dbgOut << registers->r_eflags << " " << registers->r_systemFlags << " - ";
			dbgOut << registers->r_cr0 << " - ";
			dbgOut << registers->r_cs.selector << " " << registers->r_cs.base << " - ";
			dbgOut << registers->r_ds.selector << " " << registers->r_ds.base << " - ";
			dbgOut << registers->r_es.selector << " " << registers->r_es.base << " - ";
			dbgOut << registers->r_fs.selector << " " << registers->r_fs.base << " - ";
			dbgOut << registers->r_gs.selector << " " << registers->r_gs.base << " - ";
			dbgOut << registers->r_ss.selector << " " << registers->r_ss.base << " - ";
			dbgOut << std::endl;
		}
#endif
		//Read the opcode at cs:eip
		if(!ReadOpcode(registers->r_eip, opcode, properties, prefixes, numBytes))
		{
			//An exception was caused. Handle it
			assert(readCodeException != EXCEPTION_NONE);

			registers->exception = readCodeException;
			registers->errorCode = readCodeErrorCode;
			registers->r_cr2 = readCodeCr2;
			
			readCodeException = EXCEPTION_NONE;

			instrStartEip = registers->r_eip;
			instrStartEsp = registers->r_esp;

			HandleException();

			continue;
		}

		assert(readCodeException == EXCEPTION_NONE);

		//If the opcode requires interpreting, do so
		if(	(properties & OPCODE_INTERPRET) != 0 ||
			(((properties & OPCODE_INTERPRET_IF_REG_2345) != 0) &&
				((opcode & 0x38) >> 3) > 1 &&
					((opcode & 0x38) >> 3) < 6) ||
			(((properties & OPCODE_INTERPRET_IF_REG_12) != 0) &&
				((((opcode & 0x38) >> 3)  == 1) ||
					(((opcode & 0x38) >> 3)  == 2))))
		{
			instrStartEip = registers->r_eip;
			instrStartEsp = registers->r_esp;

			//Move eip past the opcode
			registers->r_eip += numBytes;
	
			InterpretInstruction(opcode, prefixes);
	
			--registers->numInstr;

			//Handle any exception generated
			if(registers->exception != EXCEPTION_NONE)
				HandleException();

			continue;
		}

		//Otherwise, is there a translation for the instructions at cs:eip?
		//If so, get a pointer to it
		//The call to CalculatePhysicalAddress cannot fail, as we've already read the opcode
		EipToTranslationCacheMapKey key;
		bool physicalEipOK = CalculatePhysicalAddress(registers->r_cs.base + registers->r_eip, key.physicalEip);
		assert(physicalEipOK);
		key.flags = 0;
		
		if((registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			key.flags |= TRANSLATION_DEFAULT_A_O_SIZE_32_FLAG;

		if((registers->r_ss.descriptorFlags & DESCRIPTOR_FLAGS_DEFAULT_BIG_BIT) != 0)
			key.flags |= TRANSLATION_STACK_SIZE_32_FLAG;

		if(	registers->r_cs.base == 0 && registers->r_ds.base == 0 &&
			registers->r_es.base == 0 && registers->r_ss.base == 0)
		{
			key.flags |= TRANSLATION_CS_DS_ES_SS_BASES_ZERO_FLAG;
		}

		if(	(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
				DESCRIPTOR_FLAGS_DPL_MASK)
		{
			key.flags |= TRANSLATION_USER_MODE_FLAG;
		}
		else if((registers->r_cr0 & CR0_WP_FLAG) != 0)
		{
			key.flags |= TRANSLATION_SUPERVISOR_MODE_WRITE_PROTECT_FLAG;
		}

		Byte * translation = 0;

		EipToTranslationCacheTLBEntry & tlbEntry =
			eipToTranslationCacheTLB[key.Hash() & eipToTranslationCacheTLBMask];

		if(tlbEntry.key == key)
			translation = tlbEntry.value->translation;

		if(translation)
		{
			EipToTranslationTLBEntry & translationTLBEntry =
					registers->eipToTranslationTLB
						[key.physicalEip & EIP_TO_TRANSLATION_TLB_MASK];

			translationTLBEntry.physicalEip = key.physicalEip;
			translationTLBEntry.flags = key.flags;
			translationTLBEntry.translation = translation;
		}

		if(!translation)
		{
			EipToTranslationCacheMapType::const_iterator translationIt =
				eipToTranslationCacheMap.find(key);

			if(translationIt != eipToTranslationCacheMap.end())
			{
				translation = translationIt->second->translation;

				tlbEntry.key = key;
				tlbEntry.value = translationIt->second;

				EipToTranslationTLBEntry & translationTLBEntry =
					registers->eipToTranslationTLB
						[key.physicalEip & EIP_TO_TRANSLATION_TLB_MASK];
				
				translationTLBEntry.physicalEip = key.physicalEip;
				translationTLBEntry.flags = key.flags;
				translationTLBEntry.translation = translation;
			}
		}

		//If not, create a translation
		if(!translation)
		{
			Dword r_eip = registers->r_eip;
			std::vector<Dword> guestImmDwordOffsets;
			std::vector<Dword> hostImmDwordOffsets;

			bool translationCacheable = TranslateInstructions(	r_eip, key.physicalEip,
																translation,
																guestImmDwordOffsets,
																hostImmDwordOffsets);

			//Handle any exception caused
			if(readCodeException != EXCEPTION_NONE)
			{
				registers->exception = readCodeException;
				registers->errorCode = readCodeErrorCode;
				registers->r_cr2 = readCodeCr2;
			
				readCodeException = EXCEPTION_NONE;

				instrStartEip = registers->r_eip;
				instrStartEsp = registers->r_esp;

				HandleException();

				continue;
			}

			assert(guestImmDwordOffsets.size() == hostImmDwordOffsets.size());

			//If the translation is cacheable			
			if(translationCacheable)
			{
				//Add it to the eipToTranslationCacheMap, TLBs and the page translation list
				std::shared_ptr <EipToTranslationCacheMapValue> value(
					new EipToTranslationCacheMapValue);
				value->translation = translation;
				bool endPhysicalEipOK = CalculatePhysicalAddress(registers->r_cs.base + r_eip - 1, value->endPhysicalEip);
				assert(endPhysicalEipOK);

				for(std::vector<Dword>::size_type i = 0; i < guestImmDwordOffsets.size(); ++i)
					value->immDwordOffsets.insert(
						std::make_pair(guestImmDwordOffsets[i], hostImmDwordOffsets[i]));

				eipToTranslationCacheMap.insert(std::make_pair(key, value));

				tlbEntry.key = key;
				tlbEntry.value = value;

				EipToTranslationTLBEntry & translationTLBEntry =
					registers->eipToTranslationTLB
						[key.physicalEip & EIP_TO_TRANSLATION_TLB_MASK];
				
				translationTLBEntry.physicalEip = key.physicalEip;
				translationTLBEntry.flags = key.flags;
				translationTLBEntry.translation = translation;

				pageTranslationLists[key.physicalEip >> 12].push_back(
					std::make_pair(key, value));

				//Mark the page as containing translations
				registers->pageClassifications[key.physicalEip >> 12] =
					PAGE_CLASS_CONTAINS_TRANSLATIONS;

				SetWriteTLBsFlag(key.physicalEip >> 12, TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG);
			}
			else
			{
				//If not, it will need freeing
				translationsNeedingFreeing.push_back(translation);
			}
		}

		//Execute the translation
		registers->currentTranslationPhysicalPage = key.physicalEip >> 12;
		registers->currentTranslationFlags = key.flags;
		ExecuteTranslation(translation);

		//Handle any exception generated
		if(registers->exception != EXCEPTION_NONE)
		{
			instrStartEip = registers->r_eip;
			instrStartEsp = registers->r_esp;
			HandleException();
		}
	}

	SwitchToHostFpuContext();
}
