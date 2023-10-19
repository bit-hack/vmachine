//	VMachine
//	Reset the Direct3D device
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::ResetDirect3DDevice(void)
{
		//Create and fill in a present parameters structure
		D3DPRESENT_PARAMETERS presentParams = {0};

		presentParams.Windowed				= true;
		presentParams.SwapEffect			= D3DSWAPEFFECT_DISCARD;
		presentParams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
		presentParams.BackBufferFormat		= D3DFMT_UNKNOWN;

		//Release the screen texture
		assert(screenTexture);
		screenTexture->Release();
		screenTexture = 0;

		//Reset the Direct3D device object
		if(FAILED(d3dDevice->Reset(&presentParams)))
			throw Ex("Window Error: d3dDevice->Reset failed");

		//Recreate the screen texture
		CreateScreenTexture();
}
