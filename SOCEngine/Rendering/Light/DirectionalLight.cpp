#include "DirectionalLight.h"

namespace Rendering
{
	namespace Light
	{
		DirectionalLight::DirectionalLight() : LightForm()
		{
			type = DIRECTIONAL;
		}

		DirectionalLight::~DirectionalLight()
		{
		}

		bool DirectionalLight::Intersect(Intersection::Sphere &sphere)
		{
			return true;
		}
	}
}