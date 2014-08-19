#include "ObjImporter.h"
#include "Utility.h"

using namespace Importer;
using namespace Rendering::Material;
using namespace Rendering::Mesh;

ObjImporter::ObjImporter()
{
}

ObjImporter::~ObjImporter()
{
}

Core::Object* ObjImporter::Load(const std::string& fileFolderPath, const std::string& fileNameWithExtension, bool useNormalMap)
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

	for(auto iter = shapes.begin(); iter != shapes.end(); ++iter)
	{
		Core::Object* child = new Core::Object;
		child->SetName(iter->name);
		Mesh* mesh = child->AddComponent<Mesh>();

		{
			unsigned int indexCount = iter->mesh.indices.size();
			const std::vector<unsigned int>& indices = iter->mesh.indices;

			unsigned int num = iter->mesh.positions.size();
			int stride = useNormalMap == false ?
						(sizeof(Math::Vector3) * 2 + sizeof(Math::Vector2))
						// Position, Normal				  TexCoord
						: (sizeof(Math::Vector3) * 4 + sizeof(Math::Vector2));
						// Position, Normal,			  TexCoord
						// Tangetnt, Binormal
			size_t size = num * stride;
			void* buffer = malloc(size);

			unsigned int count = num / 3;
			for(unsigned int i = 0; i < count; ++i)
			{
#define INSERT_BUFFER_DATA(type, buf, value) *((type*)buf) = value; 

				*((float*)buffer) = 10.0f;
				buffer = (float*)buffer + 1;
			}
			iter->mesh.normals;
			iter->mesh.positions;
			iter->mesh.texcoords;
		}
	}

	return nullptr;
}