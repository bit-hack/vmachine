//	VMachine
//	Invalidate code translations which intersect the address range [start, end)
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"

void VMachine::InvalidateCodeTranslations(	Dword startAddress, Dword endAddress, Dword data)
{
	//Loop through pages covered by the address range
	for(Dword page = startAddress >> 12; page <= (endAddress - 1) >> 12; ++page)
	{
		//Does this page require only partial invalidation?
		if(partPageInvalidations[page])
		{
			//Loop through translations on the given page
			for(PageTranslationListType::iterator translation = pageTranslationLists[page].begin();
				translation != pageTranslationLists[page].end(); )
			{
				//Does this translation intersect the given address range?
				if(	translation->first.physicalEip < endAddress &&
					translation->second->endPhysicalEip >= startAddress)
				{
					//If so, is this simply a patching of an immediate dword?
					if(endAddress - startAddress == 4)
					{
						auto immDwordIt =
							translation->second->immDwordOffsets.find(
								startAddress - translation->first.physicalEip);

						if(immDwordIt != translation->second->immDwordOffsets.end())
						{
							//If so, patch the dword and continue with the next translation
							*reinterpret_cast<Dword *>(
								translation->second->translation + immDwordIt->second) =
									data;

							++translation;
							continue;
						}
					}

					//Mark the translation as needing freeing
					translationsNeedingFreeing.push_back(translation->second->translation);

					//Remove this translation from the eip to translation cache map and TLBs
					eipToTranslationCacheMap.erase(translation->first);

					EipToTranslationCacheTLBEntry & tlbEntry =
						eipToTranslationCacheTLB[
							translation->first.Hash() & eipToTranslationCacheTLBMask];

					if(tlbEntry.key == translation->first)
						tlbEntry.key.flags = TRANSLATION_EMPTY_FLAG;

					EipToTranslationTLBEntry & translationTLBEntry =
						registers->eipToTranslationTLB[
							translation->first.physicalEip & EIP_TO_TRANSLATION_TLB_MASK];

					if(	translationTLBEntry.physicalEip == translation->first.physicalEip &&
						translationTLBEntry.flags == translation->first.flags)
					{
						translationTLBEntry.flags = EIP_TO_TRANSLATION_TLB_EMPTY_FLAG;
					}

					//Remove this translation from the page translation list
					translation = pageTranslationLists[page].erase(translation);
				}
				else
					++translation;
			}

			//If no translations on this page exist anymore, classify the page as such
			if(pageTranslationLists[page].empty())
				registers->pageClassifications[page] = PAGE_CLASS_NORMAL;
		}
		else
		{
			//Is this simply a patching of an immediate dword?
			//Do we need to invalidate the translations on this page?
			bool immediateDword = false;
			bool invalidateTranslations = false;
			
			for(PageTranslationListType::iterator translation = pageTranslationLists[page].begin();
				translation != pageTranslationLists[page].end(); ++translation)
			{
				if(	translation->first.physicalEip < endAddress &&
					translation->second->endPhysicalEip >= startAddress)
				{
					if(endAddress - startAddress == 4)
					{
						auto immDwordIt =
							translation->second->immDwordOffsets.find(
								startAddress - translation->first.physicalEip);

						if(immDwordIt != translation->second->immDwordOffsets.end())
						{
							immediateDword = true;
							continue;
						}
					}

					invalidateTranslations = true;
				}
			}

			//If this is simply a patching of an immediate dword, perform the patching
			if(immediateDword && !invalidateTranslations)
			{
				for(PageTranslationListType::iterator translation = pageTranslationLists[page].begin();
					translation != pageTranslationLists[page].end(); ++translation)
				{
					if(	translation->first.physicalEip < endAddress &&
						translation->second->endPhysicalEip >= startAddress)
					{
						auto immDwordIt =
							translation->second->immDwordOffsets.find(
								startAddress - translation->first.physicalEip);

						if(immDwordIt != translation->second->immDwordOffsets.end())
						{
							*reinterpret_cast<Dword *>(
								translation->second->translation + immDwordIt->second) =
									data;
						}
					}
				}
			}

			//If we do not need to invalidate the translations in this page, move on to the next
			if(!invalidateTranslations)
				continue;

			//Loop through translations on the given page
			for(PageTranslationListType::iterator translation = pageTranslationLists[page].begin();
				translation != pageTranslationLists[page].end(); ++translation)
			{
				//Mark the translation as needing freeing
				translationsNeedingFreeing.push_back(translation->second->translation);

				//Remove this translation from the eip to translation cache map and TLBs
				eipToTranslationCacheMap.erase(translation->first);

				EipToTranslationCacheTLBEntry & tlbEntry =
					eipToTranslationCacheTLB[
						translation->first.Hash() & eipToTranslationCacheTLBMask];

				if(tlbEntry.key == translation->first)
					tlbEntry.key.flags = TRANSLATION_EMPTY_FLAG;

				EipToTranslationTLBEntry & translationTLBEntry =
					registers->eipToTranslationTLB[
						translation->first.physicalEip & EIP_TO_TRANSLATION_TLB_MASK];

				if(	translationTLBEntry.physicalEip == translation->first.physicalEip &&
					translationTLBEntry.flags == translation->first.flags)
				{
					translationTLBEntry.flags = EIP_TO_TRANSLATION_TLB_EMPTY_FLAG;
				}
			}

			//No translations on this page exist anymore
			pageTranslationLists[page].resize(0);
			registers->pageClassifications[page] = PAGE_CLASS_NORMAL;
			ClearWriteTLBsFlag(page, TLB_ENTRY_CONTAINS_TRANSLATIONS_FLAG);

			//If the page is marked as part page invalidation pending,
			//mark it as part page invalidation
			if(partPageInvalidationPendings[page])
			{
				partPageInvalidations[page] = true;
				partPageInvalidationPendings[page] = false;
			}
		}

		++pageInvalidationCounts[page];
	}
}
