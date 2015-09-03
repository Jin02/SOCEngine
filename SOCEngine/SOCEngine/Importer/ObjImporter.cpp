#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"
#include "ImporterUtility.h"
#include "RenderManager.h"
#include "ResourceManager.h"
#include "PhysicallyBasedMaterial.h"

#include <algorithm>
#include <ctype.h>

using namespace Importer;
using namespace Importer::Obj;
using namespace Rendering;
using namespace Rendering::Shader;
using namespace Manager;
using namespace Utility;
using namespace Resource;

ObjImporter::ObjImporter()
{
}

ObjImporter::~ObjImporter()
{
}

Material* ObjImporter::LoadMaterial(const tinyobj::material_t& tinyMaterial, const std::string& fileName, const std::string& materialFileFolder, Rendering::Material::Type materialType)
{
	const ResourceManager* resourceMgr = ResourceManager::GetInstance();
	auto scene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = scene->GetMaterialManager();
	TextureManager* textureMgr = resourceMgr->GetTextureManager();

	const std::string materialName = tinyMaterial.name;
	ASSERT_COND_MSG(materialName.empty() == false, "Material has not key");

	Material* material = materialMgr->Find(fileName, materialName);

	if(material == nullptr)
	{
		if(materialType == Material::Type::PhysicallyBasedModel)
			material = new PhysicallyBasedMaterial(materialName);			
		else
		{
			DEBUG_LOG("Warning, can't support material type.");
			material = new Material(materialName, materialType);
		}

		material->Initialize();

		// main color = diffuse color
		{
			Color diffuseColor;
			diffuseColor.r = tinyMaterial.diffuse[0];
			diffuseColor.g = tinyMaterial.diffuse[1];
			diffuseColor.b = tinyMaterial.diffuse[2];
			diffuseColor.a = tinyMaterial.dissolve;

			material->SetVariable("mainColor", diffuseColor);
		}

		if( tinyMaterial.shininess > 0.0f )
			material->SetVariable("shininess", tinyMaterial.shininess);

		// Using Utility::String::ParseDirectory
		std::string textureFileName, textureExtension;

		if(tinyMaterial.diffuse_texname.empty() == false)
		{
			Texture::Texture2D* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.diffuse_texname, false, true);

			const uint shaderSlotIndex = 0; // Default Diffuse Texture Shader Slot Index
			material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
		}

		if(tinyMaterial.normal_texname.empty() == false)
		{
			Texture::Texture2D* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.normal_texname, false, true);

			const uint shaderSlotIndex = 1; // Default Diffuse Texture Shader Slot Index
			material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
		}

		if(tinyMaterial.specular_texname.empty() == false)
		{
			Texture::Texture2D* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.specular_texname, false, true);

			const uint shaderSlotIndex = 2; // Default Diffuse Texture Shader Slot Index
			material->SetTextureUseShaderSlotIndex(shaderSlotIndex, texture, ShaderForm::Usage(false, false, false, true));
		}

		materialMgr->Add(fileName, materialName, material);
	}
	else
	{
		DEBUG_LOG("Material Manager already has new mateiral. Please check key from new material");
	}

	return material;
}

void ObjImporter::LoadMaterials(const std::vector<tinyobj::material_t>& tinyMaterials, const std::string& fileName, const std::string& materialFileFolder, Rendering::Material::Type materialType)
{
	const ResourceManager* resourceMgr = ResourceManager::GetInstance();
	auto scene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = scene->GetMaterialManager();
	TextureManager* textureMgr = resourceMgr->GetTextureManager();

	for(auto iter = tinyMaterials.begin(); iter != tinyMaterials.end(); ++iter)
	{
		LoadMaterial((*iter), fileName, materialFileFolder, materialType);

		//Material* material = LoadMaterial((*iter), fileName, materialFileFolder, materialType);
		//if(material && outMaterials)
		//	(*outMaterials)->Add(iter->name, material);
	}
}

bool ObjImporter::Load(std::vector<tinyobj::shape_t>& outShapes, std::vector<tinyobj::material_t>& outMaterials, const std::string& fileDir, const std::string& materialFileFolder)
{
	std::string error = tinyobj::LoadObj(outShapes, outMaterials, fileDir.c_str(), materialFileFolder.c_str());

	if( error.empty() == false )
	{
		DEBUG_LOG(error.c_str());
		return false;
	}

	return true;
}

