#pragma once

#include "MeshFilter.h"
#include "MeshRenderer.h"
#include "MeshBasicData.h"

namespace Rendering
{
	namespace Mesh
	{
		class Mesh
		{
		private:
			MeshFilter*			_filter;
			MeshRenderer*		_renderer;

		public:
			Mesh();
			~Mesh();

		public:
			bool Create(const std::vector<const void*>& vbDatas, unsigned int vertexBufferSize, std::vector<ENGINE_INDEX_TYPE>& indices, Material::Material* material, bool isDynamic);
			bool Test()
			{


				return true;
			}

		public:
			GET_ACCESSOR(MeshFilter, MeshFilter*, _filter);
			GET_ACCESSOR(MeshRenderer, MeshRenderer*, _renderer);
		};
	}
}