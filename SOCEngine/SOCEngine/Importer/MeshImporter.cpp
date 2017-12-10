#include "MeshImporter.h"
#include <fstream>
#include "PhysicallyBasedMaterial.h"
#include "BoundBox.h"
#include "Utility.hpp"
#include <unordered_set>
#include "ObjectManager.h"

using namespace Importer;
using namespace Core;
using namespace rapidjson;
using namespace Math;
using namespace Device;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Material;
using namespace Rendering;
using namespace Intersection;

std::vector<Vector3> CalculateTangents(
	const std::vector<Mesh::Part>& parts,
	const std::vector<float>& vertexDatas,
	uint originStrideSize, uint uv0PosInAttributes)
{
	uint elemCountInStride	= originStrideSize / sizeof(float);
	uint totalSize			= vertexDatas.size() / elemCountInStride;

	std::vector<Vector3> sdir(totalSize);
	std::vector<Vector3> tdir(totalSize);

	for (auto iter = parts.begin(); iter != parts.end(); ++iter)
	{
		const auto& indices = iter->indices;
		uint size = indices.size();
		for (uint i = 0; i<size; i += 3)
		{
			uint idx0 = indices[i + 0];
			uint idx1 = indices[i + 1];
			uint idx2 = indices[i + 2];

			std::array<Vector3, 3> vertices;
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

			std::array<Vector2, 3> uvs;
			{
				uvs[0].x = vertexDatas[idx0 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[0].y = vertexDatas[idx0 * elemCountInStride + uv0PosInAttributes + 1];

				uvs[1].x = vertexDatas[idx1 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[1].y = vertexDatas[idx1 * elemCountInStride + uv0PosInAttributes + 1];

				uvs[2].x = vertexDatas[idx2 * elemCountInStride + uv0PosInAttributes + 0];
				uvs[2].y = vertexDatas[idx2 * elemCountInStride + uv0PosInAttributes + 1];
			}

			Vector3 vtx_21	= vertices[1] - vertices[0];
			Vector3 vtx_31	= vertices[2] - vertices[0];
			Vector2 uv_21	= uvs[1] - uvs[0];
			Vector2 uv_31	= uvs[2] - uvs[0];

			float r = 1.0f / (uv_21.x * uv_31.y - uv_21.y * uv_31.x);

			Vector3 tangent((uv_31.y * vtx_21.x - uv_31.x * vtx_31.x) * r,
							(uv_31.y * vtx_21.y - uv_31.x * vtx_31.y) * r,
							(uv_31.y * vtx_21.z - uv_31.x * vtx_31.z) * r);			//sdir

			Vector3 binormal(	(uv_21.x * vtx_31.x - uv_21.y * vtx_21.x) * r,
								(uv_21.x * vtx_31.y - uv_21.y * vtx_21.y) * r,
								(uv_21.x * vtx_31.z - uv_21.y * vtx_21.z) * r	);	//tdir

			sdir[idx0] += tangent;
			sdir[idx1] += tangent;
			sdir[idx2] += tangent;

			tdir[idx0] += binormal;
			tdir[idx1] += binormal;
			tdir[idx2] += binormal;
		}
	}

	std::vector<Vector3> tangent(totalSize);
	for (uint i = 0; i < totalSize; ++i)
	{
		Vector3 normal(	vertexDatas[i * elemCountInStride + 0 + 3],
						vertexDatas[i * elemCountInStride + 1 + 3],
						vertexDatas[i * elemCountInStride + 2 + 3]	);

		float handedness = (Vector3::Dot(Vector3::Cross(normal, sdir[i]), tdir[i]) < 0.0f) ? -1.0f : 1.0f;
		tangent[i] = (sdir[i] - normal * Vector3::Dot(sdir[i], normal)).Normalized() * handedness;
	}

	return tangent;
}

Node MeshImporter::ParseNode(const rapidjson::Value& node,
							 const Matrix& parentWorldMatrix,
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
		localMatrix = Matrix::RotateUsingQuaternion(rotation.tf);

		localMatrix._41 = translation.tf.x;
		localMatrix._42 = translation.tf.y;
		localMatrix._43 = translation.tf.z;

		localMatrix._11 = scale.tf.x;
		localMatrix._22 = scale.tf.y;
		localMatrix._33 = scale.tf.z;


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
				parts.meshPartID = node["meshpartid"].GetString();
			if(node.HasMember("materialid"))
				parts.materialID = node["materialid"].GetString();

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
			Node childNode = ParseNode(childs[i], worldMatrix, translation.has, rotation.has, scale.has);
			currentNode.childs.push_back(childNode);
		}
	}

	return currentNode;
}

