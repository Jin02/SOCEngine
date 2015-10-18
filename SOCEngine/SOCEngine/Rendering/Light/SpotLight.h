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

		public:
			SpotLight();
			~SpotLight();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			void SetSpotAngleDegree(float d);
			GET_ACCESSOR(SpotAngleDegree, float, _spotAngleDegree);
		};

	}
}
