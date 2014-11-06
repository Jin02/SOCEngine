#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"
#include "ImporterUtility.h"

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

			materialMgr->Add(fileName, materialName, material, false);
		}
		else
		{
			ASSERT("Material Manager already has new mateiral. Please check key from new material");
		}
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

		tinyobj::material_t& material = materials[ iter->mesh.material_ids[0] ];
		bool isNormalMapUse = (material.normal_texname.empty() == false);

		std::vector<float>& positions		= iter->mesh.positions;
		std::vector<float>& uvs				= iter->mesh.texcoords;
		std::vector<float>& normals			= iter->mesh.normals;
		std::vector<unsigned int>& indices	= iter->mesh.indices;
		{
			//여기서 탄젠트랑 바이노멀 둘 다 계산가능하게 되었어.
			std::vector<Math::Vector3> tangents;
			std::vector<Math::Vector3> binormals;
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

			if(uvs.empty() == false)
			{
				INSERT_BUFFER_DATA(float, buffer, uvs[uvCount + 0]);
				INSERT_BUFFER_DATA(float, buffer, uvs[uvCount + 1]);
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