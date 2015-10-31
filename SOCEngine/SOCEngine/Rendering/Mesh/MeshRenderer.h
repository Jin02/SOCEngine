#pragma once

#include "Material.h"
#include "Structure.h"

namespace Rendering
{
	namespace Geometry
	{
		class MeshRenderer
		{
		public:
			enum class Type
			{
				Unknown,
				Opaque,
				Transparent,
				AlphaBlend,
				OnlyAlphaTestWithDiffuse
			};

		private:
			std::vector<Material*>		_materials;
			bool						_useAlphaBlending;

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool HasMaterial(const Material* material) const;
			bool AddMaterial(Material* material);

			Type GetCurrentRenderType() const;
			bool IsTransparent() const;
		
		public:
			GET_ACCESSOR(Materials, const std::vector<Material*>&, _materials);
			GET_SET_ACCESSOR(UseAlphaBlending, bool, _useAlphaBlending);
		};
	}
}