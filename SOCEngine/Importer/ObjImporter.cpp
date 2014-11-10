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

Material* ObjImporter::LoadMaterial(const tinyobj::material_t& tinyMaterial, const std::string& fileName, const std::string& materialFileFolder)
{
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = currentScene->GetMaterialManager();
	TextureManager* textureMgr = currentScene->GetTextureManager();

	const std::string materialName = tinyMaterial.name;
	if( materialName.empty() )
		ASSERT("Material has not key");

	Material* material = materialMgr->Find(fileName, materialName);

	if(material == nullptr)
	{
		Material::Color color;
		color.ambient.SetColor(tinyMaterial.ambient);
		color.diffuse.SetColor(tinyMaterial.diffuse);
		color.specular.SetColor(tinyMaterial.specular);
		color.emissive.SetColor(tinyMaterial.emission);
		color.shiness = tinyMaterial.shininess;
		color.opacity = tinyMaterial.dissolve;

		std::string fileName, extension;
		material = new Material(materialName,  color);

		if(tinyMaterial.ambient_texname.empty() == false)
		{
			String::ParseDirectory(tinyMaterial.ambient_texname, nullptr, &fileName, &extension);
			Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.ambient_texname + extension, materialName + fileName);
			material->UpdateAmbientMap(texture);
		}

		if(tinyMaterial.diffuse_texname.empty() == false)
		{
			String::ParseDirectory(tinyMaterial.diffuse_texname, nullptr, &fileName, &extension);
			Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.ambient_texname + extension, materialName + fileName);
			material->UpdateDiffuseMap(texture);
		}

		if(tinyMaterial.normal_texname.empty() == false)
		{
			String::ParseDirectory(tinyMaterial.normal_texname, nullptr, &fileName, &extension);
			Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.ambient_texname + extension, materialName + fileName);
			material->UpdateNormalMap(texture);
		}

		if(tinyMaterial.specular_texname.empty() == false)
		{
			String::ParseDirectory(tinyMaterial.specular_texname, nullptr, &fileName, &extension);
			Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + tinyMaterial.ambient_texname + extension, materialName + fileName);
			material->UpdateSpecularMap(texture);
		}

		Rendering::Shader::VertexShader* vs = nullptr;			
		Rendering::Shader::PixelShader*	 ps = nullptr;

		Rendering::Shader::ShaderManager* shaderMgr = currentScene->GetShaderManager();
		if(tinyMaterial.normal_texname.empty())
		{
			vs = shaderMgr->FindVertexShader(BASIC_SHADER_NAME, BASIC_VS_MAIN_FUNC_NAME);
			ps = shaderMgr->FindPixelShader(BASIC_SHADER_NAME, BASIC_PS_MAIN_FUNC_NAME);
		}
		else
		{
			vs = shaderMgr->FindVertexShader(BASIC_NORMAL_MAPPING_SHADER_NAME, BASIC_VS_MAIN_FUNC_NAME);
			ps = shaderMgr->FindPixelShader(BASIC_NORMAL_MAPPING_SHADER_NAME, BASIC_PS_MAIN_FUNC_NAME);
		}

		material->SetVertexShader(vs);
		material->SetPixelShader(ps);
		materialMgr->Add(fileName, materialName, material, false);
	}
	else
	{
		DEBUG_LOG("Material Manager already has new mateiral. Please check key from new material");
	}

	return material;
}

void ObjImporter::LoadMaterials(Structure::BaseStructure<Material>** outMaterials, const std::vector<tinyobj::material_t>& tinyMaterials, const std::string& fileName, const std::string& materialFileFolder)
{
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();

	MaterialManager* materialMgr = currentScene->GetMaterialManager();
	TextureManager* textureMgr = currentScene->GetTextureManager();
	for(auto iter = tinyMaterials.begin(); iter != tinyMaterials.end(); ++iter)
	{
		Material* material = LoadMaterial((*iter), fileName, materialFileFolder);
		if(material && outMaterials)
			(*outMaterials)->Add(iter->name, material);
	}
}

