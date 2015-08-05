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
			// �� ù ���ڰ� �������� ��� ����
			std::string::size_type lastPos = str.find_first_not_of(delimiters, 0);
			// �����ڰ� �ƴ� ù ���ڸ� ã�´�
			std::string::size_type pos     = str.find_first_of(delimiters, lastPos);

			while (std::string::npos != pos || std::string::npos != lastPos)
			{
				// token�� ã������ vector�� �߰��Ѵ�
				tokens.push_back(str.substr(lastPos, pos - lastPos));
				// �����ڸ� �پ�Ѵ´�.  "not_of"�� �����϶�
				lastPos = str.find_first_not_of(delimiters, pos);
				// ���� �����ڰ� �ƴ� ���ڸ� ã�´�
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
#define DEBUG_LOG(MSG)\
	OutputDebugString(MSG);
#else
#define DEBUG_LOG(X) X;
#endif

#define ASSERT_MSG(MSG) assert(!MSG)
#define ASSERT_COND_MSG(CONDITION, MSG) if(!(CONDITION)){assert(!MSG);}