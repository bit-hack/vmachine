//	VMachine
//	Get the required framebuffer size
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "VideoCard/VideoCard.h"

std::pair <Dword, Dword> VMachine::GetFramebufferSize(void)
{
	return videoCard->GetFramebufferSize();
}