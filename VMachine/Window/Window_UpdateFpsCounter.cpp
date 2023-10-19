//	VMachine
//	Update the fps counter
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

#ifdef FPS_COUNTER

void Window::UpdateFpsCounter(Dword instrPerMs)
{
	++numFrames;
	instrPerMsSum += instrPerMs;

	Qword currentTime = timer.GetTime();

	if(currentTime - startTime > 1000000)
	{
		double fps = static_cast<double>(1000000 * numFrames) / (currentTime - startTime);
		
		std::stringstream titleString;
		titleString << "VMachine - Fps: ";
		titleString << std::fixed << std::setprecision(2) << fps;
		titleString << " - InstrPerMs: " << instrPerMsSum / numFrames;

        SetWindowText(hWnd, titleString.str().c_str());

		startTime = currentTime;
		numFrames = 0;
		instrPerMsSum = 0;
	}
}

#endif
