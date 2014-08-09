#pragma once

#include "Material.h"
#include "Structure.h"

namespace Rendering
{
	namespace Material
	{
		class MaterialManager : public Structure::HashMap<Material>
		{
		public:
			MaterialManager() : Structure::HashMap<Material>(){}
			~MaterialManager(){}
		};
	}
}