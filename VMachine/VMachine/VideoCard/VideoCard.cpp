//	VMachine
//	Video card constructor/destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "VideoCard.h"

//Constructor
VideoCard::VideoCard() :	regenerateFramebuffer(true),
							framebufferCache(1024 * 1024),
							videoMemory(4 * videoMemoryPlaneSize),
							videoMemoryDirty(4 * videoMemoryPlaneSize),
							width(720), height(400),
							graphicsIndexRegister(0), graphicsMode(false),
							colorRegisterIndex(0), colorRegisterComponent(0),
							colorRegisterWriteMode(false)
{
	
}

//Destructor
VideoCard::~VideoCard()
{
	
}
