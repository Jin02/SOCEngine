#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"

using namespace Importer;
using namespace Rendering::Material;
using namespace Rendering::Mesh;
using namespace Rendering::Texture;

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

	Core::Object* parent = new Core::Object;
	parent->SetName(fileName);

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		if((iter->mesh.indices.size() % 3) != 0)
			ASSERT("Indices must be made only 3 units.");

		Core::Object* child = new Core::Object(parent);
		child->SetName(iter->name);

		Mesh* mesh = child->AddComponent<Mesh>();

		if((iter->mesh.indices.size() % 3) != 0)
			ASSERT("Index count must has 3 units. ex:) [x1,y1,z1,x2,y2,z2] ");

		iter->mesh.indices;

		iter->mesh.material_ids;

		iter->mesh.normals;
		iter->mesh.positions;
		iter->mesh.texcoords;
	}

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
				Utility::ParseDirectory(iter->ambient_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateAmbientMap(texture);
			}

			if(iter->diffuse_texname.empty() == false)
			{
				Utility::ParseDirectory(iter->diffuse_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateDiffuseMap(texture);
			}

			if(iter->normal_texname.empty() == false)
			{
				Utility::ParseDirectory(iter->normal_texname, nullptr, &fileName, &extension);
				Texture* texture = textureMgr->LoadTextureFromFile(materialFileFolder + iter->ambient_texname + extension, materialName + fileName);
				material->UpdateNormalMap(texture);
			}

			if(iter->specular_texname.empty() == false)
			{
				Utility::ParseDirectory(iter->specular_texname, nullptr, &fileName, &extension);
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

	return nullptr;
}