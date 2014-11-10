#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"
#include "ImporterUtility.h"

#include "LPVoidType.h"

using namespace Importer;
using namespace Rendering::Material;
using namespace Rendering::Mesh;
using namespace Rendering::Texture;
using namespace Utility;

ObjImporter::ObjImporter()
{
}

ObjImporter::~ObjImporter()
{
}

void ObjImporter::LoadMaterial(const std::vector<tinyobj::material_t>& materials, const std::string& fileDir, const std::string& fileName, const std::string& materialFileFolder)
{
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = currentScene->GetMaterialManager();
	TextureManager* textureMgr = currentScene->GetTextureManager();
	for(auto iter = materials.begin(); iter != materials.end(); ++iter)
	{
		const std::string materialName = iter->name;
		if( materialName.empty() )
			ASSERT("Material has not key");

		Material* material = materialMgr->Find(fileName, materialName);

		if(material == nullptr)
		{
			Material::Color color;
			color.ambient.SetColor(iter->ambient);
			color.diffuse.SetColor(iter->diffuse);
			color.specular.SetColor(iter->specular);
			color.emissive.SetColor(iter->emission);
			color.shiness = iter->shininess;
			color.opacity = iter->dissolve;

			std::string fileName, extension;
			material = new Material(materialName,  color);

			if(iter->ambient_texname.empty() == false)
			{
				String::ParseDirectory(iter->ambient_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateAmbientMap(texture);
			}

			if(iter->diffuse_texname.empty() == false)
			{
				String::ParseDirectory(iter->diffuse_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateDiffuseMap(texture);
			}

			if(iter->normal_texname.empty() == false)
			{
				String::ParseDirectory(iter->normal_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateNormalMap(texture);
			}

			if(iter->specular_texname.empty() == false)
			{
				String::ParseDirectory(iter->specular_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateSpecularMap(texture);
			}

			Rendering::Shader::ShaderManager* shaderMgr = currentScene->GetShaderManager();
			//기본 쉐이더 읽기 필요
			shaderMgr->LoadVertexShader();
			shaderMgr->LoadPixelShader();
			materialMgr->Add(fileName, materialName, material, false);
		}
		else
		{
			ASSERT("Material Manager already has new mateiral. Please check key from new material");
		}
	}
}

Core::Object* ObjImporter::Load(const std::string& fileDir, const std::string& fileName, const std::string& materialFileFolder, const Rendering::Shader::VertexShader* vs)
{
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;

	std::string error = tinyobj::LoadObj(shapes, materials, fileDir.c_str(), materialFileFolder.c_str());

	if( error.empty() == false )
	{
		DEBUG_LOG(error.c_str());
		return nullptr;
	}

	Core::Object* parent = new Core::Object;
	parent->SetName(fileName);

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		if((iter->mesh.indices.size() % 3) != 0)
			ASSERT("Indices must be made only 3 units.");

		if((iter->mesh.positions.size() % 3) != 0)
			ASSERT("Pos count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");

		Core::Object* child = new Core::Object(parent);
		child->SetName(iter->name);



		std::vector<float>& positions		= iter->mesh.positions;
		std::vector<float>& texcoords				= iter->mesh.texcoords;
		std::vector<float>& normals			= iter->mesh.normals;
		std::vector<unsigned int>& indices	= iter->mesh.indices;

		{
			//여기서 탄젠트랑 바이노멀 둘 다 계산가능하게 되었어.
			std::vector<Math::Vector3> tangents;
			std::vector<Math::Vector3> binormals;

			tinyobj::material_t& material = materials[ iter->mesh.material_ids[0] ];
			bool isNormalMapUse = (material.normal_texname.empty() == false);
			if(isNormalMapUse)
			{
				const Math::Vector3* vertices	= reinterpret_cast<const Math::Vector3*>(positions.data());
				const Math::Vector3* normals	= reinterpret_cast<const Math::Vector3*>(positions.data());
				const Math::Vector2* texcoords	= reinterpret_cast<const Math::Vector2*>(positions.data());

				Utility::CalculateTangentBinormal(tangents, binormals, vertices, positions.size() / 3, normals, texcoords, indices.data(), indices.size());
			}
		}

		unsigned int vertexCount = iter->mesh.positions.size() / 3;
		unsigned int stride = 0;

		void* buffer = malloc(vertexCount * stride);

		unsigned int uvCount = 0;
		for(unsigned int vtxIndex = 0; vtxIndex < 0; vtxIndex+=3, uvCount+=2)
		{
#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value, buf = (type*)buf + 1

			INSERT_BUFFER_DATA(float, buffer, positions[vtxIndex + 0]);
			INSERT_BUFFER_DATA(float, buffer, positions[vtxIndex + 1]);
			INSERT_BUFFER_DATA(float, buffer, positions[vtxIndex + 2]);

			if(texcoords.empty() == false)
			{
				INSERT_BUFFER_DATA(float, buffer, texcoords[uvCount + 0]);
				INSERT_BUFFER_DATA(float, buffer, texcoords[uvCount + 1]);
			}

			if(normals.empty() == false)
			{
				INSERT_BUFFER_DATA(float, buffer, normals[vtxIndex + 0]);
				INSERT_BUFFER_DATA(float, buffer, normals[vtxIndex + 1]);
				INSERT_BUFFER_DATA(float, buffer, normals[vtxIndex + 2]);
			}
		}

		Mesh* mesh = child->AddComponent<Mesh>();
		mesh->Create(buffer, vertexCount, stride, indices.data(), indices.size(), 0, 0);
	}



	return nullptr;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& shape,
								const std::vector<tinyobj::material_t>& materials,
								const std::string& fileName,
								const std::vector<CustomSemantic>& customSemanticData,
								const Rendering::Shader::VertexShader* vs,
								bool isDynamicMesh)
{
	const std::string objKey = fileName + ':' + shape.name;
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();
	{
		LPVoidType* lp;
		if(currentScene->GetBufferManager()->Find(lp, fileName, shape.name))
			ASSERT("Already BufferMgr has file");

		const Core::Object* object = currentScene->GetOriginObjectManager()->Find(objKey);
		if(object)
		{
			Core::Object* copyObj = Core::Object::Copy(object);
			copyObj->SetName( object->GetName() + "-Clone");
			return copyObj;
		}
	}

	if((shape.mesh.indices.size() % 3) != 0)
		ASSERT("Indices must be made only 3 units.");

	if((shape.mesh.positions.size() % 3) != 0)
		ASSERT("Pos count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");

	Core::Object* object = new Core::Object;
	object->SetName(shape.name);

	const std::vector<float>& positions		 = shape.mesh.positions;
	const std::vector<float>& texcoords		 = shape.mesh.texcoords;
	const std::vector<float>& normals		 = shape.mesh.normals;
	const std::vector<unsigned int>& indices = shape.mesh.indices;

	const tinyobj::material_t& objMtl = materials[ shape.mesh.material_ids[0] ];
	bool isNormalMapUse = (objMtl.normal_texname.empty() == false);

	std::vector<Math::Vector3> tangents;
	std::vector<Math::Vector3> binormals;

	if(isNormalMapUse)
	{
		const Math::Vector3* vertices	= reinterpret_cast<const Math::Vector3*>(positions.data());
		const Math::Vector3* normals	= reinterpret_cast<const Math::Vector3*>(positions.data());
		const Math::Vector2* texcoords	= reinterpret_cast<const Math::Vector2*>(positions.data());

		Utility::CalculateTangentBinormal(tangents, binormals, vertices, positions.size() / 3, normals, texcoords, indices.data(), indices.size());
	}

	auto& semanticInfos = vs->GetSemanticInfos();

	unsigned int stride = 0;
	{
		for(auto iter = semanticInfos.begin(); iter != semanticInfos.end(); ++iter)
			stride += iter->size;
	}

	struct InputSemanticData
	{
		CustomSemantic customData;
		unsigned int offset;

		InputSemanticData(const CustomSemantic& data) : customData(data), offset(0){}
	};

	std::vector<InputSemanticData> vertexDatas;
	bool isCustomPos		= false;
	bool isCustomNormal		= false;
	bool isCustomTexcoord	= false;
	bool isCustomTangent	= false;
	bool isCustomBinormal	= false;
	{
		// CustomSemantic convert to InputSemanticData
		// And, check custom data
		for(auto iter = customSemanticData.begin(); iter != customSemanticData.end(); ++iter)
		{
			if(iter->semanticName == "POSITION")
				isCustomPos = true;
			else if(iter->semanticName == "NORMAL")
				isCustomNormal = true;
			else if(iter->semanticName == "TEXCOORD0")
				isCustomTexcoord = true;
			else if(iter->semanticName == "TANGENT")
				isCustomTangent = true;
			else if(iter->semanticName == "BINORMAL")
				isCustomBinormal = true;

			vertexDatas.push_back((*iter));
		}
	}

	if(isCustomPos == false)
		vertexDatas.push_back(CustomSemantic("POSITION", positions.data()));

	if(isCustomNormal == false && normals.empty() == false)
		vertexDatas.push_back(CustomSemantic("NORMAL", normals.data()));

	if(tangents.empty() == false && isCustomTangent == false)
		vertexDatas.push_back(CustomSemantic("TANGENT",	tangents.data()));

	if(binormals.empty() == false && isCustomBinormal == false)
		vertexDatas.push_back(CustomSemantic("BINORMAL", binormals.data()));

	if(texcoords.empty() == false && isCustomTexcoord == false)
		vertexDatas.push_back(CustomSemantic("TEXCOORD0", texcoords.data()));

	unsigned int vtxCount = shape.mesh.positions.size() / 3;
	void* buffer = malloc(vtxCount * stride);

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
			if( count == vertexDatas.size() )
				ASSERT("Not Found Semantic Data!");
		}
	}
	
	MaterialManager* materialMgr = currentScene->GetMaterialManager();

	Rendering::Material::Material* material = materialMgr->Find(fileName, objMtl.name);
	Mesh* mesh = object->AddComponent<Mesh>();

	LPVoidType* bufferData = new LPVoidType(buffer);
	currentScene->GetBufferManager()->Add(fileName, shape.name, bufferData);

	mesh->Create(bufferData, vtxCount, stride, indices.data(), indices.size(), material, isDynamicMesh);

	currentScene->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& shape, const std::vector<tinyobj::material_t>& materials, const std::string& fileName, bool isDynamicMesh)
{
	const std::string objKey = fileName + ':' + shape.name;
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();
	{
		LPVoidType* lp;
		if(currentScene->GetBufferManager()->Find(lp, fileName, shape.name))
			ASSERT("Already BufferMgr has file");

		const Core::Object* object = currentScene->GetOriginObjectManager()->Find(objKey);
		if(object)
		{
			Core::Object* copyObj = Core::Object::Copy(object);
			copyObj->SetName( object->GetName() + "-Clone");
			return copyObj;
		}
	}

	if((shape.mesh.indices.size() % 3) != 0)
		ASSERT("Indices must be made only 3 units.");

	if((shape.mesh.positions.size() % 3) != 0)
		ASSERT("Pos count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");

	Core::Object* object = new Core::Object;
	object->SetName(shape.name);

	const std::vector<float>& positions		 = shape.mesh.positions;
	const std::vector<float>& texcoords			 = shape.mesh.texcoords;
	const std::vector<float>& normals		 = shape.mesh.normals;
	const std::vector<unsigned int>& indices = shape.mesh.indices;

	const tinyobj::material_t& objMtl = materials[ shape.mesh.material_ids[0] ];
	bool isNormalMapUse = (objMtl.normal_texname.empty() == false);

	std::vector<Math::Vector3> tangents;
	std::vector<Math::Vector3> binormals;

	if(isNormalMapUse)
	{
		const Math::Vector3* vertices	= reinterpret_cast<const Math::Vector3*>(positions.data());
		const Math::Vector3* normals	= reinterpret_cast<const Math::Vector3*>(positions.data());
		const Math::Vector2* texcoords	= reinterpret_cast<const Math::Vector2*>(positions.data());

		Utility::CalculateTangentBinormal(tangents, binormals, vertices, positions.size() / 3, normals, texcoords, indices.data(), indices.size());
	}

	unsigned int vtxCount = shape.mesh.positions.size() / 3;
	unsigned int stride = 0;
	{
		stride += sizeof(Math::Vector3);									//position
		stride += (texcoords.empty() == false) ? sizeof(Math::Vector2) : 0;		//texcoord
		stride += (normals.empty() == false) ? sizeof(Math::Vector3) : 0;	//normals
		stride += isNormalMapUse ? sizeof(Math::Vector3) * 2 : 0;			//tangent(vector3), binormal(vector3)
	}

	void* buffer = malloc(vtxCount * stride);

	unsigned int uvIndex = 0;
	for(unsigned int posIndex = 0; posIndex < positions.size(); posIndex+=3, uvIndex+=2)
	{
		#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value, buf = (type*)buf + 1

		//position
		INSERT_BUFFER_DATA(float, buffer, positions[posIndex + 0]);
		INSERT_BUFFER_DATA(float, buffer, positions[posIndex + 1]);
		INSERT_BUFFER_DATA(float, buffer, positions[posIndex + 2]);

		//texcoord
		if(texcoords.empty() == false)
		{
			INSERT_BUFFER_DATA(float, buffer, texcoords[uvIndex + 0]);
			INSERT_BUFFER_DATA(float, buffer, texcoords[uvIndex + 1]);
		}

		//normal
		if(normals.empty() == false)
		{
			INSERT_BUFFER_DATA(float, buffer, normals[posIndex + 0]);
			INSERT_BUFFER_DATA(float, buffer, normals[posIndex + 1]);
			INSERT_BUFFER_DATA(float, buffer, normals[posIndex + 2]);
		}

		//tangent, binormal
		if((tangents.empty() == false) && (binormals.empty() == false))
		{
			INSERT_BUFFER_DATA(Math::Vector3, buffer, tangents[posIndex / 3]);
			INSERT_BUFFER_DATA(Math::Vector3, buffer, binormals[posIndex / 3]);
		}
	}

	MaterialManager* materialMgr = currentScene->GetMaterialManager();

	Rendering::Material::Material* material = materialMgr->Find(fileName, objMtl.name);
	Mesh* mesh = object->AddComponent<Mesh>();

	LPVoidType* bufferData = new LPVoidType(buffer);
	currentScene->GetBufferManager()->Add(fileName, shape.name, bufferData);

	mesh->Create(bufferData, vtxCount, stride, indices.data(), indices.size(), material, isDynamicMesh);

	currentScene->GetOriginObjectManager()->Add(objKey, object);

	return object;
}