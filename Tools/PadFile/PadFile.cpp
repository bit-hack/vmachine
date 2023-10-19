//	PadFile
//	Pad a file with zeros to a given length
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include <fstream>
#include <iterator>
#include <iostream>
#include <boost\lexical_cast.hpp>

int main(int argc, char ** argv)
{
	//Ensure the correct number of arguments were given
	if(argc != 3)
	{
		std::cout << "Usage: PadFile filename size" << std::endl;
		return 1;
	}

	//Open the file
	std::ofstream fileStream(argv[1], std::ios::out | std::ios::app | std::ios::binary);

	//Get the file size
	fileStream.seekp(0, std::ios_base::end);
	std::ofstream::pos_type fileSize = fileStream.tellp();

	//Get the size to pad to
	std::size_t padSize = boost::lexical_cast<std::size_t>(argv[2]);

	//If the file is already larger than this, error
	if(fileSize > padSize)
	{
		std::cout << argv[1] << " is already larger than " << padSize << " bytes" << std::endl;
		return 1;
	}

	//Pad the file
	std::ostream_iterator<char> fileIt(fileStream);
	std::fill_n(fileIt, padSize - fileSize, 0);

	return 0;
}