//	VMachine
//	Create the screen texture
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::CreateScreenTexture(void)
{
	//Calculate the size of the screen texture
	screenTextureWidth = RoundUpToPowerOf2(framebufferWidth);
	screenTextureHeight = RoundUpToPowerOf2(framebufferHeight);

	//Create the screen texture
	if(FAILED(d3dDevice->CreateTexture(	screenTextureWidth, screenTextureHeight, 1,
										D3DUSAGE_DYNAMIC, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT,
										&screenTexture, 0)))
	{
		throw Ex("Window Error: d3dDevice->CreateTexture failed");
	}

	//Fill the screen texture with black
	D3DLOCKED_RECT lockedRect;

	if(FAILED(screenTexture->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD)))
		throw Ex("Window Error: screenTexture->LockRect failed");

	for(Dword i = 0; i < screenTextureHeight; ++i)
		memset(	static_cast<Byte *>(lockedRect.pBits) + i * lockedRect.Pitch, 0,
				screenTextureWidth * 4);

	screenTexture->UnlockRect(0);
}
