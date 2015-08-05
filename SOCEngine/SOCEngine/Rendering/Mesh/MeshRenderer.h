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
			std::vector<Material*>		_materials;

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool HasMaterial(Material* material);
			bool AddMaterial(Material* material);
			bool IsTransparent();
		
		public:
			GET_ACCESSOR(Materials, const std::vector<Material*>&, _materials);
		};
	}
}