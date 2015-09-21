#pragma once

#include "VectorMap.h"
#include <string>
#include "Object.h"
#include "Material.h"
#include "rapidjson/document.h"
#include "G3djDataTypes.h"

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

	private:
		void ParseNodes(Node& outNodes, const rapidjson::Value& node);
		void ParseMaterials(Material& outMaterial, const rapidjson::Value& matNode);
		void ParseMeshes(Mesh& outMesh, const rapidjson::Value& meshNode);

	public:
		Core::Object* ParseJson(const char* buffer);
		Core::Object* ParseBinary(const void* buffer, uint size);
		Core::Object* Load(const std::string& fileDir, Rendering::Material::Type materialType = Rendering::Material::Type::PhysicallyBasedModel, bool isDynamicMesh = false);

	public:
		Core::Object* Find(const std::string& key);
	};
}