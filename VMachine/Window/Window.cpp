//	VMachine
//	Window Constructor/Destructor
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"
#include "../Resources/resource.h"

//Constructor
Window::Window(const boost::shared_ptr<VMachine> & vmachine_, Dword width_, Dword height_)
	:	vmachine(vmachine_), width(width_), height(height_),
		style(WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX), exStyle(0),
		hWnd(0), d3d(0), d3dDevice(0),
		screenTextureWidth(0), screenTextureHeight(0),
		screenTextureHScale(1), screenTextureVScale(1), screenTexture(0),
		directSound(0), soundBuffer(0), soundBufferStarted(false),
		writeCursor(0), lastLockPlayCursor(0),
		framebufferWidth(width), framebufferHeight(height),
		mouseGrabbed(false), mouseJustGrabbed(false),
		controlPressed(false), altPressed(false),
		lButtonDown(false), mButtonDown(false), rButtonDown(false)
#ifdef FPS_COUNTER
		, startTime(0), numFrames(0), instrPerMsSum(0)
#endif
{
	//Create and fill in a window class structure
	WNDCLASSEX windowClass = {0};

	windowClass.cbSize			= sizeof(WNDCLASSEX);
	windowClass.style			= CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc		= WndProc;
	windowClass.hInstance		= GetModuleHandle(0);
	windowClass.lpszClassName	= "VMachine";
	windowClass.hCursor			= LoadCursor(0, IDC_ARROW);
	windowClass.lpszMenuName	= MAKEINTRESOURCE(IDR_MAINMENU);

	//Register the window class
	if(!RegisterClassEx(&windowClass))
		throw Ex("Window Error: RegisterClassEx failed");

	//Calculate the window size necessary for the requested client area size
	RECT windowRect = {0, 0, width, height};

	AdjustWindowRectEx(&windowRect, style, true, exStyle);

	//Create the window, passing the "this" pointer
	hWnd = CreateWindowEx(	exStyle, "VMachine", "VMachine", style, 0, 0,
							windowRect.right - windowRect.left,					
							windowRect.bottom - windowRect.top,
							0, 0, GetModuleHandle(0), this);

	if(!hWnd)
		throw Ex("Window Error: CreateWindowEx failed");

	//Create the Direct3D objects
	CreateDirect3DObjects();

	//Create the screen texture
	CreateScreenTexture();

	//Create the DirectSound object
	CreateDirectSoundObject();

	//Create the sound buffer
	CreateSoundBuffer();

	//Show the window
	ShowWindow(hWnd, SW_SHOW);
}

//Destructor
Window::~Window(void)
{
	//Release the mouse
	if(mouseGrabbed)
	{
		mouseGrabbed = false;
		ReleaseCapture();
		SetCursorPos(grabPosition.x, grabPosition.y);
		ShowCursor(true);
	}

	//Release the sound buffer
	assert(soundBuffer);
	soundBuffer->Release();

	//Release the DirectSound object
	assert(directSound);
	directSound->Release();

	//Release the screen texture if not already released
	if(screenTexture)
		screenTexture->Release();

	//Release the Direct3D device object
	assert(d3dDevice);
	d3dDevice->Release();

	//Release the Direct3D object
	assert(d3d);
	d3d->Release();

	//Destroy the window
	assert(hWnd);
	DestroyWindow(hWnd);

	//Unregister the window class
	UnregisterClass("VMachine", GetModuleHandle(0));
}
