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

Material* MaterialManager::Add(const std::string& file, const std::string& name, Material* material, bool copy)
{
	Structure::Map<Material>* inMap = _map.Find(file);
	if(inMap == nullptr)
		inMap = _map.Add(file, new Structure::Map<Material>, false);					

	return inMap->Add(name, material, copy);
}

Material* MaterialManager::Find(const std::string& file, const std::string& name)
{
	Material* material = nullptr;
	Structure::Map<Material>* inMap = _map.Find(file);
	if( inMap )
		material = inMap->Find(name);

	return material;
}

void MaterialManager::Delete(const std::string& file, const std::string& name, bool contentRemove)
{
	Structure::Map<Material>* inMap = _map.Find(file);
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

Material* MaterialManager::Add(const std::string& key, Material* material, bool copy)
{
	return _hash.Add(key, material, copy);
}

Material* MaterialManager::Find(const std::string& key)
{
	return _hash.Find(key);
}

void MaterialManager::Delete(const std::string& key, bool contentRemove)
{
	_hash.Delete(key,  contentRemove);
}