Core::Object* ObjImporter::Load(const std::string& fileDir, const std::string& fileName, const std::string& materialFileFolder, bool isDynamicMesh)
{
	std::vector<tinyobj::shape_t>		shapes;
	std::vector<tinyobj::material_t>	materials;

	std::string error = tinyobj::LoadObj(shapes, materials, fileDir.c_str(), materialFileFolder.c_str());

	if( error.empty() == false )
	{
		DEBUG_LOG(error.c_str());
		return nullptr;
	}

	LoadMaterials(nullptr, materials, fileName, materialFileFolder);

	Core::Object* parent = new Core::Object;
	parent->SetName(fileName);

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		Core::Object* child = LoadMesh((*iter), materials, fileName, isDynamicMesh);
		parent->AddObject(child, false);
	}

	return parent;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& tinyShape, const std::vector<tinyobj::material_t>& tinyMaterials, const std::string& fileName, const std::vector<CustomSemantic>& customSemanticData, Rendering::Material::Material* material, bool isDynamicMesh)
{
	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, tinyShape.name);
		if(obj)
			return obj;
	}

	CheckCorrectShape(tinyShape);

	Core::Object* object = new Core::Object;
	object->SetName(tinyShape.name);

	const std::vector<float>& positions		 = tinyShape.mesh.positions;
	const std::vector<float>& texcoords		 = tinyShape.mesh.texcoords;
	const std::vector<float>& normals		 = tinyShape.mesh.normals;
	const std::vector<unsigned int>& indices = tinyShape.mesh.indices;

	const tinyobj::material_t& objMtl = tinyMaterials[ tinyShape.mesh.material_ids[0] ];
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

	auto& semanticInfos = material->GetVertexShader()->GetSemanticInfos();

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

	unsigned int vtxCount = tinyShape.mesh.positions.size() / 3;
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
	
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();
	MaterialManager* materialMgr = currentScene->GetMaterialManager();

	Mesh* mesh = object->AddComponent<Mesh>();

	LPVoidType* bufferData = new LPVoidType(buffer);
	currentScene->GetBufferManager()->Add(fileName, tinyShape.name, bufferData);

	mesh->Create(bufferData, vtxCount, stride, indices.data(), indices.size(), material, isDynamicMesh);

	const std::string objKey = fileName + ':' + tinyShape.name;
	currentScene->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

Core::Object* ObjImporter::LoadMesh(const tinyobj::shape_t& tinyShape, const std::vector<tinyobj::material_t>& tinyMaterials, const std::string& fileName, bool isDynamicMesh)
{
	//이전에 이미 로드되어 있는 오브젝트라면, 복사해서 리턴함
	{
		Core::Object* obj = CloneOriginObject(fileName, tinyShape.name);
		if(obj)
			return obj;
	}

	CheckCorrectShape(tinyShape);

	Core::Object* object = new Core::Object;
	object->SetName(tinyShape.name);

	const std::vector<float>& positions		 = tinyShape.mesh.positions;
	const std::vector<float>& texcoords		 = tinyShape.mesh.texcoords;
	const std::vector<float>& normals		 = tinyShape.mesh.normals;
	const std::vector<unsigned int>& indices = tinyShape.mesh.indices;

	const tinyobj::material_t& objMtl = tinyMaterials[ tinyShape.mesh.material_ids[0] ];
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

	unsigned int vtxCount = tinyShape.mesh.positions.size() / 3;
	unsigned int stride = 0;
	{
		stride += sizeof(Math::Vector3);									//position
		stride += (texcoords.empty() == false) ? sizeof(Math::Vector2) : 0;	//texcoord
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

	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();
	MaterialManager* materialMgr = currentScene->GetMaterialManager();

	Rendering::Material::Material* material = materialMgr->Find(fileName, objMtl.name);
	Mesh* mesh = object->AddComponent<Mesh>();

	LPVoidType* bufferData = new LPVoidType(buffer);
	currentScene->GetBufferManager()->Add(fileName, tinyShape.name, bufferData);

	mesh->Create(bufferData, vtxCount, stride, indices.data(), indices.size(), material, isDynamicMesh);

	const std::string objKey = fileName + ':' + tinyShape.name;
	currentScene->GetOriginObjectManager()->Add(objKey, object);

	return object;
}

void ObjImporter::CheckCorrectShape(const tinyobj::shape_t& tinyShape)
{
	if((tinyShape.mesh.indices.size() % 3) != 0)
		ASSERT("Indices must be made only 3 units.");

	if((tinyShape.mesh.positions.size() % 3) != 0)
		ASSERT("Pos count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");
}

Core::Object* ObjImporter::CloneOriginObject(const std::string& fileName, const std::string& tinyShapeName)
{
	Core::Scene* currentScene = Device::Director::GetInstance()->GetCurrentScene();	
	const Core::Object* object = currentScene->GetOriginObjectManager()->Find(fileName + ':' + tinyShapeName);
	if(object)
	{
		Core::Object* copyObj = Core::Object::Copy(object);
		copyObj->SetName( object->GetName() + "-Clone");
		
		return copyObj;
	}

	return nullptr;
}