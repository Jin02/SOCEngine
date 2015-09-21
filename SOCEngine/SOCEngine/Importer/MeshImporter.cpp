#include "MeshImporter.h"
#include "Utility.h"
#include <fstream>

using namespace Importer;
using namespace Core;
using namespace Utility;
using namespace rapidjson;
using namespace Math;

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
		SAFE_DELETE(*iter);

	_originObjects.DeleteAll();
}

Core::Object* MeshImporter::Find(const std::string& key)
{
	Core::Object** found = _originObjects.Find(key);
	return found ? (*found) : nullptr;
}

void MeshImporter::ParseNode(Node& outNodes, const rapidjson::Value& node)
{
	Quaternion rotation(0.f, 0.f, 0.f, 1.f);
	{
		if(node.HasMember("rotation"))
		{
			const auto& rotationNode = node["rotation"];
			rotation.x = rotationNode[0u].GetDouble();
			rotation.y = rotationNode[1u].GetDouble();
			rotation.z = rotationNode[2u].GetDouble();
			rotation.w = rotationNode[3u].GetDouble();
		}
	}

	Vector3 translation(0.f, 0.f, 0.f);
	{
		if(node.HasMember("translation"))
		{
			const auto& translationNode = node["translation"];
			translation.x = translationNode[0u].GetDouble();
			translation.y = translationNode[1u].GetDouble();
			translation.z = translationNode[2u].GetDouble();
		}
	}

	Vector3 scale(1.0f, 1.0f, 1.0f);
	{
		if(node.HasMember("scale"))
		{
			const auto& scaleNode = node["scale"];
			scale.x = scaleNode[0u].GetDouble();
			scale.y = scaleNode[1u].GetDouble();
			scale.z = scaleNode[2u].GetDouble();
		}
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
	}

	if(node.HasMember("children"))
	{
		const auto& childs = node["children"];
		uint size = childs.Size();
		for(uint i=0; i<size; ++i)
		{
			Node childNode;
			ParseNode(childNode, childs[i]);
			currentNode.childs.push_back(childNode);
		}
	}

	outNodes = currentNode;
}

void MeshImporter::ParseMaterial(Material& outMaterial, const rapidjson::Value& matNode)
{
	Material material;

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
			texInfo.type		= texInfoNode["type"].GetString();
			outMaterial.textures.push_back(texInfo);
		}
	}

	outMaterial = material;
}

void MeshImporter::ParseMesh(Mesh& outMesh, const rapidjson::Value& meshNode)
{
	// Setting Attributes
	{
		const auto& attributesNode = meshNode["attributes"];
		uint size = attributesNode.Size();
		for(uint i=0; i<size; ++i)
			outMesh.attributes.push_back( attributesNode[i].GetString() );
	}

	// Setting Vertices
	{
		const auto& verticesNode = meshNode["vertices"];
		uint size = verticesNode.Size();
		for(uint i=0; i<size; ++i)
			outMesh.vertexDatas.push_back( verticesNode[i].GetDouble() );
	}

	// Setting Parts
	{
		const auto& partsNode = meshNode["parts"];
		uint size = partsNode.Size();
		for(uint i=0; i<size; ++i)
		{
			Mesh::Part part;

			const auto& node = partsNode[i];
			part.meshPartId = node["id"].GetString();
		//	part.type = node["type"].GetString();
			
			const auto& indicesNode = node["indices"];
			uint indicesCount = indicesNode.Size();
			for(uint i=0; i<indicesCount; ++i)
			{
				uint index = indicesNode[i].GetUint();
				part.indices.push_back(index);
			}

			outMesh.parts.push_back(part);
		}
	}
}

Core::Object* MeshImporter::ParseJson(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const char* buffer)
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
			ParseNode(node, nodes[i]);
			outNodes.push_back(node);
		}
	}

	ASSERT_COND_MSG(document.HasMember("meshes"), "Error, Where is Mesh?");
	{
		const Value& nodes = document["meshes"];
		uint size = nodes.Size();
		for(uint i=0; i<size; ++i)
		{
			Mesh mesh;
			ParseMesh(mesh, nodes[i]);
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
			ParseMaterial(mat, nodes[i]);
			outMaterials.push_back(mat);
		}
	}

	return nullptr;
}

Core::Object* MeshImporter::ParseBinary(std::vector<Mesh>& outMeshes, std::vector<Material>& outMaterials, std::vector<Node>& outNodes, const void* buffer, uint size)
{
	ASSERT_MSG("can't supported format");
	return nullptr;
}

Object* MeshImporter::Load(const std::string& fileDir, Rendering::Material::Type materialType, bool isDynamicMesh)
{
	std::string fileName, fileExtension, folderDir;
	if( String::ParseDirectory(fileDir, folderDir, fileName, fileExtension) == false )
		return nullptr;

	std::ifstream g3dFile;
	std::string g3dFileFormat;
	const std::string supportedFileFormat[] = {"g3dj", /*"g3db"*/};
	for(uint i=0; i<ARRAYSIZE(supportedFileFormat); ++i)
	{
		g3dFileFormat = supportedFileFormat[i];

		std::string g3dFilePath = folderDir + fileName + "_" + fileExtension + "." + supportedFileFormat[i];
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

	if(g3dFileFormat == "g3dj")	ParseJson(meshes, materials, nodes, buffer);
	else						ParseBinary(meshes, materials, nodes, (void*)buffer, length-1);

	delete buffer;


	return nullptr;
}