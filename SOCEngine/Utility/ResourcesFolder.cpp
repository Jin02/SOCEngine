#include "ResourcesFolder.h"

using namespace Utility;


std::string ResourcesFolder::GetPath(const char* fileName)
{
	std::string path = fileName;
	path.insert(0, DIR);

	return path;
}

std::string ResourcesFolder::GetPath(const char* fileName, std::string& out)
{
	std::string path = fileName;
	path.insert(0, DIR);
	out = path;
	return path;
}

const char* ResourcesFolder::GetDir()
{
	return DIR;
}

int ResourcesFolder::GetDirLength()
{
	return strlen(DIR);
}