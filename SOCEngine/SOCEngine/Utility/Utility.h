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

		static bool ParseDirectory(const std::string& path, std::string& outFolderPath, std::string& outFileName, std::string& outFileExtension)
		{
			return ParseDirectory(path, &outFolderPath, &outFileName, &outFileExtension);
		}

		static bool ParseDirectory(const std::string& path, std::string* outFolderPath, std::string* outFileName, std::string* outFileExtension)
		{
			auto ParseNameAndExtension = [&](const std::string& fileNameWithExtension)
			{
				unsigned int extensionPos = fileNameWithExtension.find('.');
				if( extensionPos == -1 )
					return false;

				if(outFileExtension)
					*outFileExtension = &fileNameWithExtension.c_str()[extensionPos+1];

				if(outFileName)
					*outFileName = fileNameWithExtension.substr(0, extensionPos); 

				return true;
			};

			unsigned int fileNameStartPos = path.rfind('/');
			if( fileNameStartPos == -1 )
			{
				fileNameStartPos = path.rfind('\\');
				if(fileNameStartPos == -1)
					return ParseNameAndExtension(path);
			}

			if(outFolderPath)
				*outFolderPath = path.substr(0, fileNameStartPos+1);

			return ParseNameAndExtension( &path.c_str()[fileNameStartPos+1] );
		}
	};
}
#if defined(_DEBUG)
#define DEBUG_LOG(X)\
	OutputDebugString(X);
#else
#define DEBUG_LOG(X) X;
#endif

#define ASSERT(MSG) assert(!MSG)