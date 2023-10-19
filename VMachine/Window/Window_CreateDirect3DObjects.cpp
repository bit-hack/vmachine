//	VMachine
//	Create the Direct3D objects
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"
#include "Window.h"

void Window::CreateDirect3DObjects(void)
{
	//Create the Direct3D object
	d3d = Direct3DCreate9(D3D_SDK_VERSION);

	if(!d3d)
		throw Ex("Window Error: Direct3DCreate9 failed");

	//Create and fill in a present parameters structure
	D3DPRESENT_PARAMETERS presentParams = {0};

	presentParams.Windowed				= true;
	presentParams.SwapEffect			= D3DSWAPEFFECT_DISCARD;
	presentParams.PresentationInterval	= D3DPRESENT_INTERVAL_IMMEDIATE;
	presentParams.BackBufferFormat		= D3DFMT_UNKNOWN;

	//Create the Direct3D device object
	if(FAILED(d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
								D3DCREATE_SOFTWARE_VERTEXPROCESSING,
								&presentParams, &d3dDevice)))
	{
		throw Ex("Window Error: d3d->CreateDevice failed");
	}
}
