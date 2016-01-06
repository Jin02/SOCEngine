#include "MeshImporter.h"
#include "Utility.h"
#include <fstream>
#include "ResourceManager.h"
#include "PhysicallyBasedMaterial.h"
#include "Director.h"
#include "Scene.h"
#include "RenderManager.h"
#include "ShaderForm.h"
#include "BufferManager.h"
#include "ImporterUtility.h"
#include "BoundBox.h"
#include <hash_set>

using namespace Importer;
using namespace Core;
using namespace Utility;
using namespace rapidjson;
using namespace Math;
using namespace Resource;
using namespace Device;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering;
using namespace Intersection;

MeshImporter::MeshImporter()
{
}

MeshImporter::~MeshImporter()
{
	Destroy();
}

void MeshImporter::Initialize()
{
}

void MeshImporter::Destroy()
{
	auto vector = _originObjects.GetVector();
	for(auto iter = vector.begin(); iter != vector.end(); ++iter)
		SAFE_DELETE(iter->object);

	_originObjects.DeleteAll();
}

void MeshImporter::ParseNode(Node& outNodes, const rapidjson::Value& node,
							 const Math::Matrix& parentWorldMatrix,
							 bool isSettedParentTranslation, bool isSettedParentRotation, bool isSettedParentScale)
{
	Node::Transform<Quaternion> rotation;
	{
		rotation.has = node.HasMember("rotation");
		if(rotation.has)
		{
			const auto& rotationNode = node["rotation"];
			rotation.tf.x = rotationNode[0u].GetDouble();
			rotation.tf.y = rotationNode[1u].GetDouble();
			rotation.tf.z = rotationNode[2u].GetDouble();
			rotation.tf.w = rotationNode[3u].GetDouble();
		}
		else
		{
			rotation.tf = Quaternion(0.0f, 0.0f, 0.0f, 1.0f);
		}

		rotation.has |= isSettedParentRotation;
	}

	Node::Transform<Vector3> translation;
	{
		translation.has = node.HasMember("translation");
		if(translation.has)
		{
			const auto& translationNode = node["translation"];
			translation.tf.x = translationNode[0u].GetDouble();
			translation.tf.y = translationNode[1u].GetDouble();
			translation.tf.z = translationNode[2u].GetDouble();
		}
		else
		{
			translation.tf = Vector3(0.0f, 0.0f, 0.0f);
		}

		translation.has |= isSettedParentTranslation;
	}

	Node::Transform<Vector3> scale;
	{
		scale.has = node.HasMember("scale");
		if(scale.has)
		{
			const auto& scaleNode = node["scale"];
			scale.tf.x = scaleNode[0u].GetDouble();
			scale.tf.y = scaleNode[1u].GetDouble();
			scale.tf.z = scaleNode[2u].GetDouble();
		}
		else
		{
			scale.tf = Vector3(1.0f, 1.0f, 1.0f);
		}

		scale.has |= isSettedParentScale;
	}

	Matrix localMatrix, worldMatrix;
	{
		Transform tempTF(nullptr);
		tempTF.UpdatePosition(translation.tf);
		tempTF.UpdateScale(scale.tf);
		tempTF.UpdateRotation(rotation.tf);

		tempTF.FetchLocalMatrix(localMatrix);

		worldMatrix = localMatrix * parentWorldMatrix;
	}


	std::string id = "Null";
	{
		if(node.HasMember("id"))
			id = node["id"].GetString();
	}


	auto ParseParts = [](std::vector<Node::Parts>& outParts, const Value& node)
	{
		if(node.HasMember("parts") == false)
			return;
		
		const auto& partsNode = node["parts"];
		uint size = partsNode.Size();
		for(uint i=0; i<size; ++i)
		{
			const auto& node = partsNode[i];
			Node::Parts parts;

			if(node.HasMember("meshpartid"))
				parts.meshPartId = node["meshpartid"].GetString();
			if(node.HasMember("materialid"))
				parts.materialId = node["materialid"].GetString();

			outParts.push_back(parts);
		}
	};

	std::vector<Node::Parts> parts;
	ParseParts(parts, node);

	Node currentNode;
	{
		currentNode.id			= id;
		currentNode.rotation	= rotation;
		currentNode.scale		= scale;
		currentNode.translation	= translation;
		currentNode.parts		= parts;
		currentNode.localMatrix	= localMatrix;
		currentNode.worldMatrix	= worldMatrix;
	}

	if(node.HasMember("children"))
	{
		const auto& childs = node["children"];
		uint size = childs.Size();
		for(uint i=0; i<size; ++i)
		{
			Node childNode;
			ParseNode(childNode, childs[i], worldMatrix,
						translation.has, rotation.has, scale.has);
			currentNode.childs.push_back(childNode);
		}
	}

	outNodes = currentNode;
}

