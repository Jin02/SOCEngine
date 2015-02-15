#pragma once

#include "LightForm.h"
#include "Frustum.h"
#include "Common.h"
#include <vector>

namespace Rendering
{
	namespace Manager
	{
		class LightManager
		{
		private:
			std::vector<Light::LightForm*> _lights;

		public:
			LightManager(void);
			~LightManager(void);

		};
	}
}