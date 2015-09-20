#pragma once

#include "VectorMap.h"
#include <string>
#include "Object.h"
#include "Material.h"

namespace Importer
{
	class MeshImporter
	{
	private:
		Structure::VectorMap<std::string, Core::Object*>	_originObjects;

	private:

	public:
		MeshImporter();
		~MeshImporter();

	public:
		void Initialize();
		void Destroy();
		Core::Object* Load(const std::string& fileDir, Rendering::Material::Type materialType = Rendering::Material::Type::PhysicallyBasedModel, bool isDynamicMesh = false);

	public:
		Core::Object* Find(const std::string& key);
	};
}