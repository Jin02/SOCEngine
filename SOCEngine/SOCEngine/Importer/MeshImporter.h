#pragma once

#include <string>

#include "VectorIndexer.hpp"
#include "Object.h"
#include "G3djDataTypes.hpp"
#include <unordered_map>
#include <rapidjson/document.h>
#include <set>
#include <memory>
#include "MaterialManager.hpp"
#include "ObjectManager.h"
#include "BoundBox.h"
#include "Texture2DManager.h"
#include "BufferManager.hpp"
#include "ComponentSystem.hpp"

namespace Importer
{
	class MeshImporter
	{
	public:
		struct ManagerParam
		{
			Rendering::Manager::BufferManager&		bufferManager;
			Rendering::Manager::MaterialManager&	materialManager;
			Core::ObjectManager&					objManager;
			Core::TransformPool&					transformPool;
			Rendering::Manager::Texture2DManager&	tex2DManager;
			Core::ComponentSystem&					compoSystem;
			Device::DirectX&						dx;
		};

		//key is Node::Parts::MeshPartId
		typedef std::unordered_map<std::string, Node>				NodeHashMap;
		typedef std::unordered_map<std::string, Mesh::Intersection>	IntersectionHashMap;

		static constexpr int GetMaximumRecognizeBoneCount() { return 4; }

		void ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer, bool isObjFormat);
		Core::Object& Load(ManagerParam managerParam, const std::string& fileDir, bool useDynamicVB = false, bool useDynamicIB = false);

	private:
		struct StoredOriginObject
		{
			bool			alreadyUsed;
			Core::ObjectId	objectId;
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

		void MakeMaterials(std::set<std::string>& outNormalMapMaterialKeys, ManagerParam manager, const std::vector<Material>& materials, const std::string& folderDir, const std::string& meshFileName);
		void MakeHierarchy(Core::Object& parent, const Node& node, const std::string& meshFileName, const ManagerParam& managerParam, const IntersectionHashMap& intersectionHashMap);
		StoredOriginObject* BuildMesh(ManagerParam managerParam,
			std::vector<Mesh>& meshes, const std::vector<Material>& materials, const std::vector<Node>& nodes, const std::string& folderDir, const std::string& meshFileName, bool useDynamicVB, bool useDynamicIB, const std::string& registKey);
		void FetchNormalMapMeshKeyLists(std::vector<std::pair<std::string, std::string>>& outNormalMapMeshes, const Node& node, const std::string& meshFileName);
		void FetchNodeHashMap(NodeHashMap& recurRefParts, const std::vector<Node>& nodes);

		// key is meshPartId, second value is materialId
		void FetchAllPartsInHashMap_Recursive(
			std::unordered_map<std::string, std::vector<std::string>>& outParts,
			const Node& node);

	private:
		Core::VectorMap<std::string, StoredOriginObject>	_originObjects;
	};
}