#include "MaterialManager.h"

using namespace Rendering;
using namespace Rendering::Manager;

MaterialManager::MaterialManager()
{
}

MaterialManager::~MaterialManager()
{
	DeleteAll(true);
}

BasicMaterial* MaterialManager::Add(const std::string& file, const std::string& name, BasicMaterial* material, bool copy)
{
	Structure::Map<std::string, BasicMaterial>* inMap = _map.Find(file);
	if(inMap == nullptr)
		inMap = _map.Add(file, new Structure::Map<std::string, BasicMaterial>, false);					

	return inMap->Add(name, material, copy);
}

BasicMaterial* MaterialManager::Find(const std::string& file, const std::string& name)
{
	BasicMaterial* material = nullptr;
	Structure::Map<std::string, BasicMaterial>* inMap = _map.Find(file);
	if( inMap )
		material = inMap->Find(name);

	return material;
}

void MaterialManager::Delete(const std::string& file, const std::string& name, bool contentRemove)
{
	Structure::Map<std::string, BasicMaterial>* inMap = _map.Find(file);
	if(inMap)
		_map.Delete(name, contentRemove);
}

void MaterialManager::DeleteFile(const std::string& file, bool remove)
{
	_map.Delete(file, remove);
}

void MaterialManager::DeleteAll(bool remove)
{
	_map.DeleteAll(remove);
	_hash.DeleteAll(remove);
}

BasicMaterial* MaterialManager::Add(const std::string& key, BasicMaterial* material, bool copy)
{
	return _hash.Add(key, material, copy);
}

BasicMaterial* MaterialManager::Find(const std::string& key)
{
	return _hash.Find(key);
}

void MaterialManager::Delete(const std::string& key, bool contentRemove)
{
	_hash.Delete(key,  contentRemove);
}
