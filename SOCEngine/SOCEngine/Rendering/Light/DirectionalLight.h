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
			struct Param
			{
				ushort dirX;
				ushort dirY;

				Param(float dirX, float dirY);
				~Param() {}
			};

		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			virtual void OnUpdate(float deltaTime);

		public:
			virtual void CreateShadow();

		public:
			virtual bool Intersect(const Intersection::Sphere &sphere) const;
			virtual void MakeLightBufferElement(LightTransformBuffer& out, std::shared_ptr<Container>* outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Shadow,							Shadow::DirectionalLightShadow*,	static_cast<Shadow::DirectionalLightShadow*>(_shadow));
			GET_SET_ACCESSOR(LightShaftSize,				float,								_radius);
		};
	}
}
