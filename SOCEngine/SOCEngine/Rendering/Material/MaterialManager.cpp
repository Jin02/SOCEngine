#include "MaterialManager.h"

using namespace Rendering;
using namespace Rendering::Manager;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
}

void MaterialManager::Add(const std::string& file, const std::string& name, Material* material)
{
	std::string key = file + ":" + name;
	Add(key, material);
}

Material* MaterialManager::Find(const std::string& file, const std::string& name)
{
	std::string key = file + ":" + name;
	return Find(key);
}

void MaterialManager::Delete(const std::string& file, const std::string& name)
{
	std::string key = file  + ":" + name;
	Delete(key);
}

void MaterialManager::DeleteAll()
{
	for(auto iter = _hash.begin(); iter != _hash.end(); ++iter)
		SAFE_DELETE(iter->second);

	_hash.clear();
}

void MaterialManager::Add(const std::string& key, Material* material)
{
	ASSERT_COND_MSG(Find(key) == nullptr, "Error, Duplicated key");
	_hash.insert(std::make_pair(key, material));
}

Material* MaterialManager::Find(const std::string& key)
{
	auto findIter = _hash.find(key);
	return findIter != _hash.end() ? findIter->second : nullptr;;
}

void MaterialManager::Delete(const std::string& key)
{
	auto findIter = _hash.find(key);
	if(findIter != _hash.end())
	{
		SAFE_DELETE(findIter->second);
		_hash.erase(findIter);
	}
}
