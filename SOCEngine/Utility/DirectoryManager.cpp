#include "DirectoryManager.h"
using namespace Utility;

DirectoryManager::DirectoryManager(void)
{
	_paths.push_back("../../Resource/");
	_paths.push_back("../../Shader/");
	//환경설정 파일 만들고 바꿔야함
}
DirectoryManager::~DirectoryManager(void)
{

}

const std::string& DirectoryManager::GetPath(DirectoryType type) const
{
	return _paths[type];
}

bool DirectoryManager::isAbsolutePath(const std::string& fileName)
{
	if(fileName[0] == '/' || fileName[1] == ':')
		return true;
	else
		return false;
}

const std::string DirectoryManager::FullPathforFileName(const std::string& fileName, DirectoryManager::DirectoryType type)
{
	if(isAbsolutePath(fileName))
		return fileName;

	std::string fullPath = "";

	fullPath = fullPath + _paths[type] + fileName;
	
	return fullPath;
}
