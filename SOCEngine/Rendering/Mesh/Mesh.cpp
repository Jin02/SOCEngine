#include "Mesh.h"
#include "Director.h"

namespace Rendering
{
	namespace Mesh
	{
		Mesh::Mesh() : _filter(nullptr), _renderer(nullptr)
		{
		}

		Mesh::~Mesh()
		{
		}

		bool Mesh::Create(const std::vector<const void*>& vbDatas, unsigned int vertexBufferSize, std::vector<ENGINE_INDEX_TYPE>& indices, Material::Material* material, bool isDynamic)
		{
			_filter = new MeshFilter;
			if(_filter->CreateBuffer(vbDatas, vertexBufferSize, indices, isDynamic) == false)
			{
				SAFE_DELETE(_filter);
				return false;
			}

			_renderer = new MeshRenderer;
			if(_renderer->AddMaterial(material, false) == false)
				return false;

			return true;
		}
	}
}