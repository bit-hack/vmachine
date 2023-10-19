//	VMachine
//	VMachine Constructor/Destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "Pic/Pic.h"
#include "DmaController/DmaController.h"
#include "Pit/Pit.h"
#include "KeyboardController/KeyboardController.h"
#include "FloppyController/FloppyController.h"
#include "IdeController/IdeController.h"
#include "VideoCard/VideoCard.h"
#include "SoundCard/SoundCard.h"
#include "../Resources/resource.h"

//Disable warning C4355: 'this' : used in base member initializer list
//It is OK to use 'this' in an intializer list, as long as the object being initialised does
//not call any functions using it
#pragma warning(push)
#pragma warning(disable: 4355)

//Constructor
VMachine::VMachine(	Dword memorySize_,
					const std::string & floppyImageFilename,
					const std::string & hdImageFilename)
	:	helperFunctionsSize(0), helperFunctions(0), 		
		eipToTranslationCacheTLB(eipToTranslationCacheTLBSize),
		pageTranslationLists(memorySize_ >> 12),
		pageInvalidationCounts(memorySize_ >> 12, 0),
		partPageInvalidationPendings(memorySize_ >> 12, false),
		partPageInvalidations(memorySize_ >> 12, false),
		registers(new Registers),
		r_cs(registers, registers->r_cs),
		r_ds(registers, registers->r_ds), r_es(registers, registers->r_es),
		r_fs(registers, registers->r_fs), r_gs(registers, registers->r_gs),
		r_ss(registers, registers->r_ss),
		readCodeException(EXCEPTION_NONE), readCodeErrorCode(0), readCodeCr2(0),
		intRaised(false), a20Enabled(false),
		memory(0),
		masterPic(new Pic(true, *this, 0x08)), slavePic(new Pic(false, *this, 0x70)),
		dmaController(new DmaController(*this)),
		pit(new Pit(*this, *masterPic)),
		keyboardController(new KeyboardController(*this, *masterPic, *slavePic)),
		floppyController(new FloppyController(floppyImageFilename, *masterPic, *dmaController)),
		ideController(new IdeController(hdImageFilename, *slavePic)),
		videoCard(new VideoCard),
		soundCard(new SoundCard(*masterPic, *dmaController))
{
	//Initialise the registers
	ZeroMemory(registers.get(), sizeof(Registers));

	registers->r_eip = 0x0000fff0;
	registers->r_systemFlags = 0x00000002;
	
	registers->r_cs.selector = 0xf000;
	registers->r_cs.base = 0x000f0000;
	registers->r_cs.limit = 0x0000ffff;
	registers->r_cs.descriptorFlags = 0x00009b00;

	registers->r_ds.limit = 0x0000ffff;	registers->r_ds.descriptorFlags = 0x00009300;
	registers->r_es.limit = 0x0000ffff;	registers->r_es.descriptorFlags = 0x00009300;
	registers->r_fs.limit = 0x0000ffff;	registers->r_fs.descriptorFlags = 0x00009300;
	registers->r_gs.limit = 0x0000ffff;	registers->r_gs.descriptorFlags = 0x00009300;
	registers->r_ss.limit = 0x0000ffff;	registers->r_ss.descriptorFlags = 0x00009300;

	registers->r_cr0 = 0x60000010;

	registers->exception = EXCEPTION_NONE;

	registers->zero = 0x00000000;

	registers->memorySize = memorySize_;
	registers->addressMask = ~(1 << 20);

	//Allocate and initialise the TLBs
	registers->supervisorReadTlb	= new TlbEntry[TLB_SIZE];
	registers->userReadTlb			= new TlbEntry[TLB_SIZE];
	registers->supervisorWriteTlb	= new TlbEntry[TLB_SIZE];
	registers->supervisorWriteWPTlb	= new TlbEntry[TLB_SIZE];
	registers->userWriteTlb			= new TlbEntry[TLB_SIZE];

	FlushTLBs();

	//Allocate and fill the array of page classifications
	registers->pageClassifications = new Byte[registers->memorySize >> 12];

	for(Dword i = 0; i < (registers->memorySize >> 12); ++i)
		registers->pageClassifications[i] = PAGE_CLASS_NORMAL;

	//Mark the VGA memory area as MMIO
	for(Dword i = (0xa0000 >> 12); i < (0xc0000 >> 12); ++i)
		registers->pageClassifications[i] = PAGE_CLASS_MMIO;

	//Initialise the vmachineThisPtr and the function pointers
	registers->vmachineThisPtr = reinterpret_cast<Dword>(this);
	registers->ReadBytePhysical = ::ReadBytePhysical;
	registers->ReadWordPhysical = ::ReadWordPhysical;
	registers->ReadDwordPhysical = ::ReadDwordPhysical;
	registers->WriteBytePhysical = ::WriteBytePhysical;
	registers->WriteWordPhysical = ::WriteWordPhysical;
	registers->WriteDwordPhysical = ::WriteDwordPhysical;
	registers->InvalidateCodeTranslations = ::InvalidateCodeTranslations;

	//Load the helper functions
	LoadHelperFunctions(IDR_HELPER);

	registers->jumpToNextTranslationAddress = jumpToNextTranslationAddress;

	//Load the instruction translations
	LoadInstructionTranslations(IDR_BINARITHINSTR);
	LoadInstructionTranslations(IDR_BITBYTEINSTR);
	LoadInstructionTranslations(IDR_DATATRANSFERINSTR);
	LoadInstructionTranslations(IDR_DECARITHINSTR);
	LoadInstructionTranslations(IDR_FLAGCONTROLINSTR);
	LoadInstructionTranslations(IDR_FPUINSTR);
	LoadInstructionTranslations(IDR_NEARCONTROLINSTR);
	LoadInstructionTranslations(IDR_SEGREGINSTR);
	LoadInstructionTranslations(IDR_SHIFTINSTR);
	LoadInstructionTranslations(IDR_STRINGINSTR);
	LoadInstructionTranslations(IDR_MISCINSTR);

	//Create the translation cache (fixed size: 16MB)
	translationCache = HeapCreate(
		HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, 0x1000000, 0x1000000);
	if(!translationCache)
		throw Ex("Internal Error: HeapCreate [translationCache] failed.");

	//Initialise the eip to translation cache TLB
	for(EipToTranslationCacheTLBType::iterator i = eipToTranslationCacheTLB.begin();
		i != eipToTranslationCacheTLB.end(); ++i)
	{
		i->key.flags = TRANSLATION_EMPTY_FLAG;
	}

	//Allocate and initialise the eip to translation TLB
	registers->eipToTranslationTLB = new EipToTranslationTLBEntry[EIP_TO_TRANSLATION_TLB_SIZE];

	for(Dword i = 0; i < EIP_TO_TRANSLATION_TLB_SIZE; ++i)
		registers->eipToTranslationTLB[i].flags = EIP_TO_TRANSLATION_TLB_EMPTY_FLAG;

	//Reserve and commit the FPU save areas
	fpuSaveArea = VirtualAlloc(0, 0x100, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
	hostFpuSaveArea = VirtualAlloc(0, 0x100, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

	if(!fpuSaveArea || !hostFpuSaveArea)
		throw Ex("Internal Error: VirtualAlloc [fpuSaveArea, hostFpuSaveArea] failed.");

	//Initialise the FPU context
	void * pFpuSaveArea = fpuSaveArea;
	void * pHostFpuSaveArea = hostFpuSaveArea;

	__asm
	{
		mov eax, pFpuSaveArea
		mov ecx, pHostFpuSaveArea
		
		fxsave [ecx]
		fninit
		fxsave [eax]
		fxrstor [ecx]
	}

	//Reserve and commit the memory
	//Allocate 2 extra pages on the end of the memory array
	memory = reinterpret_cast<Byte *>(
		VirtualAlloc(	0, registers->memorySize + 0x2000,
						MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));

	if(!memory)
		throw Ex("Internal Error: VirtualAlloc [memory] failed");

	registers->memoryPtr = reinterpret_cast<Dword>(memory);

	//Fill the first of the extra 2 pages with 0xff
	memset(memory + registers->memorySize, 0xff, 0x1000);

	//Initialise the contents of memory
	InitMemoryContents();

	//Link the PICs
	masterPic->SetOtherPic(slavePic.get());
	slavePic->SetOtherPic(masterPic.get());
}

#pragma warning(pop)

//Destructor
VMachine::~VMachine(void)
{
	//Delete the array of page classifications and the eip to translation TLB
	delete [] registers->pageClassifications;
	delete [] registers->eipToTranslationTLB;

	//Delete the TLBs
	delete [] registers->supervisorReadTlb;
	delete [] registers->userReadTlb;
	delete [] registers->supervisorWriteTlb;
	delete [] registers->supervisorWriteWPTlb;
	delete [] registers->userWriteTlb;

	//Destroy the translation cache
	HeapDestroy(translationCache);

	//Release the memory, FPU save areas and helper functions
	VirtualFree(memory, registers->memorySize + 0x2000, MEM_RELEASE);
	VirtualFree(fpuSaveArea, 0x100, MEM_RELEASE);
	VirtualFree(hostFpuSaveArea, 0x100, MEM_RELEASE);
	VirtualFree(helperFunctions, helperFunctionsSize, MEM_RELEASE);
}

//Global functions required to call VMachine member functions from code translations
Byte ReadBytePhysical(Dword vmachineThisPtr, Dword address)
{
	return reinterpret_cast<VMachine *>(vmachineThisPtr)->ReadBytePhysical(address);
}

Word ReadWordPhysical(Dword vmachineThisPtr, Dword address)
{
	return reinterpret_cast<VMachine *>(vmachineThisPtr)->ReadWordPhysical(address);
}

Dword ReadDwordPhysical(Dword vmachineThisPtr, Dword address)
{
	return reinterpret_cast<VMachine *>(vmachineThisPtr)->ReadDwordPhysical(address);
}

void WriteBytePhysical(Dword vmachineThisPtr, Dword address, Byte data)
{
	reinterpret_cast<VMachine *>(vmachineThisPtr)->WriteBytePhysical(address, data);
}

void WriteWordPhysical(Dword vmachineThisPtr, Dword address, Word data)
{
	reinterpret_cast<VMachine *>(vmachineThisPtr)->WriteWordPhysical(address, data);
}

void WriteDwordPhysical(Dword vmachineThisPtr, Dword address, Dword data)
{
	reinterpret_cast<VMachine *>(vmachineThisPtr)->WriteDwordPhysical(address, data);
}

void InvalidateCodeTranslations(Dword vmachineThisPtr, Dword startAddress, Dword endAddress, Dword data)
{
	reinterpret_cast<VMachine *>(vmachineThisPtr)->InvalidateCodeTranslations(startAddress, endAddress, data);
}
