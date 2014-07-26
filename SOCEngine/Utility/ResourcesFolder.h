#pragma once

#include <string>
#include "Common.h"

namespace Utility
{
	class ResourcesFolder
	{
	private:
		#define DIR  "../Resources/"

	public:
		static std::string GetPath(const char* fileName);
		static std::string GetPath(const char* fileName, std::string& out);
		static const char* GetDir();
		static int GetDirLength();
	};

}