void MeshImporter::ParseMaterial(Importer::Material& outMaterial, const rapidjson::Value& matNode, bool isObjFormat)
{
	Importer::Material material;

	material.id = matNode["id"].GetString();
	auto ParseVec3Types = [](Vector3& out, const Value& matValue, const std::string& name)
	{
		if(matValue.HasMember(name.c_str()))
		{
			const auto& matNode = matValue[name.c_str()];
			out.x = matNode[0u].GetDouble();
			out.y = matNode[1u].GetDouble();
			out.z = matNode[2u].GetDouble();
		}
	};
	ParseVec3Types(material.ambient, matNode, "ambient");
	ParseVec3Types(material.diffuse, matNode, "diffuse");
	ParseVec3Types(material.specular, matNode, "specular");
	ParseVec3Types(material.emissive, matNode, "emissive");

	if(matNode.HasMember("opacity"))
		material.opacity = matNode["opacity"].GetDouble();

	if(isObjFormat)
		material.opacity = 1.0f;

	if(matNode.HasMember("shininess"))
		material.shininess	= matNode["shininess"].GetDouble();

	if(matNode.HasMember("textures"))
	{
		const auto& texturesNode = matNode["textures"];
		uint texSize = texturesNode.Size();
		for(uint texIdx = 0; texIdx < texSize; ++texIdx)
		{
			const auto& texInfoNode = texturesNode[texIdx];
			Material::Texture texInfo;
			texInfo.id			= texInfoNode["id"].GetString();
			texInfo.fileName	= texInfoNode["filename"].GetString();

			const std::string& type = texInfoNode["type"].GetString();
			if(type == "DIFFUSE")			texInfo.type = Material::Texture::Type::Diffuse;
			else if(type == "AMBIENT")		texInfo.type = Material::Texture::Type::Ambient;
			else if(type == "NORMAL" || 
				type == "BUMP")				texInfo.type = Material::Texture::Type::Normal;
			else if(type == "EMISSIVE")		texInfo.type = Material::Texture::Type::Emissive;
			else if(type == "Reflection")	texInfo.type = Material::Texture::Type::Reflection;
			else if(type == "SHININESS")	texInfo.type = Material::Texture::Type::Shininess;
			else if(type == "TRANSPARENCY")	texInfo.type = Material::Texture::Type::Transparency;
			else if(type == "SPECULAR")		texInfo.type = Material::Texture::Type::Specular;
			else							texInfo.type = Material::Texture::Type::Undefined;

			material.textures.push_back(texInfo);
		}
	}

	outMaterial = material;
}