Core::Object* ObjImporter::Load(const std::string& fileDir,
								const std::string& fileName, 
								const std::string& materialFileFolder,
								Rendering::Material::Type materialType,
								bool isDynamicMesh)
{
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;

	std::string error = tinyobj::LoadObj(shapes, materials, fileDir.c_str(), materialFileFolder.c_str());
	ASSERT_COND_MSG(error.empty(), error.c_str());

	LoadMaterials(materials, fileName, materialFileFolder, materialType);

	Core::Object* parent = new Core::Object(fileName);

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		Core::Object* child = LoadMesh((*iter), materials[iter->mesh.material_ids[0]], fileName, materialType, isDynamicMesh);
		parent->AddChild(child);
	}

	return parent;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& tinyShape, 
									const tinyobj::material_t& tinyMtl, 
									const std::string& fileName, 
									const std::vector<Rendering::Shader::VertexShader::SemanticInfo>& semanticInfos, 
									Rendering::Material::Type materialType,
									bool isDynamicMesh)
{
	std::string shapeName = tinyShape.name;
	{
		if(shapeName.empty())
			shapeName = fileName;
	}

	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, shapeName);
		if(obj)
			return obj;
	}

	if(semanticInfos.empty())
		ASSERT_MSG("Error, ObjImporter : semanticInfos paramater is null");

	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	if(resourceManager->GetBufferManager()->Find( (const void**)nullptr, fileName, shapeName ))
		ASSERT_MSG("Error, BufferMgr already has buffer");

	CheckCorrectShape(tinyShape);

	const std::vector<float>& tiny_positions		 = tinyShape.mesh.positions;
	const std::vector<float>& tiny_texcoords		 = tinyShape.mesh.texcoords;
	const std::vector<float>& tiny_normals			 = tinyShape.mesh.normals;
	const std::vector<uint>& indices		 = tinyShape.mesh.indices;

	bool isNormalMapUse = (tinyMtl.normal_texname.empty() == false);

	std::vector<Math::Vector3>		tangents;
	std::vector<Math::Vector3>		binormals;
	std::vector<Math::Vector3>		reCalcNormals;

	if(isNormalMapUse)
	{
		const Math::Vector3* vertices		= reinterpret_cast<const Math::Vector3*>(tiny_positions.data());
		const Math::Vector3* originNormals	= reinterpret_cast<const Math::Vector3*>(tiny_normals.data());
		const Math::Vector2* texcoords		= reinterpret_cast<const Math::Vector2*>(tiny_texcoords.data());

		ImporterUtility::ReCalculateTBN(tangents, binormals, reCalcNormals,
			vertices, tiny_positions.size() / 3, originNormals, texcoords, indices.data(), indices.size());
	}

	struct InputSemanticData
	{
		CustomSemantic customData;
		uint offset;

		InputSemanticData(const CustomSemantic& data) : customData(data), offset(0){}
	};

	uint stride = sizeof(Math::Vector3); //pos
	uint bufferFlag = 0;

	std::vector<InputSemanticData> vertexDatas;
	{
		for(auto iter = semanticInfos.begin(); iter != semanticInfos.end(); ++iter)
		{
			stride += iter->size;
			std::string upeerName;
			{
				std::transform(iter->name.begin(), iter->name.end(), upeerName.begin(), toupper);
			}
			
			if(upeerName == "NORMAL")
				bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::N;
			else if(upeerName == "TEXCOORD0")
				bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::UV;
			else if(upeerName == "TANGENT" || upeerName == "BINORMAL")
				bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::TB;
		}

		vertexDatas.push_back(CustomSemantic("POSITION", tiny_positions.data()));
		if( (bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::N) )
		{
			CustomSemantic custom;
			custom.semanticName = "NORMAL";

			if(isNormalMapUse && (reCalcNormals.empty() == false))
				custom.data = reCalcNormals.data();
			else if(tiny_normals.empty() == false)
				custom.data = tiny_normals.data();
			else
				ASSERT_MSG("Error, strange normal");

			vertexDatas.push_back(custom);
		}

		if( (bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::TB) &&
			((tangents.empty() == false) && (binormals.empty() == false)) )
		{
			vertexDatas.push_back(CustomSemantic("TANGENT",	tangents.data()));
			vertexDatas.push_back(CustomSemantic("BINORMAL", binormals.data()));
		}

		if( (bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::UV) &&
			tiny_texcoords.empty() == false)
			vertexDatas.push_back(CustomSemantic("TEXCOORD0", tiny_texcoords.data()));
	}

	uint vtxCount = tinyShape.mesh.positions.size() / 3;
	void* bufferHead = malloc(vtxCount * stride);
	{
		void* buffer = bufferHead;
		for(uint vtxIndex = 0; vtxIndex < vtxCount; ++vtxIndex)
		{
			for(auto infoIter = semanticInfos.begin(); infoIter != semanticInfos.end(); ++infoIter)
			{
				const std::string& name = infoIter->name;
				uint size = infoIter->size;

				int count = 0;
				for(auto iter = vertexDatas.begin(); iter != vertexDatas.end(); ++iter, ++count)
				{
					CustomSemantic& semantic = iter->customData;
					if( semantic.semanticName == name )
					{
						memcpy(buffer, ((char*)semantic.data) + iter->offset, infoIter->size);
						buffer = (char*)buffer + infoIter->size;
						iter->offset += infoIter->size;
						break;
					}
				}

				ASSERT_COND_MSG(count != vertexDatas.size(), "Not Found Semantic Data!");
			}
		}
	}
	

	Core::Object* object = new Core::Object(shapeName);
	Mesh::Mesh* mesh = object->AddComponent<Mesh::Mesh>();

	resourceManager->GetBufferManager()->Add(fileName, shapeName, bufferHead);

	auto scene = Device::Director::GetInstance()->GetCurrentScene();
	MaterialManager* materialMgr = scene->GetMaterialManager();
	Material* material = materialMgr->Find(fileName, tinyMtl.name);
	ASSERT_COND_MSG(material, "can not found material");

	//sizeof(ENGINE_INDEX_TYPE) * _indexCount

	Mesh::Mesh::CreateFuncArguments args(fileName, shapeName);
	{
		args.vertex.data		= bufferHead;
		args.vertex.count		= vtxCount;
		args.vertex.byteWidth	= stride;

		args.index.data			= indices.data();
		args.index.count		= indices.size();
		args.index.byteWidth	= 0; // not use

		args.material			= material;
		args.isDynamic			= isDynamicMesh;
		args.bufferFlag			= bufferFlag;
	}
	
	mesh->Initialize(args);

	const std::string objKey = fileName + ':' + shapeName;
	resourceManager->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& tinyShape,
									const tinyobj::material_t& tinyMtl,
									const std::string& fileName, 
									Rendering::Material::Type materialType,
									bool isDynamicMesh)
{
	std::string shapeName = tinyShape.name;
	{
		if(shapeName.empty())
			shapeName = fileName;
	}

	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, shapeName);
		if(obj)
			return obj;
	}

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	if(resourceManager->GetBufferManager()->Find( (const void**)nullptr, fileName, shapeName ))
		ASSERT_MSG("Error, BufferMgr already has buffer");

	CheckCorrectShape(tinyShape);

	const std::vector<float>& tiny_positions		= tinyShape.mesh.positions;
	const std::vector<float>& tiny_texcoords		= tinyShape.mesh.texcoords;
	const std::vector<float>& tiny_normals			= tinyShape.mesh.normals;
	const std::vector<uint>& indices				= tinyShape.mesh.indices;

	bool isNormalMapUse = (tinyMtl.normal_texname.empty() == false);

	std::vector<Math::Vector3>		tangents;
	std::vector<Math::Vector3>		binormals;
	std::vector<Math::Vector3>		reCalcNormals;

	if(isNormalMapUse)
	{
		const Math::Vector3* vertices		= reinterpret_cast<const Math::Vector3*>(tiny_positions.data());
		const Math::Vector3* originNormals	= reinterpret_cast<const Math::Vector3*>(tiny_normals.data());
		const Math::Vector2* texcoords		= reinterpret_cast<const Math::Vector2*>(tiny_texcoords.data());

		ImporterUtility::ReCalculateTBN(tangents, binormals, reCalcNormals,
			vertices, tiny_positions.size() / 3, originNormals, texcoords, indices.data(), indices.size());
	}

	uint vtxCount = tinyShape.mesh.positions.size() / 3;
	uint stride = sizeof(Math::Vector3); // pos
	uint bufferFlag = 0;
	{
		if(tiny_texcoords.empty() == false)
		{
			bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::UV;
			stride += sizeof(Math::Vector2);	//texcoord
		}

		if(reCalcNormals.empty() == false || tiny_normals.empty() == false)
		{
			bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::N;
			stride += sizeof(Math::Vector3);
		}

		if(tangents.empty() == false && binormals.empty() == false)
		{
			bufferFlag |= (uint)RenderManager::DefaultVertexInputTypeFlag::TB;
			stride += sizeof(Math::Vector3) * 2;
		}
	}

	void* bufferHead = malloc(vtxCount * stride);
	{
		void* buffer = bufferHead;
		uint uvIndex = 0;
		for(uint posIndex = 0; posIndex < tiny_positions.size(); posIndex+=3, uvIndex+=2)
		{
			#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value, buf = (type*)buf + 1

			//position
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 0]);
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 1]);
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 2]);

			//texcoord
			if(bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::UV)
			{
				INSERT_BUFFER_DATA(float, buffer, tiny_texcoords[uvIndex + 0]);
				INSERT_BUFFER_DATA(float, buffer, tiny_texcoords[uvIndex + 1]);
			}

			//normal
			if(bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::N)
			{
				if((bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::TB) && 
					reCalcNormals.empty() == false)
				{
					INSERT_BUFFER_DATA(Math::Vector3, buffer, reCalcNormals[posIndex / 3]);
				}
				else if(tiny_normals.empty() == false)
				{
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 0]);
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 1]);
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 2]);
				}
			}
			//tangent, binormal
			if(bufferFlag & (uint)RenderManager::DefaultVertexInputTypeFlag::TB)
			{
				INSERT_BUFFER_DATA(Math::Vector3, buffer, tangents[posIndex / 3]);
				INSERT_BUFFER_DATA(Math::Vector3, buffer, binormals[posIndex / 3]);
			}
		}
	}

	Core::Object* object = new Core::Object(shapeName);

	Mesh::Mesh* mesh = object->AddComponent<Mesh::Mesh>();
	resourceManager->GetBufferManager()->Add(fileName, shapeName, bufferHead);

	auto scene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = scene->GetMaterialManager();
	Material* material = materialMgr->Find(fileName, tinyMtl.name);
	ASSERT_COND_MSG(material, "can not found material");

	Mesh::Mesh::CreateFuncArguments args(fileName, shapeName);
	{
		args.vertex.data		= bufferHead;
		args.vertex.count		= vtxCount;
		args.vertex.byteWidth	= stride;

		args.index.data			= indices.data();
		args.index.count		= indices.size();
		args.index.byteWidth	= 0; // not use

		args.material			= material;
		args.isDynamic			= isDynamicMesh;
		args.bufferFlag			= bufferFlag;
	}
	
	mesh->Initialize(args);

	const std::string objKey = fileName + ':' + shapeName;
	resourceManager->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

void ObjImporter::CheckCorrectShape(const tinyobj::shape_t& tinyShape)
{
	if((tinyShape.mesh.indices.size() % 3) != 0)
		ASSERT_MSG("Indices must be made only 3 units.");

	if((tinyShape.mesh.positions.size() % 3) != 0)
		ASSERT_MSG("Pos count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");
}

Core::Object* ObjImporter::CloneOriginObject(const std::string& fileName, const std::string& tinyShapeName)
{
	const ResourceManager* resourceManager = ResourceManager::GetInstance();
	const Core::Object* object = resourceManager->GetOriginObjectManager()->Find(fileName + ':' + tinyShapeName);

	return object ? object->Clone() : nullptr;
}