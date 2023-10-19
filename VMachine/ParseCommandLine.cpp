//	VMachine
//	Parse the command line for the disk image filenames
//
//	Copyright (c) 2006, Paul Baker
//	Distributed under the New BSD Licence. (See accompanying file License.txt or copy at
//	http://www.paulsprojects.net/NewBSDLicense.txt)

#include "Root.h"

static std::vector <std::string> SplitString(std::string str, std::string separators)
{
	std::vector <std::string> words;

	std::string::size_type i, j;
	for(i = 0;
		(j = str.find_first_of(separators, i)) != std::string::npos;
		i = j + 1)
	{
		words.push_back(str.substr(i, j - i));
	}

	if(i < str.length())
		words.push_back(str.substr(i));

	return words;
}

void ParseCommandLine(	const std::string & commandLine,
						Dword & memorySize,
						std::string & fdimgFilename,
						std::string & hdimgFilename)
{
	//Split the command line into words
	std::vector <std::string> commandWords = SplitString(commandLine, " \t");

	//Loop through the words
	for(std::vector<std::string>::const_iterator iWord = commandWords.begin();
		iWord != commandWords.end(); ++iWord)
	{
		//If this word is a recognised argument, get the filename from the next word
		if((iWord + 1) != commandWords.end())
		{
			try
			{
				if(*iWord == "-mem")
					memorySize = boost::lexical_cast<Dword>(*(iWord + 1));
			}
			catch(boost::bad_lexical_cast &)
			{
				memorySize = 0;
			}

            if(*iWord == "-fdimg")
				fdimgFilename = *(iWord + 1);

			if(*iWord == "-hdimg")
				hdimgFilename = *(iWord + 1);
		}
	}
}