void MeshImporter::ParseMesh(Importer::Mesh& outMesh, const rapidjson::Value& meshNode,
							 const NodeHashMap& nodeHashMap) //key is Node::Parts::MeshPartId
{
	uint stride		= 0;
	bool hasNormal	= false;

	const auto& attributesNode = meshNode["attributes"];
	for(uint i=0; i<attributesNode.Size(); ++i)
	{
		std::string attr = attributesNode[i].GetString();

		if(attr == "POSITION")		stride += sizeof(Vector3);
		else if(attr == "NORMAL")
		{
			stride += sizeof(Vector3);
			hasNormal = true;
		}
		else if(attr == "COLOR")	stride += sizeof(Vector4);
		else
		{
			auto IsValidAttribute = [](const std::string& attr, const std::string& getAttrStr) -> bool
			{
				return ( attr.find(getAttrStr) == 0) && ( attr.size() > getAttrStr.size() );
			};
			auto GetAttributeIndex = [](const std::string& attr) -> uint
			{
				std::string numStr = "";
				for(auto iter = attr.rbegin(); iter != attr.rend(); ++iter)
				{									
					if('0' <= *iter && *iter <='9')
						numStr.insert(numStr.begin(), *iter);
					else break;
				}

				return atoi(numStr.c_str());
			};

			if(IsValidAttribute(attr, "TEXCOORD"))
				stride += sizeof(Vector2);

			if(IsValidAttribute(attr, "BONEWEIGHT"))
				stride += sizeof(Vector2);
		}

		outMesh.attributes.push_back(attr);
	}
	

	BoundBox entireBox;
	// Setting Vertices
	{
		const auto& verticesNode = meshNode["vertices"];
		uint size = verticesNode.Size();

		Vector3 bbMin(10000, 10000, 10000);
		Vector3 bbMax(-bbMin);

		uint fltCountInStride = stride / sizeof(float);
		for(uint i=0; i<size; i+=fltCountInStride)
		{
			Vector3 pos( verticesNode[i + 0].GetDouble(),
						 verticesNode[i + 2].GetDouble(),
						-verticesNode[i + 1].GetDouble() );

			outMesh.vertexDatas.push_back(pos.x);
			outMesh.vertexDatas.push_back(pos.y);
			outMesh.vertexDatas.push_back(pos.z);

			if(bbMin.x > pos.x) bbMin.x = pos.x;
			if(bbMin.y > pos.y) bbMin.y = pos.y;
			if(bbMin.z > pos.z) bbMin.z = pos.z;

			if(bbMax.x < pos.x) bbMax.x = pos.x;
			if(bbMax.y < pos.y) bbMax.y = pos.y;
			if(bbMax.z < pos.z) bbMax.z = pos.z;

			if(hasNormal)	// normal의 위치는 pos의 다음 위치로 고정되어 있다. 
							// 뭐 어짜피 위치가 변동적이지 않기 때문에, 그냥 고정으로 해도 상관없다.
			{
				outMesh.vertexDatas.push_back(  verticesNode[i + 3].GetDouble() ); //x
				outMesh.vertexDatas.push_back(  verticesNode[i + 5].GetDouble() ); //y
				outMesh.vertexDatas.push_back( -verticesNode[i + 4].GetDouble() ); //z
			}

			for(uint j = hasNormal ? 6 : 3; j < fltCountInStride; ++j)
				outMesh.vertexDatas.push_back( verticesNode[j + i].GetDouble() );
		}

		entireBox.SetMinMax(bbMin, bbMax);
	}

	// Setting Parts
	{
		const auto& partsNode = meshNode["parts"];
		uint size = partsNode.Size();
		std::hash_set<uint> indexHashSet;
		for(uint i=0; i<size; ++i)
		{
			Mesh::Part part;

			const auto& node = partsNode[i];
			part.meshPartId = node["id"].GetString();
			
			const auto& indicesNode = node["indices"];
			uint indicesCount = indicesNode.Size();

			Vector3	botOffset(0.0f, 0.0f, 0.0f);
			bool	isSetupTranslation = false;
			{
				auto findIter = nodeHashMap.find(part.meshPartId);
				if(findIter != nodeHashMap.end())
				{
					isSetupTranslation = findIter->second->translation.has;

					const Node* importerNode = findIter->second;
					const Matrix& worldMat = importerNode->worldMatrix;
					botOffset.x = worldMat._41;
					botOffset.y = worldMat._42;
					botOffset.z = worldMat._43;

					botOffset.y += entireBox.GetMin().y;
				}
			}

			float	radius = 0.0f;
			Vector3	bbMin(10000, 10000, 10000);
			Vector3	bbMax(-bbMin);

			const uint lineCount	= stride / sizeof(float);
			auto& vertices			= outMesh.vertexDatas;
			for(uint i=0; i<indicesCount; ++i) //CW
			{
				uint index = indicesNode[i].GetUint();
				uint posIdxOffsetInVertices = index * lineCount;

				float& x = vertices[posIdxOffsetInVertices + 0];
				float& y = vertices[posIdxOffsetInVertices + 1];
				float& z = vertices[posIdxOffsetInVertices + 2];

				if(isSetupTranslation)
				{
					auto findIter = indexHashSet.find(index);
					if(findIter == indexHashSet.end())
					{
						x -= botOffset.x;
						y -= botOffset.y;
						z -= botOffset.z;

						indexHashSet.insert(index);
					}
				}


				Vector3 pos(x, y, z);
				{
					// Compute Radius
					{
						float length = pos.Length();
						if(length > radius) radius = length;
					}

					// Compute BoundBox Min/Max
					{
						if(bbMin.x > pos.x) bbMin.x = pos.x;
						if(bbMin.y > pos.y) bbMin.y = pos.y;
						if(bbMin.z > pos.z) bbMin.z = pos.z;

						if(bbMax.x < pos.x) bbMax.x = pos.x;
						if(bbMax.y < pos.y) bbMax.y = pos.y;
						if(bbMax.z < pos.z) bbMax.z = pos.z;
					}
				}
				
				part.indices.push_back(index);
			}
			part.intersection.radius		= radius;
			part.intersection.boundBoxMin	= bbMin;
			part.intersection.boundBoxMax	= bbMax;

			outMesh.parts.push_back(part);
		}
	}
}

