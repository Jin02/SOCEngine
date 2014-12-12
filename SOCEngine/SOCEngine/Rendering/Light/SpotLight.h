#pragma once

#include "LightForm.h"
#include "Cone.h"

namespace Rendering
{
	namespace Light
	{
		class SpotLight : public LightForm
		{
		public:
			float spotAngle;

		public:
			SpotLight();
			~SpotLight();

		public:
			virtual bool Intersects(Intersection::Sphere &sphere);
		};

	}
}
