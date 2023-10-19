//	VMachine
//	Main source file, containing application entry point
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "ParseCommandLine.h"
#include "Window/Window.h"
#include "VMachine/VMachine.h"

//Debug output stream
#ifdef DEBUG_OUTPUT
std::ofstream dbgOut;
#endif

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
	try
	{
		//Open the debug output stream and set hex mode
#ifdef DEBUG_OUTPUT
		dbgOut.open("DebugOut.txt");
		dbgOut << std::hex;
#endif
		//Parse the command line for the memory size and disk image filenames
		Dword memorySize = 0;
		std::string fdimgFilename = "", hdimgFilename = "";
		ParseCommandLine(lpCmdLine, memorySize, fdimgFilename, hdimgFilename);

		if(memorySize < 4 || memorySize > 16)
			throw std::exception("VMachine memory size (-mem argument) must be between 4MB and 16MB.");

		if(hdimgFilename == "")
			throw std::exception("Hard drive image (-hdimg argument) not specified.");

		//Create the VMachine
		boost::shared_ptr<VMachine> vmachine(
			new VMachine(memorySize * 1024 * 1024,
				fdimgFilename, hdimgFilename));

		//Create a window
		boost::shared_ptr<Window> window(new Window(vmachine, 720, 400));

		//Message loop
		MSG msg;
		Dword frameNumber = 0, msNumber = 0;
		Dword instrPerMs = 10000;
		Qword lastFrameEndTime = 0;

		for(;;)
		{
			if(PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
			{
				if(msg.message == WM_QUIT)
					break;

				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				//Advance the emulation by one frame
				++frameNumber;
				
				for(Dword ms = 0; ms < 25; ++ms)
				{
					//Advance the emulation by one ms
					vmachine->AdvanceOneMs(++msNumber, instrPerMs);

					//Update the sound buffer
					SoundBufferData soundBufferData = window->LockSoundBuffer(frameNumber);
					vmachine->UpdateSoundBuffer(frameNumber, soundBufferData);
					window->UnlockSoundBuffer(soundBufferData);
				}

				//Change the window size, if necessary
				std::pair <Dword, Dword> fbSize = vmachine->GetFramebufferSize();
				window->Resize(fbSize.first, fbSize.second);

				//Update the frame buffer
				FramebufferData fbData = window->LockBackBuffer();
				vmachine->UpdateFramebuffer(frameNumber, fbData);
				window->UnlockBackBuffer(fbData);

				//Update the fps counter
#ifdef FPS_COUNTER
				window->UpdateFpsCounter(instrPerMs);
#endif

				//Scale instrPerMs by the difference between the frame time and
				//the desired frame time.
				if(frameNumber != 1)
				{
					Qword frameTime = timer.GetTime() - lastFrameEndTime;

					instrPerMs = static_cast<Dword>((instrPerMs * 23500.0) / frameTime);

					if(instrPerMs < 1000)
						instrPerMs = 1000;
				}

				//Limit the FPS to 42.5
				while(timer.GetTime() - lastFrameEndTime < 23500)
					Sleep(0);

				lastFrameEndTime = timer.GetTime();
			}
		}

		return static_cast<int>(msg.wParam);
	}
	catch(std::exception & ex)
	{
		//Remove the WM_QUIT message from the message queue
		MSG msg;
		PeekMessage(&msg, 0, WM_QUIT, WM_QUIT, PM_REMOVE);

		MessageBox(0, ex.what(), "Fatal Error", MB_OK);
	}

	return 0;
}