void MeshImporter::ParseJson(std::vector<Importer::Mesh>& outMeshes, std::vector<Importer::Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer, bool isObjFormat)
{
	Document document;
	document.Parse(buffer);
	ASSERT_COND_MSG(document.HasParseError() == false, "Error, Invalid Json File");

	ASSERT_COND_MSG(document.HasMember("nodes"), "Error, Where is Node?");
	{
		const Value& nodes = document["nodes"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Node node;
			Matrix identityMat;
			Matrix::Identity(identityMat);
			ParseNode(node, nodes[i], identityMat);
			outNodes.push_back(node);
		}
	}

	NodeHashMap nodeHashMap;
	FetchNodeHashMap(nodeHashMap, outNodes);

	ASSERT_COND_MSG(document.HasMember("meshes"), "Error, Where is Mesh?");
	{
		const Value& nodes = document["meshes"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Mesh mesh;
			ParseMesh(mesh, nodes[i], nodeHashMap);
			outMeshes.push_back(mesh);
		}
	}

	ASSERT_COND_MSG(document.HasMember("materials"), "Error, Where is Material?");
	{
		const Value& nodes = document["materials"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Material mat;
			ParseMaterial(mat, nodes[i], isObjFormat);
			outMaterials.push_back(mat);
		}
	}
}

void MeshImporter::ParseBinary(std::vector<Importer::Mesh>& outMeshes, std::vector<Importer::Material>& outMaterials, std::vector<Node>& outNodes, const void* buffer, uint size)
{
	ASSERT_MSG("can't supported format");
}

Object* MeshImporter::Load(const std::string& fileDir, bool useOriginalObject, bool useDynamicVB, bool useDynamicIB, Rendering::Material::Type materialType)
{
	std::string fileName, fileFormat, folderDir;
	if( String::ParseDirectory(fileDir, folderDir, fileName, fileFormat) == false )
		return nullptr;

	// Check duplicated object
	{
		StoredOriginObject* found = _originObjects.Find(fileDir); 
		if(found)
		{
			if(useOriginalObject && found->alreadyUsed)
				ASSERT_MSG("Error, This object is already used.");

			return found->object->Clone();
		}
	}

	std::ifstream g3dFile;
	std::string g3dFileFormat;
	const std::string supportedFileFormat[] = {"g3dj", /*"g3db"*/};
	for(uint i=0; i<ARRAYSIZE(supportedFileFormat); ++i)
	{
		g3dFileFormat = supportedFileFormat[i];

		std::string g3dFilePath = folderDir + fileName + "_" + fileFormat + "." + supportedFileFormat[i];
		g3dFile.open(g3dFilePath.c_str(), std::ios::in | std::ios::ate | std::ios::binary );

		if(g3dFile.is_open() && g3dFile.good())
			break;
	}

	if((g3dFile.is_open() == false) || (g3dFile.good() == false))
		ASSERT_MSG("Error, Invalid Mesh File");

	std::streamoff length = g3dFile.tellg();
	g3dFile.seekg(0, g3dFile.beg);

	char* buffer = new char[(uint)length + 1];
	{
		g3dFile.read(buffer, length);
		buffer[length] = '\0';
	}
	g3dFile.close();

	std::vector<Mesh>		meshes;
	std::vector<Material>	materials;
	std::vector<Node>		nodes;

	bool isObjFormat = fileFormat == "obj";

	if(g3dFileFormat == "g3dj")	ParseJson(meshes, materials, nodes, buffer, isObjFormat);
	else						ParseBinary(meshes, materials, nodes, (void*)buffer, length-1);

	delete buffer;

	StoredOriginObject* storedObject = BuildMesh(meshes, materials, nodes, folderDir, fileName, useDynamicVB, useDynamicIB, fileDir);

	if(useOriginalObject == false)
	{
		Object* retObj = storedObject->object;

		std::string originName = retObj->GetName();
		retObj = retObj->Clone();
		retObj->SetName(originName);

		return retObj;
	}
//	else{

	storedObject->alreadyUsed = true;
	return storedObject->object;
//	}
}