Importer::Material MeshImporter::ParseMaterial(const rapidjson::Value& matNode, bool isObjFormat)
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
			texInfo.fileName	= std::string("Textures/") + texInfoNode["filename"].GetString();

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

	return material;
}

Importer::Mesh MeshImporter::ParseMesh(	const rapidjson::Value& meshNode,
										const NodeHashMap& nodeHashMap	) //key is Node::Parts::MeshPartID
{
	Importer::Mesh outMesh;

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
		std::unordered_set<uint> indexHashSet;
		for(uint i=0; i<size; ++i)
		{
			Mesh::Part part;

			const auto& node = partsNode[i];
			part.meshPartID = node["id"].GetString();
			
			const auto& indicesNode = node["indices"];
			uint indicesCount = indicesNode.Size();

			Vector3	botOffset(0.0f, 0.0f, 0.0f);
			bool	isSetupTranslation = false;
			{
				auto findIter = nodeHashMap.find(part.meshPartID);
				if(findIter != nodeHashMap.end())
				{
					isSetupTranslation = findIter->second.translation.has;

					const Matrix& worldMat = findIter->second.worldMatrix;
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

	return outMesh;
}

void MeshImporter::ParseJson(std::vector<Importer::Mesh>& outMeshes, std::vector<Importer::Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer, bool isObjFormat)
{
	Document document;
	document.Parse(buffer);

	// "Error, Invalid Json File"
	assert(document.HasParseError() == false);

	// "Error, Where is Node?"
	assert(document.HasMember("nodes"));
	{
		const Value& nodes = document["nodes"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Node node = ParseNode(nodes[i], Matrix::Identity());			
			outNodes.push_back(node);
		}
	}

	NodeHashMap nodeHashMap;
	FetchNodeHashMap(nodeHashMap, outNodes);

	// "Error, Where is Mesh?"
	assert(document.HasMember("meshes"));
	{
		const Value& nodes = document["meshes"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Mesh mesh = ParseMesh(nodes[i], nodeHashMap);
			outMeshes.push_back(mesh);
		}
	}

	// "Error, Where is Material?"
	assert(document.HasMember("materials"));	
	{
		const Value& nodes = document["materials"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Material mat = ParseMaterial(nodes[i], isObjFormat);
			outMaterials.push_back(mat);
		}
	}
}

ObjectID MeshImporter::Load(const ManagerParam&& managerParam, const std::string& fileDir, bool useDynamicVB, bool useDynamicIB)
{
	std::string fileName, fileFormat, folderDir;
	assert( Utility::String::ParseDirectory(fileDir, folderDir, fileName, fileFormat) );

	auto key = fileDir;
	// Check duplicated object
	{
		StoredOriginObject* found = _originObjects.Find(key); 
		if (found)
			return managerParam.objManager.Find(fileName)->GetObjectID();
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

	// "Error, Invalid Mesh File"
	bool open = g3dFile.is_open();
	bool good = g3dFile.good();
	assert(open & good);

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

	assert(g3dFileFormat == "g3dj");
	ParseJson(meshes, materials, nodes, buffer, isObjFormat);

	delete buffer;

	return BuildMesh(managerParam, meshes, materials, nodes, folderDir, fileName, useDynamicVB, useDynamicIB, key);
}

Core::ObjectID MeshImporter::BuildMesh(
	ManagerParam managerParam,
	std::vector<Importer::Mesh>& meshes,
	const std::vector<Importer::Material>& materials, const std::vector<Node>& nodes,
	const std::string& folderDir, const std::string& meshFileName, bool useDynamicVB, bool useDynamicIB,
	const std::string& registKey)
{
	std::set<std::string> normalMapMaterialKeys = MakeMaterials(managerParam, materials, folderDir, meshFileName);

	// key is meshPartID, second value is materialID
	std::unordered_map<std::string, std::vector<std::string>> meshMaterialIDInAllParts;
	//auto FetchAllPartsInHashMap = [&]()
	{
		for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
			FetchAllPartsInHashMap_Recursive(meshMaterialIDInAllParts, *iter);
	};

	IntersectionHashMap intersectionHashMap;

	// Setting VB, IB
	{
		uint meshIndex = 0;
		for(auto meshIter = meshes.begin(); meshIter != meshes.end(); ++meshIter, ++meshIndex)
		{
			uint vbKey = Utility::String::MakeKey({ meshFileName , std::to_string(meshIndex) });

			std::vector<std::string> meshPartIDKeys;

			// Make IndexBuffer
			{
				const auto& parts = meshIter->parts;
				for(auto partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
				{
					intersectionHashMap.insert(std::make_pair(partsIter->meshPartID, partsIter->intersection));

					const auto& indices = partsIter->indices;

					IndexBuffer indexBuffer;
					indexBuffer.Initialize(managerParam.dx, indices, vbKey, useDynamicIB);

					auto& ibPool = managerParam.bufferManager.GetPool<IndexBuffer>();

					uint ibKey = Utility::String::MakeKey({ meshFileName, partsIter->meshPartID });
					ibPool.Add(ibKey, indexBuffer);

					meshPartIDKeys.push_back(partsIter->meshPartID);
				}
			}

			bool hasNormalMap = false;
			for(auto iter = meshPartIDKeys.begin();
				(iter != meshPartIDKeys.end()) && (hasNormalMap == false);
				++iter)
			{
				auto findIter = meshMaterialIDInAllParts.find(*iter);

				//Error, Invalid meshPartID
				assert(findIter != meshMaterialIDInAllParts.end());

				const auto& matIDs = findIter->second;
				for(auto matIDsIter = matIDs.begin(); matIDsIter != matIDs.end(); ++matIDsIter)
				{
					const std::string& materialID	= *matIDsIter;
					auto normalMapMatFindIter		= normalMapMaterialKeys.find(materialID);

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
							semantic.name			= attr;
							semantic.semanticIndex	= semanticIndex;
							semantic.size			= stride - prevStride;
						}
						semantics.push_back(semantic);

						prevStride = stride;
					}
				}

				if(hasNormalMap)
				{
					//tangent를 버퍼에 끼워넣는 작업.

					uint originStride = stride;
					stride += sizeof(Vector3);
					auto& vertexDatas = meshIter->vertexDatas;

					std::vector<Vector3> tangents =
						CalculateTangents(meshIter->parts, meshIter->vertexDatas, originStride, uv0Pos);

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
					auto& vertices	= meshIter->vertexDatas;
					VertexBuffer::Desc desc(stride, vertices.size() / (stride / 4));

					VertexBuffer vertexBuffer;
					vertexBuffer.Initialize(managerParam.dx, desc, vertices.data(), useDynamicVB, semantics);

					auto& vbPool	= managerParam.bufferManager.GetPool<VertexBuffer>();
					vbPool.Add(vbKey, vertexBuffer);
				}
			}
		}
	}

	// Make Hierachy
	Object root = managerParam.objManager.Add(meshFileName);
		
	for(auto& node : nodes)
		MakeHierarchy(managerParam.dx, root.GetObjectID(), node, meshFileName, managerParam, intersectionHashMap);

	_originObjects.Add(registKey, { true, root.GetObjectID() });
	return root.GetObjectID();
}

void MeshImporter::FetchAllPartsInHashMap_Recursive(
	std::unordered_map<std::string, std::vector<std::string>>& recurRefParts, const Node& node)
{
	for(auto part : node.parts)
	{
		auto findIter = recurRefParts.find(part.materialID);
		if(findIter != recurRefParts.end())
			findIter->second.push_back(part.materialID);
		else
		{
			std::vector<std::string> materialIDs;
			materialIDs.push_back(part.materialID);

			recurRefParts.insert(std::make_pair(part.meshPartID, materialIDs));
		}
	}

	const auto& childs = node.childs;
	for(auto& node : childs)
		FetchAllPartsInHashMap_Recursive(recurRefParts, node);
}

std::set<std::string> MeshImporter::MakeMaterials(	ManagerParam manager,
													const std::vector<Importer::Material>& materials,
													const std::string& folderDir, const std::string& meshFileName	)
{
	std::set<std::string> outNormalMapMaterialKeys;

	auto MakeMaterial = 
		[&outNormalMapMaterialKeys, folderDir, &manager, meshFileName]
	(const Material& impMat)
	{
		MaterialManager& materialMgr	= manager.materialManager;
		Texture2DManager& textureMgr	= manager.tex2DManager;
		DirectX& dx						= manager.dx;

		const std::string materialName = impMat.id;
		assert(materialName.empty() == false); // "Material has not key"

		auto material = materialMgr.Find<PhysicallyBasedMaterial>(meshFileName + ":" + materialName);

		if(material == nullptr)
		{
			auto material = PhysicallyBasedMaterial(materialName);			
			material.Initialize(dx);
			material.SetMainColor(Color(impMat.diffuse[0], impMat.diffuse[1], impMat.diffuse[2], impMat.opacity));
			material.SetEmissiveColor(Color(impMat.emissive[0], impMat.emissive[1], impMat.emissive[2], 0.0f));

			const auto& textures = impMat.textures;
			for (auto iter = textures.begin(); iter != textures.end(); ++iter)
			{
				auto texture = textureMgr.LoadTextureFromFile(dx, folderDir + iter->fileName, false);

				if (texture)
				{
					const auto& tex2D = *texture;

					if (iter->type == Material::Texture::Type::Diffuse)				material.RegistDiffuseMap(tex2D);
					else if (iter->type == Material::Texture::Type::Specular)		material.RegistMetallicMap(tex2D);
					else if (iter->type == Material::Texture::Type::Emissive)		material.RegistEmissionMap(tex2D);
					else if (iter->type == Material::Texture::Type::Reflection)		material.RegistRoughnessMap(tex2D);
					else if (iter->type == Material::Texture::Type::Transparency)	material.RegistOpacityMap(tex2D);
					else if ((iter->type == Material::Texture::Type::Normal))
					{
						material.RegistNormalMap(tex2D);
						outNormalMapMaterialKeys.insert(materialName);
					}
					else
						assert(0); // "Warning, Unsupported Texture Type.
				}
				
			}

			materialMgr.Add<PhysicallyBasedMaterial>(meshFileName + ":" + materialName, material);
		}
	};

	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
		MakeMaterial(*iter);

	return outNormalMapMaterialKeys;
}

void MeshImporter::MakeHierarchy(	Device::DirectX& dx,
									Core::ObjectID parentID, const Node& node,
									const std::string& meshFileName,
									const ManagerParam& managerParam,
									const IntersectionHashMap& intersectionHashMap	)
{
	auto& objManager	= managerParam.objManager;
	Object object		= objManager.Add(node.id);

	uint objID			= object.GetObjectID().Literal();
	objManager.Find(parentID)->AddChild(object);

	// Setting Transform
	auto& transformPool	= managerParam.transformPool;
	Transform* thisTF	= transformPool.Find(objID);
	{
		thisTF->UpdatePosition(node.translation.tf);
		thisTF->UpdateQuaternion(node.rotation.tf);
		thisTF->UpdateScale(node.scale.tf);
	}

	auto AttachMeshComponent = [&dx, intersectionHashMap, managerParam, meshFileName](Object& object, const Node::Parts& part)
	{
		// Setting Intersection
		BoundBox boundBox;
		float radius = 0.0f;
		{
			auto findIter = intersectionHashMap.find(part.meshPartID);
			if(findIter != intersectionHashMap.end())
			{
				boundBox.SetMinMax(findIter->second.boundBoxMin, findIter->second.boundBoxMax);
				radius = findIter->second.radius;
			}
		}
		
		auto& buferMgr		= managerParam.bufferManager;
		auto& materialMgr	= managerParam.materialManager;

		uint ibKey = Utility::String::MakeKey({ meshFileName, part.meshPartID });
		IndexBuffer* indexBuffer = buferMgr.GetPool<IndexBuffer>().Find(ibKey);
		assert(indexBuffer); // "Error, Invalid mesh part id"

		uint vbKey = indexBuffer->GetVBKey();
		assert(vbKey != -1); // "Error, Can't find VB. Invalid VBKey"

		VertexBuffer* vertexBuffer = buferMgr.GetPool<VertexBuffer>().Find(vbKey);
		assert(vertexBuffer); // "Error, Can't find VB. Invalid VBKey"

		auto& mesh = object.AddComponent<Rendering::Geometry::Mesh>();
		mesh.Initialize(dx, vertexBuffer->GetSemantics(), vbKey, ibKey);
		mesh.SetBoundBox(boundBox);
		mesh.SetRadius(radius);

		std::string strKey	= meshFileName + ":" + part.materialID;
		bool existMaterial	= materialMgr.Has<PhysicallyBasedMaterial>(strKey);
		MaterialID matID	= managerParam.materialManager.FindID<PhysicallyBasedMaterial>(existMaterial ? strKey : "@Default");
		mesh.SetPBRMaterialID(matID);
	};

	// attach submesh and mesh component.
	{
		const auto& parts	= node.parts;
		uint size			= parts.size();

		if(size > 1)
		{
			for(auto iter = parts.begin(); iter != parts.end(); ++iter)
			{
				const std::string& subMeshName = iter->meshPartID;
				Object subMeshObj = objManager.Add(subMeshName);
				object.AddChild(subMeshObj);

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
		MakeHierarchy(dx, object.GetObjectID(), *iter, meshFileName, managerParam, intersectionHashMap);
}

void MeshImporter::FetchNodeHashMap(NodeHashMap& outNodeHashMap, const std::vector<Node>& nodes)
{
	for(auto iter = nodes.begin(); iter != nodes.end(); ++iter)
	{
		const auto& parts = iter->parts;
		for(auto partsIter = parts.begin(); partsIter != parts.end(); ++partsIter)
			outNodeHashMap.insert( std::make_pair(partsIter->meshPartID, *iter) );

		const auto& childs = iter->childs;
		if(childs.empty() == false)
			FetchNodeHashMap(outNodeHashMap, childs);
	}
}
