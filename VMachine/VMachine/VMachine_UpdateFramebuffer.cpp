//	VMachine
//	Update the framebuffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VMachine.h"
#include "VideoCard/VideoCard.h"

void VMachine::UpdateFramebuffer(Dword frameNumber, FramebufferData & framebuffer)
{
	videoCard->UpdateFramebuffer(frameNumber, framebuffer);
}
