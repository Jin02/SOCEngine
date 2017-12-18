#pragma once

#include <string>

#include "VectorIndexer.hpp"
#include "Object.h"
#include "G3djDataTypes.hpp"
#include <unordered_map>
#include <rapidjson/document.h>
#include <set>
#include <memory>
#include "MaterialManager.h"
#include "ObjectManager.h"
#include "BoundBox.h"
#include "Texture2DManager.h"
#include "BufferManager.hpp"
#include "ComponentSystem.h"

namespace Importer
{
	class MeshImporter
	{
	public:
		MeshImporter() = default;
		DISALLOW_ASSIGN_COPY(MeshImporter);

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

		//key is Node::Parts::MeshPartID
		typedef std::unordered_map<std::string, Node>				NodeHashMap;
		typedef std::unordered_map<std::string, Mesh::Intersection>	IntersectionHashMap;

		static constexpr int GetMaximumRecognizeBoneCount() { return 4; }

		Core::ObjectID Load(const ManagerParam&& managerParam, const std::string& fileDir, bool useDynamicVB = false, bool useDynamicIB = false);

	private:
		struct StoredOriginObject
		{
			bool			alreadyUsed;
			Core::ObjectID	objectID;
		};

		void ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer, bool isObjFormat);
		Node ParseNode(const rapidjson::Value& node, const Math::Matrix& parentWorldMatrix,
						bool isSettedParentTranslation = false, bool isSettedParentRotation = false, bool isSettedParentScale = false);
		Material ParseMaterial(const rapidjson::Value& matNode, bool isObjFormat);
		Mesh ParseMesh(const rapidjson::Value& meshNode, const NodeHashMap& nodeHashMap);

		std::set<std::string> MakeMaterials(ManagerParam manager, const std::vector<Material>& materials, const std::string& folderDir, const std::string& meshFileName);
		void MakeHierarchy(Device::DirectX& dx, Core::ObjectID parentID, const Node& node, const std::string& meshFileName, const ManagerParam& managerParam, const IntersectionHashMap& intersectionHashMap);
		Core::ObjectID BuildMesh(ManagerParam managerParam,
								std::vector<Mesh>& meshes, const std::vector<Material>& materials, const std::vector<Node>& nodes,
								const std::string& folderDir, const std::string& meshFileName,
								bool useDynamicVB, bool useDynamicIB, const std::string& registKey);
		void FetchNodeHashMap(NodeHashMap& recurRefParts, const std::vector<Node>& nodes);

		// key is meshPartID, second value is materialID
		void FetchAllPartsInHashMap_Recursive(std::unordered_map<std::string, std::vector<std::string>>& outParts, const Node& node);

	private:
		Core::VectorMap<std::string, StoredOriginObject>	_originObjects;
	};
}