MeshImporter::StoredOriginObject* MeshImporter::BuildMesh(
	std::vector<Importer::Mesh>& meshes, const std::vector<Importer::Material>& materials, const std::vector<Node>& nodes,
	const std::string& folderDir, const std::string& meshFileName, bool useDynamicVB, bool useDynamicIB,
	const std::string& registKey)
{
	std::set<std::string> normalMapMaterialKeys;
	MakeMaterials(normalMapMaterialKeys, materials, folderDir, meshFileName);

	BufferManager* bufferMgr = ResourceManager::GetInstance()->GetBufferManager();
	MaterialManager* materialManager = Director::GetInstance()->GetCurrentScene()->GetMaterialManager();

	// key is meshPartId, second value is materialId
	std::hash_map<std::string, std::vector<std::string>> meshMaterialIdInAllParts;
	//auto FetchAllPartsInHashMap = [&]()
	{
		for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
			FetchAllPartsInHashMap_Recursive(meshMaterialIdInAllParts, *iter);
	};

	IntersectionHashMap intersectionHashMap;

	// Setting VB, IB
	{
		uint meshIterIdx = 0;
		for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter, ++meshIterIdx)
		{
			std::string vbChunkKey = "";
			std::string vertexBufferKey = GetVertexBufferKey(meshFileName, meshIterIdx, &vbChunkKey);

			std::vector<std::string> meshPartIdKeys;

			// Make IndexBuffer
			{
				const auto& parts = meshIter->parts;
				for(auto partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
				{
					intersectionHashMap.insert(std::make_pair(partsIter->meshPartId, &partsIter->intersection));

					const auto& indices = partsIter->indices;
					IndexBuffer* indexBuffer = new IndexBuffer;

					bool success = indexBuffer->Initialize(indices, vertexBufferKey, useDynamicIB);
					ASSERT_COND_MSG(success, "Error, Can't create index buffer");

					bufferMgr->Add(meshFileName, partsIter->meshPartId, indexBuffer);

					meshPartIdKeys.push_back(partsIter->meshPartId);
				}
			}

			bool hasNormalMap = false;
			for(auto iter = meshPartIdKeys.begin();
				(iter != meshPartIdKeys.end()) && (hasNormalMap == false);
				++iter)
			{
				auto findIter = meshMaterialIdInAllParts.find(*iter);

				if(findIter == meshMaterialIdInAllParts.end())
					ASSERT_MSG("Error, Invalid meshPartId");

				const auto& matIds = findIter->second;
				for(auto matIdsIter = matIds.begin(); matIdsIter != matIds.end(); ++matIdsIter)
				{
					const std::string& materialId	= *matIdsIter;
					auto normalMapMatFindIter		= normalMapMaterialKeys.find(materialId);

					if(normalMapMatFindIter != normalMapMaterialKeys.end())
					{
						hasNormalMap = true;
						break;
					}
				}
			}

			std::vector<VertexShader::SemanticInfo> semantics; // attributes
			// Setting VB
			{
				auto& attributes = meshIter->attributes;

				uint stride	= 0;
				uint uv0Pos	= 0;
				{
					uint prevStride = 0;
					for(auto iter = attributes.begin(); iter != attributes.end(); ++iter)
					{
						std::string attr = *iter;

						uint semanticIndex = 0;

						if(attr == "POSITION")		stride += sizeof(Vector3);
						else if(attr == "NORMAL")	stride += sizeof(Vector3);
						else if(attr == "COLOR")	stride += sizeof(Vector4);
						else
						{
							auto IsValidAttribute = [](const std::string& attr, const std::string& getAttrStr) -> bool
							{
								return ( attr.find(getAttrStr) == 0) && ( attr.size() > getAttrStr.size() );
							};
							auto GetAttributeIndex = [](const std::string& attr) -> uint
							{
								std::string numStr = "";
								for(auto iter = attr.rbegin(); iter != attr.rend(); ++iter)
								{									
									if('0' <= *iter && *iter <='9')
										numStr.insert(numStr.begin(), *iter);
									else break;
								}

								return atoi(numStr.c_str());
							};

							if(IsValidAttribute(attr, "TEXCOORD"))
							{
								semanticIndex = GetAttributeIndex(attr);
								attr = "TEXCOORD";

								if(uv0Pos == 0)
									uv0Pos = stride / sizeof(float);

								stride += sizeof(Vector2);
							}
							if(IsValidAttribute(attr, "BONEWEIGHT"))
							{
								semanticIndex = GetAttributeIndex(attr);

								attr = "BONEWEIGHT";
								stride += sizeof(Vector2);
							}
						}

						VertexShader::SemanticInfo semantic;
						{
							semantic.name = attr;
							semantic.semanticIndex = semanticIndex;
							semantic.size = stride - prevStride;
						}
						semantics.push_back(semantic);

						prevStride = stride;
					}
				}

				if(hasNormalMap)
				{
					uint originStride = stride;
					stride += sizeof(Vector3);
					auto& vertexDatas = meshIter->vertexDatas;

					std::vector<Vector3> tangents;
					CalculateTangents(tangents, meshIter->parts, meshIter->vertexDatas, originStride, uv0Pos);

					// Tangent를 쓴다는건, 이미 앞에 Normal을 사용한다는 것과 같다.
					// 그래서 그냥 고정값임. 사실은 귀찮기도하고, 그래 귀찮다.
					const uint tangentOrder = (uint)Attribute::Tangent;

					//const uint originElemCount = originStride / sizeof(float);
					const uint next = originStride / sizeof(float);
					uint size = vertexDatas.size();

					uint tanIdx = 0;
					uint start = (sizeof(Vector3) * 2) / sizeof(float);
					for(uint i= start; i<size; i+=next)
					{
						const Vector3& tangent = tangents[tanIdx++];

						vertexDatas.insert( vertexDatas.begin() + i, tangent.z );	//z
						vertexDatas.insert( vertexDatas.begin() + i, tangent.y );	//y
						vertexDatas.insert( vertexDatas.begin() + i, tangent.x );	//x

						size = vertexDatas.size();
					}

					VertexShader::SemanticInfo info;
					{
						info.name = "TANGENT";
						info.size = sizeof(Vector3);
					}
					semantics.insert(semantics.begin() + tangentOrder, info);
				}

				// Make Vertex Buffer
				{
					auto& vertices = meshIter->vertexDatas;
					VertexBuffer* vertexBuffer = new VertexBuffer;

					uint count = vertices.size() / (stride / 4);

					vertexBuffer->Initialize(vertices.data(), stride, count, useDynamicVB, vertexBufferKey, &semantics);
					bufferMgr->Add(meshFileName, vbChunkKey, vertexBuffer);
				}
			}
		}
	}

	// Make Hierachy
	Object* root = new Object(meshFileName, nullptr);
		
	for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
		MakeHierarchy(root, (*iter), meshFileName, bufferMgr, materialManager, intersectionHashMap);

	_originObjects.Add(registKey, StoredOriginObject(false, root));
	StoredOriginObject* ret = _originObjects.Find(registKey);

	return ret;
}

