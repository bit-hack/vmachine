//	VMachine
//	Structure holding data about a framebuffer
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

struct FramebufferData
{
	FramebufferData(const Dword width_, const Dword height_, const Dword pitch_,
					Dword * const data_)
		:	width(width_), height(height_), pitch(pitch_), data(data_)
	{}

	const Dword width, height, pitch;
	Dword hScale, vScale;
	Dword * const data;
};
