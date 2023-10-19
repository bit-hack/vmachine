//	VMachine
//	Clear the translation cache
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::ClearTranslationCache(void)
{
	//Destroy and recreate the translation cache (fixed size: 16MB)
	HeapDestroy(translationCache);

	translationCache = HeapCreate(
		HEAP_NO_SERIALIZE | HEAP_CREATE_ENABLE_EXECUTE, 0x1000000, 0x1000000);
	if(!translationCache)
		throw Ex("Internal Error: HeapCreate [translationCache] failed.");

	//Clear the eip to translation cache map and TLBs
	eipToTranslationCacheMap.clear();

	for(EipToTranslationCacheTLBType::iterator i = eipToTranslationCacheTLB.begin();
		i != eipToTranslationCacheTLB.end(); ++i)
	{
		i->key.flags = TRANSLATION_EMPTY_FLAG;
	}

	for(Dword i = 0; i < EIP_TO_TRANSLATION_TLB_SIZE; ++i)
		registers->eipToTranslationTLB[i].flags = EIP_TO_TRANSLATION_TLB_EMPTY_FLAG;

	//Clear the page translation list
	for(Dword i = 0; i < (registers->memorySize >> 12); ++i)
		pageTranslationLists[i].resize(0);

	//Reset the page classifications
	for(Dword i = 0; i < (registers->memorySize >> 12); ++i)
		registers->pageClassifications[i] = PAGE_CLASS_NORMAL;

	for(Dword i = (0xa0000 >> 12); i < (0xc0000 >> 12); ++i)
		registers->pageClassifications[i] = PAGE_CLASS_MMIO;
}
