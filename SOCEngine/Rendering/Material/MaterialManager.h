#pragma once

#include "Dictionary.h"
#include "Material.h"

namespace Rendering
{
	namespace Material
	{
		class MaterialManager : public Dictionary<Material>
		{
		};

	}
}