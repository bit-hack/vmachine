//	VMachine
//	Message handling procedure
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"
#include "../VMachine/VMachine.h"
#include "../VMachine/FloppyController/FloppyController.h"
#include "../Resources/resource.h"

void Window::PauseTimerAndSound(void)
{
	timer.Pause();

	if(soundBufferStarted)
		soundBuffer->Stop();
}

void Window::UnpauseTimerAndSound(void)
{
	timer.Unpause();

	if(soundBufferStarted)
		soundBuffer->Play(0, 0, DSBPLAY_LOOPING);
}

LRESULT Window::MsgProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
	case WM_PAINT:
		Paint();
		return 0;

	case WM_CLOSE:
		DestroyWindow(hWnd);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_SYSCOMMAND:
		{
			HMENU hMenu = GetMenu(hWnd);
			EnableMenuItem(	hMenu, ID_FLOPPY_EJECT,
				vmachine->IsFloppyDiskInserted() ? MF_ENABLED : MF_GRAYED);
		}
		break;

	case WM_ENTERSIZEMOVE:
	case WM_ENTERMENULOOP:
		PauseTimerAndSound();
		break;

	case WM_EXITSIZEMOVE:
	case WM_EXITMENULOOP:
		UnpauseTimerAndSound();
		break;

	case WM_COMMAND:
		{
			switch(LOWORD(wParam))
			{
			case ID_FILE_EXIT:
				PostMessage(hWnd, WM_CLOSE, 0, 0);
				return 0;

			case ID_FLOPPY_INSERT:
				{
					OPENFILENAME ofn = {0};
					TCHAR filename[MAX_PATH] = {0};
					
					ofn.lStructSize	= sizeof(OPENFILENAME);
					ofn.hwndOwner	= hWnd;
					ofn.lpstrFilter	= 
						"Image Files (*.ima, *.img, *.dsk)\0*.IMA;*.IMG;*.DSK\0"
						"All Files (*.*)\0*.*\0\0";
					ofn.lpstrFile	= filename;
					ofn.nMaxFile	= MAX_PATH;
					ofn.Flags		= OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

					PauseTimerAndSound();

					if(GetOpenFileName(&ofn) != 0)
					{
						try
						{
							vmachine->InsertFloppyDisk(filename);
						}
						catch(FloppyController::ImageIncorrectSizeEx & ex)
						{
							MessageBox(hWnd, ex.what(), "Floppy Disk Error", MB_OK);
						}
					}

					UnpauseTimerAndSound();
				}
				return 0;

			case ID_FLOPPY_EJECT:
				vmachine->EjectFloppyDisk();
				return 0;

			case ID_HELP_ABOUTVMACHINE:
				PauseTimerAndSound();
				MessageBox(hWnd,
					"VMachine " VERSION_NUMBER_STRING "\n\n"
					"Copyright (c) " COPYRIGHT_YEAR_STRING ", Paul Baker\n\n"
					"Downloaded from: www.paulsprojects.net",
					"VMachine",
					MB_OK);
				UnpauseTimerAndSound();
			}
		}
        break;

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:

		if(wParam == VK_CONTROL)
			controlPressed = true;

		if(wParam == VK_MENU)
			altPressed = true;

		//If Ctrl-Alt (or Alt-Ctrl) is pressed, release the mouse
		if(mouseGrabbed && controlPressed && altPressed)
		{
            mouseGrabbed = false;
			ReleaseCapture();
			SetCursorPos(grabPosition.x, grabPosition.y);
			ShowCursor(true);
		}

		vmachine->SetKeyState(static_cast<Byte>((lParam & 0x00ff0000) >> 16), true);
		return 0;

	case WM_KEYUP:
	case WM_SYSKEYUP:

		if(wParam == VK_CONTROL)
			controlPressed = false;

		if(wParam == VK_MENU)
			altPressed = false;

		vmachine->SetKeyState(static_cast<Byte>((lParam & 0x00ff0000) >> 16), false);
		return 0;

	case WM_LBUTTONDOWN:
		lButtonDown = true;

		//If the mouse was not grabbed, grab it
		if(!mouseGrabbed)
		{
			mouseGrabbed = true;
			mouseJustGrabbed = true;
			SetCapture(hWnd);
			GetCursorPos(&grabPosition);
			ShowCursor(false);

			//Move the cursor to the centre of the window
			POINT centre = {width/2, height/2};
			ClientToScreen(hWnd, &centre);
			SetCursorPos(centre.x, centre.y);
		}
		else
		{
			//If the mouse was already grabbed, report the button state to the VMachine
			vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		}

		return 0;

	case WM_LBUTTONUP:
		lButtonDown = false;

		//If the mouse is not grabbed and no buttons are pressed, uncapture the mouse
		if(!mouseGrabbed && !mButtonDown && !rButtonDown)
			ReleaseCapture();

		if(mouseGrabbed)
		{
			if(mouseJustGrabbed)
				mouseJustGrabbed = false;
			else
				vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		}
		
		return 0;

	case WM_MBUTTONDOWN:
		mButtonDown = true;

		//If the mouse is not grabbed and no other buttons are pressed,
		//capture it temporarily so we receive the correcponging "button up" message
		if(!mouseGrabbed && !lButtonDown && !rButtonDown)
			SetCapture(hWnd);

		if(mouseGrabbed)
			vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		
		return 0;

	case WM_MBUTTONUP:
		mButtonDown = false;
		if(!mouseGrabbed && !lButtonDown && !rButtonDown)
			ReleaseCapture();
		if(mouseGrabbed)
			vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		return 0;

	case WM_RBUTTONDOWN:
		rButtonDown = true;
		if(!mouseGrabbed && !lButtonDown && !mButtonDown)
			SetCapture(hWnd);
		if(mouseGrabbed)
			vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		return 0;

	case WM_RBUTTONUP:
		rButtonDown = false;
		if(!mouseGrabbed && !lButtonDown && !mButtonDown)
			ReleaseCapture();
		if(mouseGrabbed)
			vmachine->SetMouseState(0, 0, lButtonDown, mButtonDown, rButtonDown);
		return 0;

	case WM_MOUSEMOVE:
		//If the mouse is grabbed
		if(mouseGrabbed)
		{
			//Calculate how far the mouse has moved
			SignedWord dx = static_cast<SignedWord>(LOWORD(lParam) - width/2);
			SignedWord dy = static_cast<SignedWord>(HIWORD(lParam) - height/2);

			//If the mouse has moved
			if(dx != 0 || dy != 0)
			{
				//Move the cursor to the centre of the window
				POINT centre = {width/2, height/2};
				ClientToScreen(hWnd, &centre);
				SetCursorPos(centre.x, centre.y);

				//Report the movement to the VMachine
				vmachine->SetMouseState(dx, dy, lButtonDown, mButtonDown, rButtonDown);
			}
		}
		return 0;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}
