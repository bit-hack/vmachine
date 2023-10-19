//	VMachine
//	VMachine class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once
#include <memory>

#include "x86Constants.h"
#include "Registers.h"
#include "OpcodeProperties.h"
#include "PrefixBitset.h"
#include "SegmentRegister/SegmentRegister.h"
#include "SegmentRegister/CodeSegmentRegister/CodeSegmentRegister.h"
#include "SegmentRegister/DataSegmentRegister/DataSegmentRegister.h"
#include "SegmentRegister/StackSegmentRegister/StackSegmentRegister.h"

struct PatchInfo;
class MicroCode;

class Pic;
class DmaController;
class Pit;
class KeyboardController;
class FloppyController;
class IdeController;
class VideoCard;
class SoundCard;

class VMachine
{
public:

	//Constructor/Destructor
	VMachine(	Dword memorySize_,
				const std::string & floppyImageFilename,
				const std::string & hdImageFilename);
	~VMachine(void);
	
	//Copy constructor/Copy assignment operator handled by boost::noncopyable

	//Advance the emulation by one millisecond
	void AdvanceOneMs(Dword msNumber, Dword instrPerMs);

	//Get the required framebuffer size
	std::pair <Dword, Dword> GetFramebufferSize(void);

	//Update the framebuffer
	void UpdateFramebuffer(Dword frameNumber, FramebufferData & framebuffer);

	//Update the sound buffer
	void UpdateSoundBuffer(Dword frameNumber, SoundBufferData & soundBuffer);

	//Set the state of a key
	void SetKeyState(Byte scancode, bool pressed);

	//Set the state of the mouse
	void SetMouseState(	SignedWord dx, SignedWord dy,
						bool lButtonDown, bool mButtonDown,	bool rButtonDown);

	//Insert/eject a floppy disk
	void InsertFloppyDisk(const std::string & imageFilename);
	void EjectFloppyDisk(void);

	bool IsFloppyDiskInserted(void);

private:

	//Load the helper functions
	void LoadHelperFunctions(WORD resourceID);

	//Helper functions called from translated code
	Dword helperFunctionsSize;
	Byte * helperFunctions;
		
	Dword readByteSupervisorModeAddress, readByteUserModeAddress;
	Dword readWordSupervisorModeAddress, readWordUserModeAddress;
	Dword readDwordSupervisorModeAddress, readDwordUserModeAddress;
	Dword writeByteSupervisorModeAddress;
	Dword writeByteSupervisorModeWriteProtectAddress;
	Dword writeByteUserModeAddress;
	Dword writeWordSupervisorModeAddress;
	Dword writeWordSupervisorModeWriteProtectAddress;
	Dword writeWordUserModeAddress;
	Dword writeDwordSupervisorModeAddress;
	Dword writeDwordSupervisorModeWriteProtectAddress;
	Dword writeDwordUserModeAddress;
	Dword setDataSegmentRegisterValueSupervisorModeAddress;
	Dword setDataSegmentRegisterValueUserModeAddress;
	Dword setDataSegmentRegisterValuePtrSupervisorModeAddress;
	Dword setDataSegmentRegisterValuePtrUserModeAddress;
	Dword jumpToNextTranslationAddress;
	Dword jumpToNextTranslationWithPatchAddress;

	//Load the instruction translations
	void LoadInstructionTranslations(WORD resourceID);

	//Structure holding information on an instruction translation
	struct Translation
	{
		std::vector <Byte> translation;

		std::vector <Dword> ReadByteOffsets, ReadWordOffsets, ReadDwordOffsets;
		std::vector <Dword> WriteByteOffsets, WriteWordOffsets, WriteDwordOffsets;
		std::vector <Dword> SetDataSegmentRegisterValueOffsets;
		std::vector <Dword> SetDataSegmentRegisterValuePtrOffsets;
		std::vector <Dword> segRegBaseOffsets;
		std::vector <Dword> baseRegOffsets, indexRegOffsets;
		std::vector <Dword> indexShiftOffsets, displacementOffsets;
		std::vector <Dword> RbOffsets, GbOffsets;
		std::vector <Dword> RwOffsets, GwOffsets;
		std::vector <Dword> RdOffsets, GdOffsets;
		std::vector <Dword> SwOffsets;
		std::vector <Dword> immByteOffsets, immWordOffsets, immDwordOffsets;
		std::vector <Dword> instrLengthOffsets;
	};

	//Instruction translations, indexed by opcode
	std::map <Dword, Translation> translations;

