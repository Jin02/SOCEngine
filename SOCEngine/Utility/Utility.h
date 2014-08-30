#pragma once

#include <string>
#include <vector>
#include <stdio.h>

class Utility
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

	static std::string WinCmd(const std::string& cmd)
	{
		FILE* fp = _popen(cmd.c_str(), "r");
		if( fp == nullptr )
			return "ERROR";

		char buffer[128];
		std::string result = "";
		while(!feof(fp))
		{
			if(fgets(buffer, 128, fp) != nullptr)
				result += buffer;
		}
		_pclose(fp);
		return result;
	}
};