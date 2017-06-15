#pragma once

#include <string>
#include <vector>
#include <stdio.h>
#include <assert.h>
#include <Windows.h>

namespace Utility
{
	class String
	{
	public:
		//http://www.joinc.co.kr/modules/moniwiki/wiki.php/Site/C++/Documents/C++ProgramingHowToPerPage/standardstring.html
		static void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ")
		{
			// 맨 첫 글자가 구분자인 경우 무시
			std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
			// 구분자가 아닌 첫 글자를 찾는다
			std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

			while (std::string::npos != pos || std::string::npos != lastPos)
			{
				// token을 찾았으니 vector에 추가한다
				tokens.push_back(str.substr(lastPos, pos - lastPos));
				// 구분자를 뛰어넘는다.  "not_of"에 주의하라
				lastPos = str.find_first_not_of(delimiters, pos);
				// 다음 구분자가 아닌 글자를 찾는다
				pos = str.find_first_of(delimiters, lastPos);
			}
		}

		static bool ParseDirectory(const std::string& path, std::string& outFolderPath, std::string& outFileName, std::string& outFileFormat)
		{
			return ParseDirectory(path, &outFolderPath, &outFileName, &outFileFormat);
		}

		static bool ParseDirectory(const std::string& path, std::string* outFolderPath, std::string* outFileName, std::string* outFileFormat)
		{
			auto ParseNameAndFormat = [&](const std::string& fileNameWithFormat)
			{
				unsigned int formatPos = fileNameWithFormat.find('.');
				if( formatPos == -1 )
					return false;

				if(outFileFormat)
					*outFileFormat = &fileNameWithFormat.c_str()[formatPos+1];

				if(outFileName)
					*outFileName = fileNameWithFormat.substr(0, formatPos); 

				return true;
			};

			unsigned int fileNameStartPos = path.rfind('/');
			if( fileNameStartPos == -1 )
			{
				fileNameStartPos = path.rfind('\\');
				if(fileNameStartPos == -1)
					return ParseNameAndFormat(path);
			}

			if(outFolderPath)
				*outFolderPath = path.substr(0, fileNameStartPos+1);

			return ParseNameAndFormat( &path.c_str()[fileNameStartPos+1] );
		}
	};
}