//	VMachine
//	Translate the instructions at a given eip
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "MicroCode/MicroCode.h"
#include "EmitFunctions/Emit.h"
#include "PatchInfo.h"

bool VMachine::TranslateInstructions(	Dword & r_eip, Dword physicalEip,
										Byte *& translation,
										std::vector<Dword> & guestImmDwordOffsets,
										std::vector<Dword> & hostImmDwordOffsets)
{
	Dword translationStartEip = r_eip;
	Dword translationStartPage = (registers->r_cs.base + translationStartEip) >> 12;
	Dword translationStartPhysicalPage = physicalEip >> 12;
	bool translationCacheable = true;
	bool translationMayChangeCsDsEsSsBasesZeroFlag = false;
	static MicroCode microCode;
	static std::vector <Byte> tempTranslation;
	std::vector <Dword> readByteCallOffsets, readWordCallOffsets, readDwordCallOffsets;
	std::vector <Dword> writeByteCallOffsets, writeWordCallOffsets, writeDwordCallOffsets;
	std::vector <Dword> setDataSegmentRegisterValueCallOffsets;
	std::vector <Dword> setDataSegmentRegisterValuePtrCallOffsets;
	std::vector <Dword> jumpToNextTranslationJumpOffsets;
	std::vector <Dword> jumpToNextTranslationWithPatchJumpOffsets;
	std::vector <PatchInfo> patches;

	microCode.clear();
	tempTranslation.resize(0);

	//Loop through up to 1000 instructions
	Dword numInstr;
	Dword opcode = 0;
	OpcodeProperties properties = 0;
	for(numInstr = 0; numInstr < 1000; ++numInstr)
	{
		PrefixBitset prefixes;
		Dword numBytes;
		Dword startEip = r_eip;
		Dword startPage = (registers->r_cs.base + startEip) >> 12;
		bool controlTransfer = false;

		//If we have moved onto a different page, we're done
		if(startPage != translationStartPage)
			break;

		//Read the opcode
		if(!ReadOpcode(r_eip, opcode, properties, prefixes, numBytes))
		{
			//If this is not the first instruction in the translation,
			//ignore any exception caused
			if(numInstr != 0)
				readCodeException = EXCEPTION_NONE;

			r_eip = startEip;
			break;
		}

		//If this opcode requires interpreting, we're done
		if(	(properties & OPCODE_INTERPRET) != 0 ||
			(((properties & OPCODE_INTERPRET_IF_REG_2345) != 0) &&
				((opcode & 0x38) >> 3) > 1 &&
					((opcode & 0x38) >> 3) < 6) ||
			(((properties & OPCODE_INTERPRET_IF_REG_12) != 0) &&
				((((opcode & 0x38) >> 3)  == 1) ||
					(((opcode & 0x38) >> 3)  == 2))))
		{
			break;
		}

		//Move r_eip past the opcode
		r_eip += numBytes;

		//If this instruction has a function implementation
#ifdef USE_MICROCODE
		if(	(properties & OPCODE_FUNCTION_IMPLEMENTATION) != 0 ||
			((properties & OPCODE_FUNCTION_IMPLEMENTATION_IF_MOD_MEM_AND_REG_46) != 0 &&
				((opcode & 0xc0) != 0xc0) &&
					((((opcode & 0x38) >> 3) == 4) || (((opcode & 0x38) >> 3) == 6))) ||
			((prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 &&
				(properties & OPCODE_STRING) != 0))
#endif
		{
			//Convert the current microcode to host code
			microCode.ConvertToHostCode(tempTranslation, hostImmDwordOffsets,
										readByteCallOffsets, readWordCallOffsets,
										readDwordCallOffsets, writeByteCallOffsets,
										writeWordCallOffsets, writeDwordCallOffsets,
										setDataSegmentRegisterValueCallOffsets);

			//Clear the microcode
			microCode.clear();

			//Add the translation for this instruction
			//Only add an instruction which crosses a page boundary if it is the first
			//instruction in the translation
			if(!AddInstructionToTranslation(numInstr == 0, opcode, properties, prefixes,
											startEip, r_eip, tempTranslation,
											readByteCallOffsets, readWordCallOffsets,
											readDwordCallOffsets, writeByteCallOffsets,
											writeWordCallOffsets, writeDwordCallOffsets,
											setDataSegmentRegisterValueCallOffsets,
											setDataSegmentRegisterValuePtrCallOffsets,
											patches))
			{
				//If this is not the first instruction in the translation,
				//ignore any exception caused
				if(numInstr != 0)
					readCodeException = EXCEPTION_NONE;

				r_eip = startEip;
				break;
			}

			//Otherwise, if the instruction crossed a page boundary (as the first instruction
			//in the translation), the translation is not cacheable
			Dword page = (registers->r_cs.base + r_eip - 1) >> 12;
			if(page != startPage)
				translationCacheable = false;

			//If this is a control transfer instruction, we're done with this translation
			if((properties & (	OPCODE_UNCOND_NEAR_CONTROL_TRANSFER |
								OPCODE_COND_NEAR_CONTROL_TRANSFER |
								OPCODE_MISC_NEAR_CONTROL_TRANSFER)) != 0)
			{
				++numInstr;
				break;
			}

			//If this was a repeated string instruction, we're done with this translation
			if(	(prefixes & (PREFIX_REP | PREFIX_REPNE)) != 0 &&
				(properties & OPCODE_STRING) != 0)
			{
				++numInstr;
				break;
			}

			//Otherwise, begin a new set of microcode with a microOp to advance eip
			microCode.push_back(false, MicroOp(	TYPE_ADDEIP, FUNC_NONE,
												OPERAND_TYPE_IMMEDIATE_32, r_eip - startEip));
		}
#ifdef USE_MICROCODE
		else
		{
			//Otherwise, convert this instruction to microcode
			MicroCode instrMicroCode;
			Dword guestImmDwordOffset = 0;
			bool instructionMayCauseException = true;
			if(!ConvertGuestInstructionToMicroCode(	opcode, prefixes, startEip, r_eip,
													translationMayChangeCsDsEsSsBasesZeroFlag,
													instrMicroCode, guestImmDwordOffset,
													instructionMayCauseException))
			{
				assert(readCodeException != EXCEPTION_NONE);

				//An exception was caused. Move r_eip back to the start of this instruction
				r_eip = startEip;

				//If this is not the first instruction in the translation, ignore the exception
				if(numInstr != 0)
					readCodeException = EXCEPTION_NONE;

				//We're done with this translation
				break;
			}

			assert(readCodeException == EXCEPTION_NONE);

			//If the instruction crossed a page boundary
			Dword page = (registers->r_cs.base + r_eip - 1) >> 12;
			if(page != startPage)
			{
				//If the instruction is the first in the translation,
				//the translation is not cacheable
				if(numInstr == 0)
					translationCacheable = false;
				else
				{
					//Otherwise, move r_eip back to the start of this instruction,
					//and we're done with this translation
					r_eip = startEip;

					break;
				}
			}

			if(guestImmDwordOffset != 0)
				guestImmDwordOffsets.push_back(
					(startEip - translationStartEip) + guestImmDwordOffset);

			//If this is a control transfer instruction, we're done with this translation
			if((properties & (	OPCODE_UNCOND_NEAR_CONTROL_TRANSFER |
								OPCODE_COND_NEAR_CONTROL_TRANSFER |
								OPCODE_MISC_NEAR_CONTROL_TRANSFER)) != 0)
			{
				//Append the MicroCode for this instruction to the MicroCode
				microCode.push_back(instrMicroCode);

				++numInstr;
				break;
			}

			//If this instruction could not have caused an exception
#ifdef LAZY_EIP
			if(!instructionMayCauseException)
			{
				//If the last MicroOp was of TYPE_ADDEIP
				if(microCode.size() != 0 && microCode.back().type == TYPE_ADDEIP)
				{
					//Subtract the offset added from startEip, and remove the microOp
					startEip -= microCode.back().operands[0];
					microCode.pop_back();
				}
			}
#endif

			//Append the MicroCode for this instruction to the MicroCode
			microCode.push_back(instrMicroCode);

			//Append a microOp to advance eip
			microCode.push_back(false, MicroOp(	TYPE_ADDEIP, FUNC_NONE,
												OPERAND_TYPE_IMMEDIATE_32, r_eip - startEip));
		}
#endif
	}

	//Convert the microcode to host code
	microCode.ConvertToHostCode(tempTranslation, hostImmDwordOffsets,
								readByteCallOffsets, readWordCallOffsets, readDwordCallOffsets,
								writeByteCallOffsets, writeWordCallOffsets, writeDwordCallOffsets,
								setDataSegmentRegisterValueCallOffsets);

	//Append an instruction to decrease registers->numInstr by numInstr
	//sub dword [ebp + offsetof(Registers, numInstr)], numInstr
	tempTranslation.push_back(0x81);
	tempTranslation.push_back(0xad);
	tempTranslation.push_back(static_cast<Byte>(offsetof(Registers, numInstr)));
	tempTranslation.push_back(static_cast<Byte>(offsetof(Registers, numInstr) >> 8));
	tempTranslation.push_back(static_cast<Byte>(offsetof(Registers, numInstr) >> 16));
	tempTranslation.push_back(static_cast<Byte>(offsetof(Registers, numInstr) >> 24));
	tempTranslation.push_back(static_cast<Byte>(numInstr));
	tempTranslation.push_back(static_cast<Byte>(numInstr >> 8));
	tempTranslation.push_back(static_cast<Byte>(numInstr >> 16));
	tempTranslation.push_back(static_cast<Byte>(numInstr >> 24));

	//If block chaining is enabled, append a jump to JumpToNextTranslation
#ifdef BLOCK_CHAINING

	bool jumpPatched = false;
	
#ifdef JUMP_PATCHING

	if(!partPageInvalidations[translationStartPhysicalPage])
	{
		//Do not add any patchable jumps if the translation may change the flags
		//If the last instruction was an unconditional near control transfer instruction
		if(	!translationMayChangeCsDsEsSsBasesZeroFlag &&
			(properties & OPCODE_UNCOND_NEAR_CONTROL_TRANSFER) != 0)
		{
			//CMP registers->numInstr, 0
			//JL +5
			//CALL JumpToNextTranslationWithPatch
			EmitALU_Md_Id(	tempTranslation, ALU_FUNC_CMP, REG_EBP, REG_NONE, 0,
							offsetof(Registers, numInstr), 0);
			EmitJCond(tempTranslation, CONDITION_L, 5);
			jumpToNextTranslationWithPatchJumpOffsets.push_back(tempTranslation.size());
			EmitCALL(tempTranslation);

			jumpPatched = true;
		}
		else if(!translationMayChangeCsDsEsSsBasesZeroFlag &&
				(properties & OPCODE_COND_NEAR_CONTROL_TRANSFER) != 0)
		{
			//Otherwise, if it was a conditional near control transfer instruction
			Dword cond = opcode & 0x0f;

			//CMP registers->numInstr, 0
			//JL l1
			//PUSH registers->r_eflags	/ MOV ah, registers->r_eflags
			//POPF						/ SAHF
			//Jcond +5
			//CALL JumpToNextTranslationWithPatch
			//CALL JumpToNextTranslationWithPatch
			//l1:
			EmitALU_Md_Id(	tempTranslation, ALU_FUNC_CMP, REG_EBP, REG_NONE, 0,
							offsetof(Registers, numInstr), 0);
			Dword l1PatchOffset = tempTranslation.size();
			EmitJCond(tempTranslation, CONDITION_L, 5);
			if(ConditionUsesOFlag(cond))
			{
				EmitPUSH_Md(tempTranslation, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
				EmitPOPF(tempTranslation);
			}
			else
			{
				EmitMOV_Gb_Mb(tempTranslation, REG_AH, REG_EBP, REG_NONE, 0, offsetof(Registers, r_eflags));
				EmitSAHF(tempTranslation);
			}
			EmitJCond(tempTranslation, cond, 5);
			jumpToNextTranslationWithPatchJumpOffsets.push_back(tempTranslation.size());
			EmitCALL(tempTranslation);
			jumpToNextTranslationWithPatchJumpOffsets.push_back(tempTranslation.size());
			EmitCALL(tempTranslation);

			tempTranslation[l1PatchOffset + 1] =
				static_cast<Byte>(tempTranslation.size() - (l1PatchOffset + 2));

			jumpPatched = true;
		}
	}

#endif

	if(!jumpPatched)
	{
		//CMP registers->numInstr, 0
		//JL +5
		//JMP JumpToNextTranslation
		EmitALU_Md_Id(	tempTranslation, ALU_FUNC_CMP, REG_EBP, REG_NONE, 0,
						offsetof(Registers, numInstr), 0);
		EmitJCond(tempTranslation, CONDITION_L, 5);
		jumpToNextTranslationJumpOffsets.push_back(tempTranslation.size());
		EmitJMP(tempTranslation);
	}

#endif

	//Append a return instruction
	tempTranslation.push_back(0xc3);

	//Pad the host code to a Dword boundary
	while((tempTranslation.size() & 3) != 0)
		tempTranslation.push_back(0x00);

	//Append any immediate data
	for(std::vector<PatchInfo>::iterator patch = patches.begin();
		patch != patches.end(); ++patch)
	{
		if(patch->type == PATCH_TYPE_DATA)
		{
			Dword translationSize = tempTranslation.size();

			tempTranslation.push_back(static_cast<Byte>(patch->data));
			tempTranslation.push_back(static_cast<Byte>(patch->data >> 8));
			tempTranslation.push_back(static_cast<Byte>(patch->data >> 16));
			tempTranslation.push_back(static_cast<Byte>(patch->data >> 24));

			//Change patch->data to hold the offset within the translation at which the immediate
			//data was inserted
			patch->data = translationSize;
		}
	}

	//Allocate space in the translation cache for this translation
	translation = TranslationCacheAlloc(tempTranslation.size());

	//If the translation cache is full, clear it and retry
	if(!translation)
	{
		ClearTranslationCache();
		translation = TranslationCacheAlloc(tempTranslation.size());
	}

	if(!translation)
		throw Ex("Internal Error: TranslationCacheAlloc failed");

	//Copy the translation to the translation cache
	memcpy(translation, &tempTranslation[0], tempTranslation.size());

	//Patch in the offsets to the helper functions
	const int numHelperFunctions = 10;

	const std::vector <Dword> * const offsetVectors[numHelperFunctions] =
	{
		&readByteCallOffsets, &readWordCallOffsets, &readDwordCallOffsets,
		&writeByteCallOffsets, &writeWordCallOffsets, &writeDwordCallOffsets,
		&setDataSegmentRegisterValueCallOffsets,
		&setDataSegmentRegisterValuePtrCallOffsets,
		&jumpToNextTranslationJumpOffsets,
		&jumpToNextTranslationWithPatchJumpOffsets
	};

	const Dword supervisorModeFunctionAddresses[numHelperFunctions] =
	{
		readByteSupervisorModeAddress, readWordSupervisorModeAddress,
		readDwordSupervisorModeAddress, writeByteSupervisorModeAddress,
		writeWordSupervisorModeAddress, writeDwordSupervisorModeAddress,
		setDataSegmentRegisterValueSupervisorModeAddress,
		setDataSegmentRegisterValuePtrSupervisorModeAddress,
		jumpToNextTranslationAddress, jumpToNextTranslationWithPatchAddress
	};

	const Dword supervisorModeWriteProtectFunctionAddresses[numHelperFunctions] =
	{
		readByteSupervisorModeAddress, readWordSupervisorModeAddress,
		readDwordSupervisorModeAddress, writeByteSupervisorModeWriteProtectAddress,
		writeWordSupervisorModeWriteProtectAddress, writeDwordSupervisorModeWriteProtectAddress,
		setDataSegmentRegisterValueSupervisorModeAddress,
		setDataSegmentRegisterValuePtrSupervisorModeAddress,
		jumpToNextTranslationAddress, jumpToNextTranslationWithPatchAddress
	};

	const Dword userModeFunctionAddresses[numHelperFunctions] =
	{
		readByteUserModeAddress, readWordUserModeAddress,
		readDwordUserModeAddress, writeByteUserModeAddress,
		writeWordUserModeAddress, writeDwordUserModeAddress,
		setDataSegmentRegisterValueUserModeAddress,
		setDataSegmentRegisterValuePtrUserModeAddress,
		jumpToNextTranslationAddress, jumpToNextTranslationWithPatchAddress
	};

	const Dword * functionAddresses = 0;

	if(	(registers->r_cs.descriptorFlags & DESCRIPTOR_FLAGS_DPL_MASK) ==
			DESCRIPTOR_FLAGS_DPL_MASK)
	{
		functionAddresses = userModeFunctionAddresses;
	}
	else if((registers->r_cr0 & CR0_WP_FLAG) != 0)
	{
		functionAddresses = supervisorModeWriteProtectFunctionAddresses;
	}
	else
	{
		functionAddresses = supervisorModeFunctionAddresses;
	}

	for(Dword i = 0; i < numHelperFunctions; ++i)
	{
		for(std::vector<Dword>::const_iterator offset = offsetVectors[i]->begin();
			offset != offsetVectors[i]->end(); ++offset)
		{
			*reinterpret_cast<Dword *>(&translation[*offset + 1]) =
				functionAddresses[i] - reinterpret_cast<Dword>(&translation[*offset + 5]);
		}
	}

	//Make any patches
	for(std::vector<PatchInfo>::const_iterator patch = patches.begin();
		patch != patches.end(); ++patch)
	{
		if(patch->type == PATCH_TYPE_DATA)
		{
			for(std::vector<Dword>::const_iterator i = patch->offsets.begin();
				i != patch->offsets.end(); ++i)
			{
				*reinterpret_cast<Dword *>(&translation[*i]) =
					reinterpret_cast<Dword>(&translation[patch->data]);
			}
		}
		else if(patch->type == PATCH_TYPE_DATA_POINTER)
		{
			for(std::vector<Dword>::const_iterator i = patch->offsets.begin();
				i != patch->offsets.end(); ++i)
			{
				*reinterpret_cast<Dword *>(&translation[*i]) = patch->data;
			}
		}
	}

	return translationCacheable;
}
