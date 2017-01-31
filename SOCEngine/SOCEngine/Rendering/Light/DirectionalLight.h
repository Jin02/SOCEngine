#pragma once

#include "LightForm.h"
#include "Frustum.h"
#include "DirectionalLightShadow.h"

#define DIRECTIONAL_LIGHT_FRUSTUM_MIN_Z		1.0f
#define DIRECTIONAL_LIGHT_FRUSTUM_MAX_Z		10000.0f

namespace Rendering
{
	namespace Light
	{
		class DirectionalLight : public LightForm
		{
		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			virtual void OnUpdate(float deltaTime);

		public:
			virtual void CreateShadow();

		public:
			virtual bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeTransform(DirXYHalf& outDirXY) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Shadow,			Shadow::DirectionalLightShadow*,	static_cast<Shadow::DirectionalLightShadow*>(_shadow));
			GET_SET_ACCESSOR(LightShaftSize,	float,					_radius);
		};
	}
}