	//Execute instructions
	void ExecuteInstructions(void);

	//Read the opcode at a given eip
	bool ReadOpcode(Dword r_eip, Dword & opcode, OpcodeProperties & properties,
					PrefixBitset & prefixes, Dword & numBytes);

	//Set the value of a segment register
	bool SetCSValue(Word selector);
	bool SetDataSegmentRegisterValue(SegmentRegister * segReg, Word selector);
	bool SetSSValue(Word selector);

	//Translate the instructions at a given eip
	bool TranslateInstructions(	Dword & r_eip, Dword physicalEip, Byte *& translation,
								std::vector<Dword> & guestImmDwordOffsets,
								std::vector<Dword> & hostImmDwordOffsets);

	//Add the translation for an instruction to a translation
	bool AddInstructionToTranslation(	bool addIfCrossesPageBoundary,
										Dword opcode, OpcodeProperties properties,
										PrefixBitset prefixes,
										Dword startEip, Dword & r_eip,
										std::vector <Byte> & translation,
										std::vector <Dword> & readByteCallOffsets, std::vector <Dword> & readWordCallOffsets, std::vector <Dword> & readDwordCallOffsets,
										std::vector <Dword> & writeByteCallOffsets, std::vector <Dword> & writeWordCallOffsets, std::vector <Dword> & writeDwordCallOffsets,
										std::vector <Dword> & setDataSegmentRegisterValueCallOffsets,
										std::vector <Dword> & setDataSegmentRegisterValuePtrCallOffsets,
										std::vector <PatchInfo> & patches);

	//Convert a guest instruction to microcode
	bool ConvertGuestInstructionToMicroCode(Dword opcode, PrefixBitset prefixes,
											Dword startEip, Dword & r_eip,
											bool & translationMayChangeCsDsEsSsBasesZeroFlag,
											MicroCode & microCode, Dword & guestImmDwordOffset,
											bool & instructionMayCauseException);

	//Output an effective address calculation as microcode
	bool OutputEffectiveAddressCalculation(	Dword opcode, PrefixBitset prefixes,
											bool translationMayChangeCsDsEsSsBasesZeroFlag,
											Dword & r_eip, MicroCode & microCode,
											Dword & Gd, bool addSegRegBase = true);

	//Translation cache
	HANDLE translationCache;

	Byte * TranslationCacheAlloc(Dword numBytes)
	{ return reinterpret_cast<Byte *>(HeapAlloc(translationCache, 0, numBytes)); }

	void TranslationCacheFree(Byte * ptr)
	{ HeapFree(translationCache, 0, ptr); }

	void ClearTranslationCache(void);

	//Map a guest physical eip to a cached translation
	struct EipToTranslationCacheMapKey
	{
		Dword physicalEip;
		Dword flags;
		
		Dword Hash() const
		{
			return physicalEip;
		}

		bool operator==(const EipToTranslationCacheMapKey & rhs) const
		{
			return (physicalEip == rhs.physicalEip) && (flags == rhs.flags);
		}

		bool operator<(const EipToTranslationCacheMapKey & rhs) const
		{
			return	(physicalEip < rhs.physicalEip) ||
					((physicalEip == rhs.physicalEip) && (flags < rhs.flags));
		}
	};

	struct HashEipToTranslationCacheMapKey
	{
		static const size_t bucket_size = 4;
		static const size_t min_buckets = 8;

		Dword operator()(const EipToTranslationCacheMapKey & key) const
		{ return key.Hash(); }

		bool operator()(const EipToTranslationCacheMapKey & lhs,
						const EipToTranslationCacheMapKey & rhs) const
		{ return lhs < rhs; }
	};

	struct EipToTranslationCacheMapValue
	{
		Byte * translation;
		Dword endPhysicalEip;
		std::unordered_map <Dword, Dword> immDwordOffsets;
	};

	typedef std::unordered_map <EipToTranslationCacheMapKey,
		std::shared_ptr<EipToTranslationCacheMapValue>,
			HashEipToTranslationCacheMapKey>
					EipToTranslationCacheMapType;

	EipToTranslationCacheMapType eipToTranslationCacheMap;

	//Eip to translation cache TLB
	struct EipToTranslationCacheTLBEntry
	{
		EipToTranslationCacheMapKey key;
		std::shared_ptr <EipToTranslationCacheMapValue> value;
	};

	static const Dword eipToTranslationCacheTLBSize = 0x1000;	//Must be a power of 2
	static const Dword eipToTranslationCacheTLBMask = eipToTranslationCacheTLBSize - 1;

