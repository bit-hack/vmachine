//	VMachine
//	Window class
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

class VMachine;

class Window
{
public:

	//Constructor/Destructor
	Window(const std::shared_ptr<VMachine> & vmachine_, Dword width_, Dword height_);
	~Window(void);
	
	//Copy constructor/Copy assignment operator handled by boost::noncopyable

	//Resize the window
	void Resize(Dword newFramebufferWidth, Dword newFramebufferHeight);

	//Lock/unlock the back buffer
	FramebufferData LockBackBuffer(void);
	void UnlockBackBuffer(const FramebufferData & framebuffer);

	//Lock/unlock the sound buffer
	SoundBufferData LockSoundBuffer(Dword frameNumber);
	void UnlockSoundBuffer(const SoundBufferData & soundBufferData);

	//Update the fps counter
#ifdef FPS_COUNTER
	void UpdateFpsCounter(Dword instrPerMs);
#endif

private:

	//VMachine associated with this window
	std::shared_ptr<VMachine> vmachine;

	//Window procedure. Calls the non-static MsgProc
	static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	//Set the hWnd member. Called by WndProc on receipt of WM_NCCREATE
	void SetHWnd(HWND hWnd_)
	{ hWnd = hWnd_; }

	//Message handling procedure
	void PauseTimerAndSound(void);
	void UnpauseTimerAndSound(void);
	
	LRESULT MsgProc(UINT msg, WPARAM wParam, LPARAM lParam);

	//Client area size
	Dword width, height;

	//Window style
	Dword style, exStyle;

	//Window handle
	HWND hWnd;

	//Create the Direct3D objects
	void CreateDirect3DObjects(void);

	//Direct3D object
	IDirect3D9 * d3d;

	//Reset the Direct3D device
	void ResetDirect3DDevice(void);

	//Direct3D device object
	IDirect3DDevice9 * d3dDevice;

	//Create the screen texture
	void CreateScreenTexture(void);

	//Screen texture
	Dword screenTextureWidth, screenTextureHeight;
	Dword screenTextureHScale, screenTextureVScale;
	IDirect3DTexture9 * screenTexture;

	//Create the DirectSound object
	void CreateDirectSoundObject(void);

	//DirectSound object
	IDirectSound8 * directSound;

	//Create the sound buffer
	void CreateSoundBuffer(void);

	//Sound buffer
	IDirectSoundBuffer8 * soundBuffer;
	bool soundBufferStarted;
	Dword writeCursor;
	Dword lastLockPlayCursor;

	//Sound buffer locked region
	DWORD lockNumBytes1, lockNumBytes2;
	void * lockPtr1;
	void * lockPtr2;

	//Paint the window
	void Paint(void);

	//Framebuffer size
	Dword framebufferWidth, framebufferHeight;

	//Mouse data
	bool mouseGrabbed, mouseJustGrabbed;
	POINT grabPosition;
	bool controlPressed, altPressed;
	bool lButtonDown, mButtonDown, rButtonDown;

	//Frames per second data
#ifdef FPS_COUNTER
	Qword startTime;
	Dword numFrames;
	Dword instrPerMsSum;
#endif

public:

	//Exception thrown by Window functions
	class Ex : public std::exception
	{
	public:
		Ex(const std::string & what) : std::exception(what.c_str())
		{}
	};
};
