#pragma once

#include "ObjImporter.h"

namespace Importer
{
	class MeshImporter
	{
	private:
		//Obj::ObjImporter*	_objImporter;

	public:
		MeshImporter();
		~MeshImporter();

	public:
		Core::Object* Load(const std::string& fileDir, const std::string& materialFolder, Rendering::Material::Type materialType, bool isDynamicMesh = false);


	};
}