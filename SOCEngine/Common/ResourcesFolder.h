#pragma once

#include <string>

namespace Utility
{
	class ResourcesFolder
	{
#define DIR  "../Resources/"

	public:
		static std::string GetPath(const char* fileName)
		{
			std::string path = fileName;
			path.insert(0, DIR);

			return path;
		}

		static std::string GetPath(const char* fileName, std::string *out)
		{
			std::string path = fileName;
			path.insert(0, DIR);

			if(out)
				(*out) = path;

			return path;
		}

		static const char* GetDir()
		{
			return DIR;
		}

		static int GetDirLength()
		{
			return strlen(DIR);
		}
	};

}