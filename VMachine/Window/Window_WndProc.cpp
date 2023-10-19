//	VMachine
//	Window procedure. Calls the non-static MsgProc
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

//Disable the warnings regarding {Get|Set}WindowLongPtr and 64-bit portability
#pragma warning(disable: 4312; disable: 4244)

LRESULT CALLBACK Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//Get the pointer to the Window object from the user data
	Window * window = reinterpret_cast<Window *>(GetWindowLongPtr(hWnd, GWL_USERDATA));

	//If the pointer has not been set yet
	if(!window)
	{
		//If this is a WM_NCCREATE message
		if(msg == WM_NCCREATE)
		{
			//Get the pointer from lParam and save it as the user data
			window = reinterpret_cast<Window *>(
						reinterpret_cast<LPCREATESTRUCT>(lParam)->lpCreateParams);

			SetWindowLongPtr(hWnd, GWL_USERDATA, reinterpret_cast<LONG_PTR>(window));

			//Set the hWnd member of the window object
			window->SetHWnd(hWnd);
		}
		else
		{
			//Otherwise, await the WM_NCCREATE message
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}

	//Call the non-static MsgProc
	return window->MsgProc(msg, wParam, lParam);
}