	typedef std::vector <EipToTranslationCacheTLBEntry> EipToTranslationCacheTLBType;
	EipToTranslationCacheTLBType eipToTranslationCacheTLB;

	//Physical eips for which translations exist, by page
	typedef std::list < std::pair < EipToTranslationCacheMapKey, 
		std::shared_ptr<EipToTranslationCacheMapValue> > >
			PageTranslationListType;
	
	std::vector <PageTranslationListType> pageTranslationLists;

	//Invalidate code translations which intersect the address range [start, end)
public:
	void InvalidateCodeTranslations(Dword startAddress, Dword endAddress, Dword data);
private:

	//Update the partPageInvalidations array
	void UpdatePartPageInvalidations(void);

	std::vector <Dword> pageInvalidationCounts;
	std::vector <bool> partPageInvalidationPendings;
	std::vector <bool> partPageInvalidations;

	//The translations which need freeing
	std::vector <Byte *> translationsNeedingFreeing;

	//Decode a ModR/M byte
	//Used by ConvertGuestInstructionToMicroCode
	bool DecodeModRMByteRegReg(Dword & r_eip, Dword & Gd, Dword & Rd);
	bool DecodeModRMByteRegMemoryA16(	Dword & r_eip, Dword & Gd, Dword & segRegBase,
										Dword & baseReg, Dword & indexReg,
										Dword & displacement);
	bool DecodeModRMByteRegMemoryA32(	Dword & r_eip, Dword & Gd, Dword & segRegBase,
										Dword & baseReg, Dword & indexReg, Dword & shift,
										Dword & displacement);

	//Used by function implementations (AddInstructionToTranslation)
	bool DecodeModRMByteA16(Dword r_eip, Byte & modRM,
							SegmentRegister *& segReg, Dword *& baseReg, Dword *& indexReg, Dword & displacement,
							Byte *& Rb, Byte *& Gb, Word *& Rw, Word *& Gw, Dword *& Rd, Dword *& Gd, SegmentRegisterData *& Sw,
							Dword & modRMSize);

	bool DecodeModRMByteA32(Dword r_eip, Byte & modRM,
							SegmentRegister *& segReg, Dword *& baseReg, Dword *& indexReg, Dword & indexShift, Dword & displacement,
							Byte *& Rb, Byte *& Gb, Word *& Rw, Word *& Gw, Dword *& Rd, Dword *& Gd, SegmentRegisterData *& Sw,
							Dword & modRMSize);

	//Used by interpretive implementations
	bool DecodeModRMByteRegRegWord(Dword & r_eip, Word *& reg1, Word *& reg2);
	bool DecodeModRMByteSegRegRegWord(Dword & r_eip, SegmentRegister *& segReg, Word *& reg);
	bool DecodeModRMByteRegRegDword(Dword & r_eip, Dword *& reg1, Dword *& reg2);
	bool DecodeModRMByteRegMemoryWord(	Dword & r_eip, Dword opcode, Word *& reg1,
										SegmentRegister *& defaultSegReg, Dword & offset);
	bool DecodeModRMByteSegRegMemoryWord(	Dword & r_eip, Dword opcode, SegmentRegister *& segReg,
											SegmentRegister *& defaultSegReg, Dword & offset);
	bool DecodeModRMByteRegMemoryDword(	Dword & r_eip, Dword opcode, Dword *& reg1,
										SegmentRegister *& defaultSegReg, Dword & offset);

	bool DecodeModRMByteRegMemoryA16Word(	Dword & r_eip, Word *& reg1,
											SegmentRegister *& defaultSegReg, Dword & offset);
	bool DecodeModRMByteRegMemoryA16Dword(	Dword & r_eip, Dword *& reg1,
											SegmentRegister *& defaultSegReg, Dword & offset);
	bool DecodeModRMByteRegMemoryA32Word(	Dword & r_eip, Word *& reg1,
											SegmentRegister *& defaultSegReg, Dword & offset);
	bool DecodeModRMByteRegMemoryA32Dword(	Dword & r_eip, Dword *& reg1,
											SegmentRegister *& defaultSegReg, Dword & offset);

	//Get the override segment register to use, if any
	SegmentRegister * GetOverrideSegReg(PrefixBitset prefixes);

	//Execute a translation
	void ExecuteTranslation(Byte * translation);

	//Interpret an instruction
	void InterpretInstruction(Dword opcode, PrefixBitset prefixes);