void MeshImporter::FetchAllPartsInHashMap_Recursive(
	std::hash_map<std::string, std::vector<std::string>>& outParts, const Node& node)
{
	const auto& parts = node.parts;
	for(auto iter = parts.begin(); iter != parts.end(); ++iter)
	{
		auto findIter = outParts.find(iter->materialId);
		if(findIter != outParts.end())
			findIter->second.push_back(iter->materialId);
		else
		{
			std::vector<std::string> materialIds;
			materialIds.push_back(iter->materialId);

			outParts.insert(std::make_pair(iter->meshPartId, materialIds));
		}
	}

	const auto& childs = node.childs;
	for(auto iter = childs.begin(); iter != childs.end(); ++iter)
		FetchAllPartsInHashMap_Recursive(outParts, *iter);
}

void MeshImporter::FetchNormalMapMeshKeyLists(
	std::vector<std::pair<std::string, std::string>>& outNormalMapMeshes,
	const Node& node,
	const std::string& meshFileName)
{
	for(auto iter = node.parts.begin(); iter != node.parts.end(); ++iter)
	{
		const std::string& materialId = iter->materialId;
	}
}

std::string MeshImporter::GetVertexBufferKey(const std::string& meshFileName, uint meshIdx, std::string* outChunkKey) const
{
	std::string chunkKey = "Chunk" + std::to_string(meshIdx);
	if(outChunkKey)	(*outChunkKey) = chunkKey;
	return meshFileName + ":" + chunkKey;
}

