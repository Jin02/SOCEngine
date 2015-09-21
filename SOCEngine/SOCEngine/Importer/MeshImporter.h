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
		void ParseNode(Node& outNodes, const rapidjson::Value& node);
		void ParseMaterial(Material& outMaterial, const rapidjson::Value& matNode);
		void ParseMesh(Mesh& outMesh, const rapidjson::Value& meshNode);

	private:

	public:
		Core::Object* ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer);
		Core::Object* ParseBinary(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const void* buffer, uint size);
		Core::Object* Load(const std::string& fileDir, Rendering::Material::Type materialType = Rendering::Material::Type::PhysicallyBasedModel, bool isDynamicMesh = false);

	public:
		Core::Object* Find(const std::string& key);
	};
}