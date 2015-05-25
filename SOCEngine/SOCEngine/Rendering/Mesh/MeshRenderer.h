#pragma once

#include "Material.h"
#include "Structure.h"
#include "TransformPipelineParam.h"

namespace Rendering
{
	namespace Mesh
	{
		class MeshRenderer
		{
		private:
			Structure::Vector<std::string, Material>	_materials;

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool AddMaterial(Material* material, bool copy = false);
			void ClassifyMaterialWithMesh(void* mesh);

		public:
			GET_ACCESSOR(MaterialCount, unsigned int, _materials.GetSize());
		};
	}
}