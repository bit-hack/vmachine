//	VDisk
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include <iostream>
#include <string>
#include <boost\lexical_cast.hpp>
#include <windows.h>

static char GetType(void)
{
	std::string typeStr;

	do
	{
		std::cout << "Would you like to create a floppy or hard disk image (F or H)? ";
		
		getline(std::cin, typeStr);
	}
	while(typeStr != "F" && typeStr != "f" && typeStr != "H" && typeStr != "h");

	return toupper(typeStr[0]);
}

static std::string GetFilename(char type)
{
	std::string defaultName = (type == 'F') ? "a.img" : "c.img";

	std::cout << "Please enter the filename to use (default: " << defaultName << "): ";

	std::string filename;
	std::getline(std::cin, filename);

	if(filename == "")
		filename = defaultName;

	return filename;
}

static int GetHDImageSize(void)
{
	int size = 0;

	do
	{
		std::cout << "Please enter the image size required in MB (20 - 500): ";

		std::string sizeStr;
		std::getline(std::cin, sizeStr);

		try
		{
			size = boost::lexical_cast<int>(sizeStr);
		}
		catch(boost::bad_lexical_cast &)
		{
			size = 0;
		}
	}
	while(size < 20 || size > 500);

	return size;
}

static bool RequestReplace(const std::string & filename)
{
	std::string ynStr;

	do
	{
		std::cout << filename << " already exists." << std::endl;
		std::cout << "Would you like to replace it (Y or N)? ";
		
		getline(std::cin, ynStr);
	}
	while(ynStr != "Y" && ynStr != "y" && ynStr != "N" && ynStr != "n");

	return toupper(ynStr[0]) == 'Y';
}

int main()
{
	std::cout << "VDisk - Disk Image Creator" << std::endl << std::endl;
	
	char type = GetType();
	
	int numSectors = 0;

	if(type == 'F')
	{
		numSectors = 2880;
	}
	else if(type == 'H')
	{
		int numBytes = GetHDImageSize() * 1024 * 1024;
		int numCylinders = numBytes / (512 * 63 * 16);
        numSectors = numCylinders * 63 * 16;		
	}

	std::string filename = GetFilename(type);
	
	HANDLE imageFile;
	bool retry = false;

	do
	{
		retry = false;

		imageFile = CreateFile(	filename.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0,
								CREATE_NEW, 0, 0);

		if(imageFile == INVALID_HANDLE_VALUE)
		{
			if(GetLastError() == ERROR_FILE_EXISTS)
			{
				if(RequestReplace(filename) == true)
				{
					DeleteFile(filename.c_str());
					retry = true;
				}
				else
				{
					filename = GetFilename(type);
					retry = true;
				}
			}
			else
			{
				std::cout << "Unable to create " << filename << std::endl;
				return 1;
			}
		}
	}
	while(retry == true);
    
	if(SetFilePointer(imageFile, numSectors * 0x200, 0, FILE_BEGIN) ==
		INVALID_SET_FILE_POINTER)
	{
		std::cout << "Unable to create " << filename << std::endl;

		CloseHandle(imageFile);
		return 1;
	}

	if(!SetEndOfFile(imageFile))
	{
		std::cout << "Unable to create " << filename << std::endl;

		CloseHandle(imageFile);
		return 1;
	}

	CloseHandle(imageFile);

	std::cout << filename << " successfully created!" << std::endl;

	return 0;
}
