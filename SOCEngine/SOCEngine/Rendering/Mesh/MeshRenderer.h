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
				Opaque,
				Transparent,
				AlphaBlend,
				Unknown,
				MAX = Unknown
			};

		private:
			std::vector<const Material*>		_materials;
			bool								_useAlphaBlending;

		public:
			MeshRenderer();
			~MeshRenderer();

		public:
			bool HasMaterial(const Material* material) const;
			bool AddMaterial(const Material* material);
			void DeleteMaterial(uint index);

			Type GetCurrentRenderType() const;
			bool IsTransparent() const;
		
		public:
			GET_ACCESSOR(Materials,				const std::vector<const Material*>&,	_materials);
			GET_SET_ACCESSOR(UseAlphaBlending,	bool,									_useAlphaBlending);
		};
	}
}