void MeshImporter::MakeMaterials(std::set<std::string>& outNormalMapMaterialKeys, const std::vector<Importer::Material>& materials,
								 const std::string& folderDir, const std::string& meshFileName)
{
	const ResourceManager* resourceMgr	= ResourceManager::GetInstance();
	const Scene* scene					= Director::GetInstance()->GetCurrentScene();
	MaterialManager* materialMgr		= scene->GetMaterialManager();
	TextureManager* textureMgr			= resourceMgr->GetTextureManager();

	auto MakeMaterial = [&](const Material& impMat, Rendering::Material::Type materialType)
	{
		const std::string materialName = impMat.id;
		ASSERT_COND_MSG(materialName.empty() == false, "Material has not key");

		Rendering::Material* material = materialMgr->Find(meshFileName, materialName);

		if(material == nullptr)
		{
			if(materialType == Rendering::Material::Type::PhysicallyBasedModel)
				material = new PhysicallyBasedMaterial(materialName);			
			else
			{
				DEBUG_LOG("Warning, can't support material type.");
				material = new Rendering::Material(materialName, materialType);
			}

			material->Initialize();

			if(materialType == Rendering::Material::Type::PhysicallyBasedModel)
			{
				PhysicallyBasedMaterial* pbm = dynamic_cast<PhysicallyBasedMaterial*>(material);
				pbm->UpdateMainColor(Color(impMat.diffuse[0], impMat.diffuse[1], impMat.diffuse[2], impMat.opacity));
				pbm->SetEmissionColor(Color(impMat.emissive[0], impMat.emissive[1], impMat.emissive[2], 0.0f));

				const auto& textures = impMat.textures;
				for(auto iter = textures.begin(); iter != textures.end(); ++iter)
				{
					Texture::Texture2D* texture = textureMgr->LoadTextureFromFile(folderDir + "Textures/" + iter->fileName, false);

					if(texture)
					{
						if(iter->type == Material::Texture::Type::Diffuse)
							pbm->UpdateDiffuseMap(texture);
						else if( (iter->type == Material::Texture::Type::Normal) )
						{
							pbm->UpdateNormalMap(texture);
							outNormalMapMaterialKeys.insert(materialName);
						}
						else if(iter->type == Material::Texture::Type::Specular)
							pbm->UpdateSpecularMap(texture);
						else
						{
							DEBUG_LOG("Warning, Unsupported Texture Type.");
						}
					}
				}
			}
			else
			{
				DEBUG_LOG("Warning, can't support material type.");

				Color diffuseColor;
				diffuseColor.r = impMat.diffuse[0];
				diffuseColor.g = impMat.diffuse[1];
				diffuseColor.b = impMat.diffuse[2];
				diffuseColor.a = impMat.opacity;

				material->SetVariable("MainColor", diffuseColor);

				if( impMat.shininess > 0.0f )
					material->SetVariable("Shininess", impMat.shininess);

				const auto& textures = impMat.textures;
				for(auto iter = textures.begin(); iter != textures.end(); ++iter)
				{
					Texture::Texture2D* texture = textureMgr->LoadTextureFromFile(meshFileName + ":" + iter->fileName, false);

					if(iter->type == Material::Texture::Type::Diffuse)
					{
						const uint shaderSlotIndex = (uint)TextureBindIndex::DiffuseTex;
						material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
					}
					else if( (iter->type == Material::Texture::Type::Normal) )
					{
						const uint shaderSlotIndex = (uint)TextureBindIndex::NormalTex;
						material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
						outNormalMapMaterialKeys.insert(materialName);
					}
					else if(iter->type == Material::Texture::Type::Specular)
					{
						const uint shaderSlotIndex = (uint)TextureBindIndex::SpecularTex;
						material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
					}
					else
					{
						DEBUG_LOG("Warning, Unsupported Texture Type.");
					}
				}
			}

			materialMgr->Add(meshFileName, materialName, material);
		}
		else
		{
			DEBUG_LOG("Material Manager already has new mateiral. Please check key from new material");
		}
	};

	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		MakeMaterial(*iter, Rendering::Material::Type::PhysicallyBasedModel);
}

void MeshImporter::MakeHierarchy(Core::Object* parent, const Node& node,
								 const std::string& meshFileName,
								 BufferManager* bufferManager, MaterialManager* materialManager,
								 const IntersectionHashMap& intersectionHashMap)
{
	Object* object = new Object(node.id ,parent);

	// Setting Transform
	{
		Transform* tf = object->GetTransform();
		tf->UpdatePosition(node.translation.tf);
		tf->UpdateRotation(node.rotation.tf);
		tf->UpdateScale(node.scale.tf);
	}

	auto AttachMeshComponent = [&](Object* object, const Node::Parts& part)
	{
		// Setting Intersection
		{
			auto findIter = intersectionHashMap.find(part.meshPartId);
			if(findIter != intersectionHashMap.end())
			{
				BoundBox bb;
				bb.SetMinMax(findIter->second->boundBoxMin, findIter->second->boundBoxMax);
				
				object->SetRadius(findIter->second->radius);
				object->SetBoundBox(bb);
			}
		}
		IndexBuffer* indexBuffer = nullptr;
		bool success = bufferManager->Find(&indexBuffer, meshFileName, part.meshPartId);
		ASSERT_COND_MSG(success, "Error, Invalid mesh part id");

		std::string vbChunkKey = "";
		{
			std::vector<std::string> tokens;
			{
				const std::string& vertexBufferKey = indexBuffer->GetUseVertexBufferKey();
				Utility::String::Tokenize(vertexBufferKey, tokens, ":");
			}

			vbChunkKey = tokens.back();
		}
		ASSERT_COND_MSG(vbChunkKey.empty() == false, "Error, Invalid vb Chunk Key");

		VertexBuffer* vertexBuffer = nullptr;
		success = bufferManager->Find(&vertexBuffer, meshFileName, vbChunkKey);
		ASSERT_COND_MSG(success, "Error, Invalid vb Chunk Key");

		Rendering::Material* material = materialManager->Find(meshFileName, part.materialId);

		Rendering::Geometry::Mesh* mesh = object->AddComponent<Rendering::Geometry::Mesh>();
		mesh->Initialize(vertexBuffer, indexBuffer, material);
	};

	// attach submesh and mesh component.
	{
		const auto& parts =  node.parts;
		uint size = parts.size();

		if(size > 1)
		{
			for(auto iter = parts.begin(); iter != parts.end(); ++iter)
			{
				const std::string& subMeshId = iter->meshPartId;
				Object* subMeshObj = new Object(subMeshId, object);

				AttachMeshComponent(subMeshObj, *iter);
			}
		}
		else if(size > 0)
		{
			const Node::Parts& part = node.parts[0];
			AttachMeshComponent(object, part);
		}
	}

	auto& childs = node.childs;
	for(auto iter = childs.begin(); iter != childs.end(); ++iter)
		MakeHierarchy(object, *iter, meshFileName, bufferManager, materialManager, intersectionHashMap);
}

