//	VMachine
//	Lock/unlock the back buffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

FramebufferData Window::LockBackBuffer(void)
{
	D3DLOCKED_RECT lockedRect;

	if(FAILED(screenTexture->LockRect(0, &lockedRect, 0, D3DLOCK_DISCARD)))
		throw Ex("Window Error: screenTexture->LockRect failed");

	return FramebufferData(	framebufferWidth, framebufferHeight, lockedRect.Pitch / 4,
							reinterpret_cast<Dword *>(lockedRect.pBits));
}

void Window::UnlockBackBuffer(const FramebufferData & framebuffer)
{
	screenTexture->UnlockRect(0);

	//Save the scaling to apply to the screen texture
	screenTextureHScale = framebuffer.hScale;
	screenTextureVScale = framebuffer.vScale;

	//Invalidate the window's contents
	InvalidateRect(hWnd, 0, false);
}
