//	VMachine
//	Parse the command line for the disk image filenames
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#pragma once

void ParseCommandLine(	const std::string & commandLine,
						Dword & memorySize,
						std::string & fdimgFilename,
						std::string & hdimgFilename);