	//Instruction implementations
	void POP_SS_O16(PrefixBitset prefixes);		void POP_SS_O32(PrefixBitset prefixes);
	void MOV_Sw_Rw(PrefixBitset prefixes);	void MOV_Sw_Mw(Dword opcode, PrefixBitset prefixes);
	void LSS_Gw_Mp(Dword opcode, PrefixBitset prefixes);
	void LSS_Gd_Mp(Dword opcode, PrefixBitset prefixes);

	void JMP_Rw(PrefixBitset prefixes);		void JMP_Mw(Dword opcode, PrefixBitset prefixes);
	void JMP_Rd(PrefixBitset prefixes);		void JMP_Md(Dword opcode, PrefixBitset prefixes);
	void CALL_Rw(PrefixBitset prefixes);	void CALL_Mw(Dword opcode, PrefixBitset prefixes);
	void CALL_Rd(PrefixBitset prefixes);	void CALL_Md(Dword opcode, PrefixBitset prefixes);

	void DoFarJump(Word newCs, Dword newEip);
	void JMP_FAR_Ap(Dword opcode, PrefixBitset prefixes);
	void JMP_FAR_Mp(Dword opcode, PrefixBitset prefixes);

	void DoFarCall(Dword opcode, Word newCs, Dword newEip);
	void CALL_FAR_Ap(Dword opcode, PrefixBitset prefixes);
	void CALL_FAR_Mp(Dword opcode, PrefixBitset prefixes);

	enum IntType
	{
		INT_TYPE_HARDWARE = 0, INT_TYPE_SOFTWARE, INT_TYPE_EXCEPTION
	};

	void DoInt(Byte intNum, IntType intType, Dword errorCode);
	void INT_Ib(PrefixBitset prefixes);

	void DoFarReturn(Dword opcode, bool iret, Word stackPointerOffset);
	void RETF(Dword opcode, PrefixBitset prefixes);
	void RETF_Iw(Dword opcode, PrefixBitset prefixes);
	void IRET(Dword opcode, PrefixBitset prefixes);

	void IN_AL_Ib(PrefixBitset prefixes);		void IN_AL_DX(PrefixBitset prefixes);
	void IN_AX_Ib(PrefixBitset prefixes);		void IN_AX_DX(PrefixBitset prefixes);
	void IN_EAX_Ib(PrefixBitset prefixes);		void IN_EAX_DX(PrefixBitset prefixes);
	void OUT_Ib_AL(PrefixBitset prefixes);		void OUT_DX_AL(PrefixBitset prefixes);
	void OUT_Ib_AX(PrefixBitset prefixes);		void OUT_DX_AX(PrefixBitset prefixes);
	void OUT_Ib_EAX(PrefixBitset prefixes);		void OUT_DX_EAX(PrefixBitset prefixes);

	void INS_Yb_DX_A16(PrefixBitset prefixes);	void INS_Yb_DX_A32(PrefixBitset prefixes);
	void INS_Yw_DX_A16(PrefixBitset prefixes);	void INS_Yw_DX_A32(PrefixBitset prefixes);
	void INS_Yd_DX_A16(PrefixBitset prefixes);	void INS_Yd_DX_A32(PrefixBitset prefixes);
	void OUTS_DX_Xb_A16(PrefixBitset prefixes);	void OUTS_DX_Xb_A32(PrefixBitset prefixes);
	void OUTS_DX_Xw_A16(PrefixBitset prefixes);	void OUTS_DX_Xw_A32(PrefixBitset prefixes);
	void OUTS_DX_Xd_A16(PrefixBitset prefixes);	void OUTS_DX_Xd_A32(PrefixBitset prefixes);

	void CLI(PrefixBitset prefixes);			void STI(PrefixBitset prefixes);

	void POPF_O16(PrefixBitset prefixes);		void POPF_O32(PrefixBitset prefixes);

	void SLDT_Rw(Dword opcode);				void SLDT_Mw(Dword opcode, PrefixBitset prefixes);
	void STR_Rw(Dword opcode);				void STR_Mw(Dword opcode, PrefixBitset prefixes);
	void LLDT_Rw(Dword opcode);				void LLDT_Mw(Dword opcode, PrefixBitset prefixes);
	void LTR_Rw(Dword opcode);				void LTR_Mw(Dword opcode, PrefixBitset prefixes);

	void DoVERR(Word selector);
	void VERR_Rw(PrefixBitset prefixes);	void VERR_Mw(Dword opcode, PrefixBitset prefixes);

