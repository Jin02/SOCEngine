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
				unsigned short			dirX;
				unsigned short			dirY;

				unsigned short			outerConeCosAngle;
				unsigned short			innerConeCosAngle;

				Params() : outerConeCosAngle(0), innerConeCosAngle(0), dirX(0), dirY(0) {}
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
			virtual bool Intersects(const Intersection::Sphere &sphere);
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_SET_ACCESSOR(SpotAngleDegree, float, _spotAngleDegree);
			GET_SET_ACCESSOR(Falloff, float, _falloff);
		};

	}
}
