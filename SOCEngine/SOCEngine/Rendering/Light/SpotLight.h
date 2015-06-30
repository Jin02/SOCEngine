#pragma once

#include "LightForm.h"
#include "Cone.h"
#include "Vector2.h"

namespace Rendering
{
	namespace Light
	{
		class SpotLight : public LightForm
		{
		public:
			struct Params
			{
				Math::Vector3			dir;
				unsigned short			coneAngle;
				unsigned short			falloff;

				Params() : coneAngle(0), falloff(0) {}
				~Params() {}
			};

		public:
			Params			_param;

			float			_spotAngleDegree;
			float			_falloff;

		public:
			SpotLight();
			~SpotLight();

		public:
			virtual bool Intersects(Intersection::Sphere &sphere);
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			GET_SET_ACCESSOR(SpotAngleDegree, float, _spotAngleDegree);
			GET_SET_ACCESSOR(Falloff, float, _falloff);
		};

	}
}
