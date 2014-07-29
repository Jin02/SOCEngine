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
			MeshFilter* _filter;

		public:
			Mesh() : _filter(nullptr)
			{
			}

			~Mesh()
			{
			}

		public:
		};
	}
}