void MeshImporter::CalculateTangents(
	std::vector<Math::Vector3>& outTangents, 
	const std::vector<Importer::Mesh::Part>& parts,
	const std::vector<float>& vertexDatas,
	uint originStrideSize, uint uv0PosInAttributes)
{
	uint elemCountInStride = originStrideSize / sizeof(float);
	uint totalSize = vertexDatas.size() / elemCountInStride;
#if 0
	std::vector<std::pair<Math::Vector3, uint>> tangents(totalSize);
#else
	outTangents.clear();
	outTangents.resize(totalSize);
#endif

	for(auto iter = parts.begin(); iter != parts.end(); ++iter)
	{
		const auto& indices = iter->indices;
		uint size = indices.size();
		for(uint i=0; i<size; i+=3)
		{
			uint idx0 = indices[i + 0];
			uint idx1 = indices[i + 1];
			uint idx2 = indices[i + 2];

			std::array<Math::Vector3, 3> vertices;
			{
				vertices[0].x = vertexDatas[idx0 * elemCountInStride + 0];
				vertices[0].y = vertexDatas[idx0 * elemCountInStride + 1];
				vertices[0].z = vertexDatas[idx0 * elemCountInStride + 2];

				vertices[1].x = vertexDatas[idx1 * elemCountInStride + 0];
				vertices[1].y = vertexDatas[idx1 * elemCountInStride + 1];
				vertices[1].z = vertexDatas[idx1 * elemCountInStride + 2];

				vertices[2].x = vertexDatas[idx2 * elemCountInStride + 0];
				vertices[2].y = vertexDatas[idx2 * elemCountInStride + 1];
				vertices[2].z = vertexDatas[idx2 * elemCountInStride + 2];
			}

			std::array<Math::Vector2, 3> uvs;
			{
				uvs[0].x = vertexDatas[idx0 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[0].y = vertexDatas[idx0 * elemCountInStride + uv0PosInAttributes + 1];

				uvs[1].x = vertexDatas[idx1 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[1].y = vertexDatas[idx1 * elemCountInStride + uv0PosInAttributes + 1];

				uvs[2].x = vertexDatas[idx2 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[2].y = vertexDatas[idx2 * elemCountInStride + uv0PosInAttributes + 1];
			}

			Math::Vector3 tangent;
			ImporterUtility::CalculateTangent(tangent, vertices, uvs);
#if 0
			tangents[idx0].first += tangent;	tangents[idx0].second++;
			tangents[idx1].first += tangent;	tangents[idx1].second++;
			tangents[idx2].first += tangent;	tangents[idx2].second++;
#else
			outTangents[idx0] = tangent;
			outTangents[idx1] = tangent;
			outTangents[idx2] = tangent;
#endif
		}
	}

	// Normalize Tangents and Save ouput
	{
#if 0
		for(auto iter = tangents.begin(); iter != tangents.end(); ++iter)
		{
			uint count = iter->second;
			Math::Vector3 tangent = iter->first / (float)count;
			tangent = tangent.Normalize();
			outTangents.push_back(tangent);
		}
#endif
	}

}

void MeshImporter::FetchNodeHashMap(NodeHashMap& outNodeHashMap, const std::vector<Node>& nodes)
{
	for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
	{
		const auto& parts = iter->parts;
		for(auto partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
			outNodeHashMap.insert( std::make_pair(partsIter->meshPartId, &(*iter)) );

		const auto& childs = iter->childs;
		if(childs.empty() == false)
			FetchNodeHashMap(outNodeHashMap, childs);
	}
}