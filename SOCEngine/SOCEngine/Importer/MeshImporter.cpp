#include "MeshImporter.h"
#include "Utility.h"

using namespace Importer;
using namespace Core;
using namespace Utility;

MeshImporter::MeshImporter() : _objImporter(nullptr)
{
}

MeshImporter::~MeshImporter()
{
	SAFE_DELETE(_objImporter);
}

void MeshImporter::Initialize()
{
	SAFE_DELETE(_objImporter);
	_objImporter = new Obj::ObjImporter;
}

Object* MeshImporter::Load(const std::string& fileDir, const std::string& materialFolder, Rendering::Material::Type materialType, bool isDynamicMesh)
{
	std::string fileName, fileExtension, folderDir;
	if( String::ParseDirectory(fileDir, folderDir, fileName, fileExtension) == false )
		return nullptr;

	Object* meshObject = nullptr;
	if(fileExtension == "obj")
		meshObject = _objImporter->Load(fileDir, fileName, materialFolder, materialType, isDynamicMesh);

	return meshObject;
}