	void DoVERW(Word selector);
	void VERW_Rw(PrefixBitset prefixes);	void VERW_Mw(Dword opcode, PrefixBitset prefixes);

	void SGDT_Ms_O16(Dword opcode, PrefixBitset prefixes);
	void SGDT_Ms_O32(Dword opcode, PrefixBitset prefixes);
	void SIDT_Ms_O16(Dword opcode, PrefixBitset prefixes);
	void SIDT_Ms_O32(Dword opcode, PrefixBitset prefixes);
	void LGDT_Ms_O16(Dword opcode, PrefixBitset prefixes);
	void LGDT_Ms_O32(Dword opcode, PrefixBitset prefixes);
	void LIDT_Ms_O16(Dword opcode, PrefixBitset prefixes);
	void LIDT_Ms_O32(Dword opcode, PrefixBitset prefixes);
	void SMSW_Rw(PrefixBitset prefixes);	void SMSW_Mw(Dword opcode, PrefixBitset prefixes);
	void LMSW_Rw(PrefixBitset prefixes);	void LMSW_Mw(Dword opcode, PrefixBitset prefixes);
	void INVLPG_M(Dword opcode, PrefixBitset prefixes);

	void DoLAR_O16(Word * Gw, Word selector);	bool DoLAR_O32(Dword * Gd, Word selector);
	void LAR_Gw_Rw(PrefixBitset prefixes);		void LAR_Gw_Mw(Dword opcode, PrefixBitset prefixes);
	void LAR_Gd_Rw(PrefixBitset prefixes);		void LAR_Gd_Mw(Dword opcode, PrefixBitset prefixes);
	
	void DoLSL_O16(Word * Gw, Word selector);	bool DoLSL_O32(Dword * Gd, Word selector);
	void LSL_Gw_Rw(PrefixBitset prefixes);		void LSL_Gw_Mw(Dword opcode, PrefixBitset prefixes);
	void LSL_Gd_Rw(PrefixBitset prefixes);		void LSL_Gd_Mw(Dword opcode, PrefixBitset prefixes);

	void MOV_Cd_Rd(PrefixBitset prefixes);		void MOV_Dd_Rd(PrefixBitset prefixes);

	//Set the value of a control register
	void SetCR0Value(Dword newCr0);
	void SetCR3Value(Dword newCr3);

	//Switch tasks
	enum SwitchTasksType
	{
		SWITCH_TASKS_JUMP = 0, SWITCH_TASKS_CALL, SWITCH_TASKS_INT, SWITCH_TASKS_IRET
	};

	void SwitchTasks(SwitchTasksType type, Word selector, Dword descriptorLow, Dword descriptorHigh);

	//Read ss:esp from the TSS
	bool ReadSsEspFromTss(Dword privilegeLevel, Word & newSs, Dword & newEsp);

	//Is an I/O port access permitted by the I/O permission bitmap?
	bool IsIoPermittedByBitmap(Dword port, Byte accessSize);

	//Handle an exception
	void HandleException(void);

	//Flush the TLBs
	void FlushTLBs(void);

	//Set/clear a flag for a given physical page in the write TLBs
	void SetWriteTLBsFlag(Dword physicalPage, Dword flag);
	void ClearWriteTLBsFlag(Dword physicalPage, Dword flag);

	//CPU registers
	const std::shared_ptr<Registers> registers;

	CodeSegmentRegister r_cs;
	DataSegmentRegister r_ds, r_es, r_fs, r_gs;
	StackSegmentRegister r_ss;

	Dword readCodeException, readCodeErrorCode, readCodeCr2;

	//The values of eip and esp at the start of an instruction
	Dword instrStartEip;
	Dword instrStartEsp;

	//Raise/lower the INT line
	friend class Pic;

	void RaiseINT(void)
	{ intRaised = true; }

	void LowerINT(void)
	{ intRaised = false; }

	//Handle an interrupt
	void HandleInterrupt(void);

	//Is the INT line raised?
	bool intRaised;

	//Switch FPU contexts
	void SwitchToVMachineFpuContext(void);
	void SwitchToHostFpuContext(void);

	//Floating point save areas
	void * fpuSaveArea;
	void * hostFpuSaveArea;

	//Initialise the contents of memory
	void InitMemoryContents(void);

	//Calculate a physical address
	bool CalculatePhysicalAddress(Dword linearAddress, Dword & physicalAddress);

