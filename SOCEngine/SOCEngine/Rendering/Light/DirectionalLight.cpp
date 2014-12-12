#include "DirectionalLight.h"

namespace Rendering
{
	namespace Light
	{
		DirectionalLight::DirectionalLight() : LightForm()
		{
			_type = LightType::Directional;
		}

		DirectionalLight::~DirectionalLight()
		{
		}

		bool DirectionalLight::Intersects(const Intersection::Sphere &sphere)
		{
			return true;
		}
	}
}