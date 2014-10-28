#include "ObjImporter.h"
#include "Utility.h"
#include "MaterialManager.h"
#include "Director.h"

using namespace Importer;
using namespace Rendering::Material;
using namespace Rendering::Mesh;

ObjImporter::ObjImporter()
{
}

ObjImporter::~ObjImporter()
{
}

Core::Object* ObjImporter::Load(const std::string& fileFolderPath, const std::string& fileNameWithExtension)
{
	std::vector<tinyobj::shape_t> shapes;
	std::string error = tinyobj::LoadObj(shapes, (fileFolderPath+fileNameWithExtension).data());

	if(error.empty() == false)
		return nullptr;

	std::string fileName;
	{
		std::vector<std::string> tokens;
		Utility::Tokenize(fileNameWithExtension, tokens, ".");
		fileName = tokens[0];
	}

	Core::Object* parent = new Core::Object;
	parent->SetName(fileName);

	MaterialManager* materialMgr = Device::Director::GetInstance()->GetCurrentScene()->GetMaterialManager();

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		Core::Object* child = new Core::Object;
		child->SetName(iter->name);
		Mesh* mesh = child->AddComponent<Mesh>();

		const unsigned int num = iter->mesh.positions.size() / 3;
		int stride = (sizeof(Math::Vector3) * 2 + sizeof(Math::Vector2));
						// Position, Normal				  TexCoord
		size_t size = num * stride;

		const void* vertexBufferData = nullptr;
		{
			void* buffer = malloc(size);
		
			const std::vector<float>& positions = iter->mesh.positions;
			const std::vector<float>& normals = iter->mesh.normals;
			const std::vector<float>& texcoords = iter->mesh.texcoords;

#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value; buf = (type*)buffer + 1; 

			unsigned int vec3Idx = 0;
			unsigned int vec2Idx = 0;
			for(unsigned int i = 0; i < num; ++i)
			{
				INSERT_BUFFER_DATA(float, buffer, positions[vec3Idx + 0]);
				INSERT_BUFFER_DATA(float, buffer, positions[vec3Idx + 1]);
				INSERT_BUFFER_DATA(float, buffer, positions[vec3Idx + 2]);

				INSERT_BUFFER_DATA(float, buffer, normals[vec3Idx + 0]);
				INSERT_BUFFER_DATA(float, buffer, normals[vec3Idx + 1]);
				INSERT_BUFFER_DATA(float, buffer, normals[vec3Idx + 2]);
				vec3Idx += 3;

				INSERT_BUFFER_DATA(float, buffer, texcoords[vec2Idx + 0]);
				INSERT_BUFFER_DATA(float, buffer, texcoords[vec2Idx + 1]);
				vec2Idx += 2;
			}

			vertexBufferData = buffer;
		}

		const tinyobj::material_t& objMaterial = iter->material;
		const std::string& materialName = objMaterial.name;

		Material* material = materialMgr->Find(materialName);
		if( material == nullptr )
		{
			Material::Color lightColor;
			lightColor.diffuse.SetColor(objMaterial.diffuse);
			lightColor.specular.SetColor(objMaterial.specular);
			lightColor.emissive.SetColor(objMaterial.emission);
			lightColor.ambient.SetColor(objMaterial.ambient);
			lightColor.shiness = objMaterial.shininess;
			lightColor.opacity = objMaterial.dissolve;

			material = new Material(materialName, lightColor);
			materialMgr->Add(fileName+":"+materialName, material, false);
		}

		objMaterial.diffuse_texname;
		objMaterial.normal_texname;
		objMaterial.specular_texname;

		const std::vector<unsigned int>& indices = iter->mesh.indices;
		mesh->Create(vertexBufferData, num, size, indices.data(), indices.size(), nullptr, false);
	}

	return nullptr;
}