#include "MeshImporter.h"
#include "Utility.h"

using namespace Importer;
using namespace Core;
using namespace Utility;

MeshImporter::MeshImporter()
{
}

MeshImporter::~MeshImporter()
{
	Destroy();
}

void MeshImporter::Initialize()
{
	std::function<void(const std::string& key, const Core::Object* obj)> test;

	auto Add = [&](const std::string& key, Core::Object* obj)
	{
		_originObjects.Add(key, obj);
	};
	auto Find = [&](const std::string& key)
	{
		return this->Find(key);
	};


}

void MeshImporter::Destroy()
{
	auto vector = _originObjects.GetVector();
	for(auto iter = vector.begin(); iter != vector.end(); ++iter)
		SAFE_DELETE(*iter);

	_originObjects.DeleteAll();

}

Core::Object* MeshImporter::Find(const std::string& key)
{
	Core::Object** found = _originObjects.Find(key);
	return found ? (*found) : nullptr;
}

Object* MeshImporter::Load(const std::string& fileDir, Rendering::Material::Type materialType, bool isDynamicMesh)
{
	std::string fileName, fileExtension, folderDir;
	if( String::ParseDirectory(fileDir, folderDir, fileName, fileExtension) == false )
		return nullptr;

	return nullptr;
}