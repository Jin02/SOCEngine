#pragma once

#include "VectorMap.h"
#include <string>
#include "Object.h"
#include "Material.h"
#include "rapidjson/document.h"
#include "G3djDataTypes.h"
#include "MaterialManager.h"
#include <set>
#include <hash_map>
#include "BufferManager.h"

namespace Importer
{
	class MeshImporter
	{
	public:
		static const int MaximumRecognizeBoneCount = 4;

	private:
		Structure::VectorMap<std::string, Core::Object*>	_originObjects;

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
		void CalculateTangents(
			std::vector<Math::Vector3>& outTangents,
			const std::vector<Mesh::Part>& parts, 
			const std::vector<float>& vertices, 
			uint originStrideSize, uint uv0PosInAttributes);

		std::string GetVertexBufferKey(const std::string& meshFileName, uint meshIdx, std::string* outChunkKey) const;

	private:
		void MakeMaterials(std::set<std::string>& outNormalMapMaterialKeys, const std::vector<Material>& materials, const std::string& meshFileName);
		void MakeHierarchy(Core::Object* parent, const Node& node, const std::string& meshFileName, Rendering::Manager::BufferManager* bufferManager, Rendering::Manager::MaterialManager* materialManager);
		Core::Object* BuildMesh(std::vector<Mesh>& meshes, const std::vector<Material>& materials, const std::vector<Node>& nodes, const std::string& meshFileName, bool useDynamicVB, bool useDynamicIB);
		void FetchNormalMapMeshKeyLists(std::vector<std::pair<std::string, std::string>>& outNormalMapMeshes, const Node& node, const std::string& meshFileName);

		// key is meshPartId, second value is materialId
		void FetchAllPartsInHashMap_Recursive(
			std::hash_map<std::string, std::vector<std::string>>& outParts,
			const Node& node);

	public:
		void ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer);
		void ParseBinary(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const void* buffer, uint size);
		Core::Object* Load(const std::string& fileDir, bool useDynamicVB = false, bool useDynamicIB = false, Rendering::Material::Type materialType = Rendering::Material::Type::PhysicallyBasedModel);

	public:
		Core::Object* Find(const std::string& key);
	};
}