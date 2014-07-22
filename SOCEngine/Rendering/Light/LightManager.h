#pragma once

#include "LightForm.h"
#include "Frustum.h"
#include "Common.h"
#include <vector>

namespace Rendering
{
	namespace Light
	{
		class LightManager
		{
		private:
			std::vector<LightForm*> _lights;

		public:
			LightManager(void);
			~LightManager(void);

		public:
			bool Intersects(std::vector<LightForm*>& outLights, Rendering::Frustum *frustum);
			bool Intersects(Rendering::Frustum *frustum, const LightForm* light);

		public:
			bool AddLight(LightForm* light);
			void Delete(LightForm* light, bool remove);
			void DeleteAll(bool remove);
		};
	}
}