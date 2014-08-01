#pragma once

#include "MeshFilter.h"
#include "MeshRenderer.h"

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
			Mesh() : _filter(nullptr), _renderer(nullptr)
			{
			}

			~Mesh()
			{
			}

		public:
			bool Create()
			{
				_filter = new MeshFilter;
				_renderer = new MeshRenderer;


				_filter->CreateBuffer(vbData, vbCount, vbSize, indices, indicesCount, dynamicMesh);

				 
				return true;
			}

		public:
			GET_ACCESSOR(MeshFilter, MeshFilter*, _filter);
			GET_ACCESSOR(MeshRenderer, MeshRenderer*, _renderer);
		};
	}
}