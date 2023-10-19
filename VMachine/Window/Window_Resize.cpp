//	VMachine
//	Resize the window
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::Resize(Dword newFramebufferWidth, Dword newFramebufferHeight)
{
	if(newFramebufferWidth != framebufferWidth || newFramebufferHeight != framebufferHeight)
	{
		//Set the new framebuffer size
		framebufferWidth = newFramebufferWidth;
		framebufferHeight = newFramebufferHeight;

		//Calculate the new client area size
		width = framebufferWidth;
		height = framebufferHeight;

		if(width < 400)
			width *= 2;

		if(height < 300)
			height *= 2;

		//Calculate the window size necessary for the requested client area size
		RECT windowRect = {0, 0, width, height};

		AdjustWindowRectEx(&windowRect, style, true, exStyle);

		//Resize the window
		RECT oldWindowRect;
		GetWindowRect(hWnd, &oldWindowRect);

		MoveWindow(	hWnd, oldWindowRect.left, oldWindowRect.top,
					windowRect.right - windowRect.left,
					windowRect.bottom - windowRect.top,
					true);

		//Reset the Direct3D device object, hence resizing the screen texture
		ResetDirect3DDevice();
	}
}