	//Read memory
	friend class DmaController;

public:
	//Public in order to be called from the global functions
	Byte ReadBytePhysical(Dword address);
	Word ReadWordPhysical(Dword address);
	Dword ReadDwordPhysical(Dword address);
private:

	bool TranslateAddressForRead(bool userMode, Dword & address);

	bool ReadByteLinear(Dword address, Byte & data, bool forceSupervisorMode = false);
	bool ReadWordLinear(Dword address, Word & data, bool forceSupervisorMode = false);
	bool ReadDwordLinear(Dword address, Dword & data, bool forceSupervisorMode = false);

	bool ReadByte(const SegmentRegister & segReg, Dword offset, Byte & data);
	bool ReadWord(const SegmentRegister & segReg, Dword offset, Word & data);
	bool ReadDword(const SegmentRegister & segReg, Dword offset, Dword & data);

	Byte ReadCodeBytePhysical(Dword address);
	Word ReadCodeWordPhysical(Dword address);
	Dword ReadCodeDwordPhysical(Dword address);

	bool TranslateAddressForReadCode(bool userMode, Dword & address);

	bool ReadCodeByte(Dword offset, Byte & data);
	bool ReadCodeWord(Dword offset, Word & data);
	bool ReadCodeDword(Dword offset, Dword & data);

	//Write memory
public:
	void WriteBytePhysical(Dword address, Byte data);
	void WriteWordPhysical(Dword address, Word data);
	void WriteDwordPhysical(Dword address, Dword data);
private:

	bool TranslateAddressForWrite(bool userMode, Dword & address);

	bool WriteByteLinear(Dword address, Byte data, bool forceSupervisorMode = false);
	bool WriteWordLinear(Dword address, Word data, bool forceSupervisorMode = false);
	bool WriteDwordLinear(Dword address, Dword data, bool forceSupervisorMode = false);

	bool WriteByte(const SegmentRegister & segReg, Dword offset, Byte data);
	bool WriteWord(const SegmentRegister & segReg, Dword offset, Word data);
	bool WriteDword(const SegmentRegister & segReg, Dword offset, Dword data);

	//Access the stack
	bool PushWord(Word data);
	bool PushDword(Dword data);
	
	bool PopWord(Word & data);
	bool PopDword(Dword & data);

	bool ReadStackWord(Dword offset, Word & data);
	bool ReadStackDword(Dword offset, Dword & data);

	bool WriteStackWord(Dword offset, Word data);
	bool WriteStackDword(Dword offset, Dword data);

	//Toggle the A20 line
	friend class KeyboardController;

	void EnableA20Line(void)
	{
		if(!a20Enabled)
			FlushTLBs();
		
		a20Enabled = true;
		registers->addressMask = ~0;
	}

	void DisableA20Line(void)
	{
		if(a20Enabled)
			FlushTLBs();
		
		a20Enabled = false;
		registers->addressMask = ~(1 << 20);
	}

	bool GetA20LineStatus(void)
	{ return a20Enabled; }

	bool a20Enabled;
	
	//Memory
	Byte * memory;

	//Access I/O address space
	Byte InputByte(Word address);
	Word InputWord(Word address);
	Dword InputDword(Word address);
	
	void OutputByte(Word address, Byte data);
	void OutputWord(Word address, Word data);
	void OutputDword(Word address, Dword data);

	//Other components
	std::unique_ptr<Pic> masterPic, slavePic;
	std::unique_ptr<DmaController> dmaController;
	std::unique_ptr<Pit> pit;
	std::unique_ptr<KeyboardController> keyboardController;
	std::unique_ptr<FloppyController> floppyController;
	std::unique_ptr<IdeController> ideController;
	std::unique_ptr<VideoCard> videoCard;
	std::unique_ptr<SoundCard> soundCard;

public:

	//Exception thrown by VMachine functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};

//Global functions required to call VMachine member functions from code translations
Byte ReadBytePhysical(Dword vmachineThisPtr, Dword address);
Word ReadWordPhysical(Dword vmachineThisPtr, Dword address);
Dword ReadDwordPhysical(Dword vmachineThisPtr, Dword address);

void WriteBytePhysical(Dword vmachineThisPtr, Dword address, Byte data);
void WriteWordPhysical(Dword vmachineThisPtr, Dword address, Word data);
void WriteDwordPhysical(Dword vmachineThisPtr, Dword address, Dword data);

void InvalidateCodeTranslations(Dword vmachineThisPtr, Dword startAddress, Dword endAddress, Dword data);
