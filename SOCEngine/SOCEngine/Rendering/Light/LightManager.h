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

		public:
			bool Intersects(std::vector<Light::LightForm*>& outLights, Rendering::Camera::Frustum *frustum);
			bool Intersects(Rendering::Camera::Frustum *frustum, const Light::LightForm* light);

		public:
			bool AddLight(Light::LightForm* light);
			void Delete(Light::LightForm* light, bool remove);
			void DeleteAll(bool remove);
		};
	}
}