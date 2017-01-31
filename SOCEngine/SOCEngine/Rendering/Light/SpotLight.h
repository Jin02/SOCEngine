#pragma once

#include "LightForm.h"
#include "Cone.h"
#include "Vector2.h"
#include "SpotLightShadow.h"

namespace Rendering
{
	namespace Light
	{
		class SpotLight : public LightForm
		{
		public:
			typedef LightForm::WorldPosWithRadius TransformType;
			struct Param 
			{
				LightForm::DirXYHalf		dirXY;
				unsigned short			outerConeCosAngle;
				unsigned short			innerConeCosAngle;

				Param(float dirX, float dirY) : dirXY(dirX, dirY), outerConeCosAngle(0), innerConeCosAngle(0) {}
				~Param() {}
			};

		private:
			Param			_param;
			float			_spotAngleDegree;

		public:
			SpotLight();
			virtual ~SpotLight();

		public:
			virtual void CreateShadow();

		public:
			virtual bool Intersect(const Intersection::Sphere &sphere) const;

			void MakeTransform(TransformType& out) const;			
			void MakeParam(Param& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			void SetSpotAngleDegree(float d);

			GET_ACCESSOR(SpotAngleDegree,	float,						_spotAngleDegree);
			GET_ACCESSOR(Shadow,			Shadow::SpotLightShadow*,	static_cast<Shadow::SpotLightShadow*>(_shadow) );
		};
	}
}
