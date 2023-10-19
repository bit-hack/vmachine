//	VMachine
//	Paint the window
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

namespace
{
	struct ScreenRectVertex
	{
		float x, y, z, rhw;
		Dword color;
		float s, t;

		static const Dword fvf = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;
	};
}

void Window::Paint(void)
{
	assert(screenTextureHScale != 0);
	assert(screenTextureVScale != 0);
	assert(d3dDevice);
	
	//Fill in the vertex data for the screen rectangle
	ScreenRectVertex vertexData[4] =
	{
		-0.5f,			-0.5f,			0.5f,	1.0f,	0x00ffffff,
		0.0f,
		0.0f,
		width -0.5f,	-0.5f,			0.5f,	1.0f,	0x00ffffff,
		static_cast<float>(framebufferWidth) / (screenTextureWidth * screenTextureHScale),
		0.0f,
		-0.5f,			height -0.5f,	0.5f,	1.0f,	0x00ffffff,
		0.0f,
		static_cast<float>(framebufferHeight) / (screenTextureHeight * screenTextureVScale),
		width -0.5f,	height -0.5f,	0.5f,	1.0f,	0x00ffffff,
		static_cast<float>(framebufferWidth) / (screenTextureWidth * screenTextureHScale),
		static_cast<float>(framebufferHeight) / (screenTextureHeight * screenTextureVScale)
	};

	//Begin scene
	if(SUCCEEDED(d3dDevice->BeginScene()))
	{
		//Draw the screen rectangle
		d3dDevice->SetFVF(ScreenRectVertex::fvf);
		d3dDevice->SetTexture(0, screenTexture);

		d3dDevice->DrawPrimitiveUP(	D3DPT_TRIANGLESTRIP, 2, vertexData,
									sizeof(ScreenRectVertex));

		//End scene
		d3dDevice->EndScene();
	}

	//Present the scene
	HRESULT hr = d3dDevice->Present(0, 0, 0, 0);

	//If the device was lost but is now ready to be reset, do so
	if(hr == D3DERR_DEVICELOST)
	{
		hr = d3dDevice->TestCooperativeLevel();

		if(hr == D3DERR_DEVICENOTRESET)
			ResetDirect3DDevice();
	}

	//Validate the window's contents
	ValidateRect(hWnd, 0);
}
