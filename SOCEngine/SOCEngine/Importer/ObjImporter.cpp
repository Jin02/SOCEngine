#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"
#include "ImporterUtility.h"

#include "LPVoidType.h"
#include "RenderManager.h"

#include "ResourceManager.h"

using namespace Importer;
using namespace Importer::Obj;
using namespace Rendering;
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

	MaterialManager* materialMgr = resourceMgr->GetMaterialManager();
	TextureManager* textureMgr = resourceMgr->GetTextureManager();

	const std::string materialName = tinyMaterial.name;
	ASSERT_COND_MSG(materialName.empty() == false, "Material has not key");

	Material* material = materialMgr->Find(fileName, materialName);

	if(material == nullptr)
	{
		material = new Material(materialName, materialType);

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
			Texture::Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.diffuse_texname, false);

			const unsigned int shaderSlotIndex = 0; // Default Diffuse Texture Shader Slot Index
			material->UpdateTexture_ShaderSlotIndex(shaderSlotIndex, texture);
		}

		if(tinyMaterial.normal_texname.empty() == false)
		{
			Texture::Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.normal_texname, false);

			const unsigned int shaderSlotIndex = 1; // Default Diffuse Texture Shader Slot Index
			material->UpdateTexture_ShaderSlotIndex(shaderSlotIndex, texture);
		}

		if(tinyMaterial.specular_texname.empty() == false)
		{
			Texture::Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.specular_texname, false);

			const unsigned int shaderSlotIndex = 2; // Default Diffuse Texture Shader Slot Index
			material->UpdateTexture_ShaderSlotIndex(shaderSlotIndex, texture);
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

	MaterialManager* materialMgr = resourceMgr->GetMaterialManager();
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

	if( error.empty() == false )
	{
		DEBUG_LOG(error.c_str());
		return nullptr;
	}

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
	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, tinyShape.name);
		if(obj)
			return obj;
	}

	if(semanticInfos.empty())
		ASSERT_MSG("Error, ObjImporter : semanticInfos paramater is null");

	const ResourceManager* resourceManager = ResourceManager::GetInstance();

	if(resourceManager->GetBufferManager()->Find( (LPVoidType*)nullptr, fileName, tinyShape.name ))
		ASSERT_MSG("Error, BufferMgr already has buffer");

	CheckCorrectShape(tinyShape);

	const std::vector<float>& tiny_positions		 = tinyShape.mesh.positions;
	const std::vector<float>& tiny_texcoords		 = tinyShape.mesh.texcoords;
	const std::vector<float>& tiny_normals			 = tinyShape.mesh.normals;
	const std::vector<unsigned int>& indices		 = tinyShape.mesh.indices;

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
		unsigned int offset;

		InputSemanticData(const CustomSemantic& data) : customData(data), offset(0){}
	};

	uint stride = sizeof(Math::Vector3); //pos
	Mesh::MeshFilter::BufferElementFlag bufferFlag = 0;

	std::vector<InputSemanticData> vertexDatas;
	{
		for(auto iter = semanticInfos.begin(); iter != semanticInfos.end(); ++iter)
		{
			if(iter->name == "NORMAL")
			{
				stride += sizeof(Math::Vector3);
				bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::Normal;
			}
			else if(iter->name == "TEXCOORD0")
			{
				stride += sizeof(Math::Vector2);
				bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::UV;
			}
			else if(iter->name == "TANGENT")
			{
				stride += sizeof(Math::Vector3);
				bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::Tangent;
			}
			else if(iter->name == "BINORMAL")
			{
				stride += sizeof(Math::Vector3);
				bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::Binormal;
			}
		}

		vertexDatas.push_back(CustomSemantic("POSITION", tiny_positions.data()));
		if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Normal) )
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

		if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Tangent) &&
			tangents.empty() == false)
			vertexDatas.push_back(CustomSemantic("TANGENT",	tangents.data()));

		if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Binormal) &&
			binormals.empty() == false)
			vertexDatas.push_back(CustomSemantic("BINORMAL", binormals.data()));

		if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::UV) &&
			tiny_texcoords.empty() == false)
			vertexDatas.push_back(CustomSemantic("TEXCOORD0", tiny_texcoords.data()));
	}

	unsigned int vtxCount = tinyShape.mesh.positions.size() / 3;
	void* bufferHead = malloc(vtxCount * stride);
	{
		void* buffer = bufferHead;
		for(unsigned int vtxIndex = 0; vtxIndex < vtxCount; ++vtxIndex)
		{
			for(auto infoIter = semanticInfos.begin(); infoIter != semanticInfos.end(); ++infoIter)
			{
				const std::string& name = infoIter->name;
				unsigned int size = infoIter->size;

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
	
	Core::Object* object = new Core::Object(tinyShape.name);

	Mesh::Mesh* mesh = object->AddComponent<Mesh::Mesh>();

	LPVoidType bufferData(bufferHead);
	resourceManager->GetBufferManager()->Add(fileName, tinyShape.name, bufferData);

	MaterialManager* materialMgr = resourceManager->GetMaterialManager();
	Material* material = materialMgr->Find(fileName, tinyMtl.name);
	ASSERT_COND_MSG(material, "can not found material");

	//sizeof(ENGINE_INDEX_TYPE) * _indexCount

	Mesh::Mesh::CreateFuncArguments args(fileName, tinyShape.name);
	{
		args.vertex.data		= bufferData.GetBuffer();
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

	const std::string objKey = fileName + ':' + tinyShape.name;
	resourceManager->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& tinyShape,
									const tinyobj::material_t& tinyMtl,
									const std::string& fileName, 
									Rendering::Material::Type materialType,
									bool isDynamicMesh)
{
	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, tinyShape.name);
		if(obj)
			return obj;
	}

	ResourceManager* resourceManager = ResourceManager::GetInstance();
	if(resourceManager->GetBufferManager()->Find( (LPVoidType*)nullptr, fileName, tinyShape.name ))
		ASSERT_MSG("Error, BufferMgr already has buffer");

	CheckCorrectShape(tinyShape);

	const std::vector<float>& tiny_positions		 = tinyShape.mesh.positions;
	const std::vector<float>& tiny_texcoords		 = tinyShape.mesh.texcoords;
	const std::vector<float>& tiny_normals			 = tinyShape.mesh.normals;
	const std::vector<unsigned int>& indices		 = tinyShape.mesh.indices;

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

	unsigned int vtxCount = tinyShape.mesh.positions.size() / 3;
	unsigned int stride = 0;
	Rendering::Mesh::MeshFilter::BufferElementFlag bufferFlag = 0;
	{
		stride += sizeof(Math::Vector3);

		if(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::UV)
		{
			bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::UV;
			stride += sizeof(Math::Vector2);	//texcoord
		}

		if(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Normal)
		{
			bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::Normal;
			stride += sizeof(Math::Vector3);
		}

		if(isNormalMapUse)
		{
			if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Binormal) && 
				(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Tangent))
			{
				bufferFlag |= (uint)Mesh::MeshFilter::BufferElement::Binormal | (uint)Mesh::MeshFilter::BufferElement::Tangent;
				stride += sizeof(Math::Vector3) * 2;
			}
		}
	}

	void* bufferHead = malloc(vtxCount * stride);
	{
		void* buffer = bufferHead;
		unsigned int uvIndex = 0;
		for(unsigned int posIndex = 0; posIndex < tiny_positions.size(); posIndex+=3, uvIndex+=2)
		{
			#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value, buf = (type*)buf + 1

			//position
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 0]);
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 1]);
			INSERT_BUFFER_DATA(float, buffer, tiny_positions[posIndex + 2]);

			//texcoord
			if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::UV)
				&& tiny_texcoords.empty() == false)
			{
				INSERT_BUFFER_DATA(float, buffer, tiny_texcoords[uvIndex + 0]);
				INSERT_BUFFER_DATA(float, buffer, tiny_texcoords[uvIndex + 1]);
			}

			//normal
			if(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Normal)
			{
				if((isNormalMapUse == false && tiny_normals.empty() == false) )
				{
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 0]);
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 1]);
					INSERT_BUFFER_DATA(float, buffer, tiny_normals[posIndex + 2]);
				}
				else if((isNormalMapUse && reCalcNormals.empty() == false) )
				{
					INSERT_BUFFER_DATA(Math::Vector3, buffer, reCalcNormals[posIndex / 3]);
				}
			}
			//tangent, binormal
			if( (bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Binormal) && 
				(bufferFlag & (uint)Mesh::MeshFilter::BufferElement::Tangent) &&
				( (tangents.empty() == false) && (binormals.empty() == false) ))
			{
				INSERT_BUFFER_DATA(Math::Vector3, buffer, tangents[posIndex / 3]);
				INSERT_BUFFER_DATA(Math::Vector3, buffer, binormals[posIndex / 3]);
			}
		}
	}

	Core::Object* object = new Core::Object(tinyShape.name);

	Mesh::Mesh* mesh = object->AddComponent<Mesh::Mesh>();
	
	LPVoidType bufferData(bufferHead);
	resourceManager->GetBufferManager()->Add(fileName, tinyShape.name, bufferData);

	MaterialManager* materialMgr = resourceManager->GetMaterialManager();
	Material* material = materialMgr->Find(fileName, tinyMtl.name);
	ASSERT_COND_MSG(material, "can not found material");

	const std::string bufferKey = fileName + ':' + tinyShape.name;

	Mesh::Mesh::CreateFuncArguments args(fileName, tinyShape.name);
	{
		args.vertex.data		= bufferData.GetBuffer();
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
	resourceManager->GetOriginObjectManager()->Add(bufferKey, object);

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