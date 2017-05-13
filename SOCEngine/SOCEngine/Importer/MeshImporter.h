#pragma once

#include <string>

#include "VectorIndexer.hpp"
#include "Object.hpp"
#include "G3djDataTypes.hpp"
#include <unordered_map>
#include <rapidjson/document.h>
#include <set>

namespace Importer
{
	class MeshImporter
	{
	public:
		//key is Node::Parts::MeshPartId
		typedef std::unordered_map<std::string, const Node*>				NodeHashMap;
		typedef std::unordered_map<std::string, const Mesh::Intersection*>	IntersectionHashMap;

		static constexpr int GetMaximumRecognizeBoneCount() { return 4; }

		void Destroy();

		void ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer, bool isObjFormat);
		void ParseBinary(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const void* buffer, uint size);
		Core::Object* Load(const std::string& fileDir, bool useOriginalObject = false, bool useDynamicVB = false, bool useDynamicIB = false, Rendering::Material::Type materialType = Rendering::Material::Type::PhysicallyBasedModel);

	private:
		struct StoredOriginObject
		{
			bool			alreadyUsed;
			Core::Object*	object;
		};

		void ParseNode(Node& outNodes, const rapidjson::Value& node, const Math::Matrix& parentWorldMatrix,
			bool isSettedParentTranslation = false, bool isSettedParentRotation = false, bool isSettedParentScale = false);
		void ParseMaterial(Material& outMaterial, const rapidjson::Value& matNode, bool isObjFormat);
		void ParseMesh(Mesh& outMesh, const rapidjson::Value& meshNode, const NodeHashMap& nodeHashMap);
		void CalculateTangents(
			std::vector<Math::Vector3>& outTangents,
			const std::vector<Mesh::Part>& parts,
			const std::vector<float>& vertices,
			uint originStrideSize, uint uv0PosInAttributes);

		std::string GetVertexBufferKey(const std::string& meshFileName, uint meshIdx, std::string* outChunkKey) const;

		void MakeMaterials(std::set<std::string>& outNormalMapMaterialKeys, const std::vector<Material>& materials, const std::string& folderDir, const std::string& meshFileName);
		void MakeHierarchy(Core::Object* parent, const Node& node, const std::string& meshFileName, Rendering::Manager::BufferManager* bufferManager, Rendering::Manager::MaterialManager* materialManager, const IntersectionHashMap& intersectionHashMap);
		StoredOriginObject* BuildMesh(std::vector<Mesh>& meshes, const std::vector<Material>& materials, const std::vector<Node>& nodes, const std::string& folderDir, const std::string& meshFileName, bool useDynamicVB, bool useDynamicIB, const std::string& registKey);
		void FetchNormalMapMeshKeyLists(std::vector<std::pair<std::string, std::string>>& outNormalMapMeshes, const Node& node, const std::string& meshFileName);
		void FetchNodeHashMap(NodeHashMap& outNodeHashMap, const std::vector<Node>& nodes);

		// key is meshPartId, second value is materialId
		void FetchAllPartsInHashMap_Recursive(
			std::unordered_map<std::string, std::vector<std::string>>& outParts,
			const Node& node);

		Core::VectorMap<std::string, StoredOriginObject>	_originObjects;
	};
}