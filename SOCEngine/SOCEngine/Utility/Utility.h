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
	class Common
	{
	public:
		static unsigned short ConvertF32ToF16(float fValueToConvert)
		{
			// single precision floating point format:
			// |1|   8    |          23           |
			// |s|eeeeeeee|mmmmmmmmmmmmmmmmmmmmmmm|

			// half precision floating point format:
			// |1|  5  |    10    |
			// |s|eeeee|mmmmmmmmmm|

			unsigned int uFloatBits = (*(unsigned int *) &fValueToConvert);

			// note, this functions does not handle values that are too large (i.e. overflow), 
			// nor does it handle NaNs or infinity
			int  nExponent = (int)((uFloatBits & 0x7F800000u) >> 23) - 127 + 15;
			assert(nExponent < 31);

			// if the resulting value would be a denorm or underflow, then just return a (signed) zero
			if( nExponent <= 0 )
			{
				return (unsigned short)((uFloatBits & 0x80000000u) >> 16);
			}

			// else, exponent is in the range [1,30], and so we can represent 
			// the value to convert as a normalized 16-bit float (with some loss of precision, of course)
			unsigned int uSignBit =      (uFloatBits & 0x80000000u) >> 16;
			unsigned int uExponentBits = (unsigned int)nExponent << 10;
			unsigned int uMantissaBits = (uFloatBits & 0x007FFFFFu) >> 13;

			return (unsigned short)(uSignBit | uExponentBits | uMantissaBits);
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