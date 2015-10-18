#pragma once

#include "LightForm.h"

namespace Rendering
{
	namespace Light
	{
		class PointLight : public LightForm
		{
		public:
			PointLight();
			~PointLight();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& out) const;

		public:
			virtual Core::Component* Clone() const;
		};

	}
}