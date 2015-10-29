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
			struct Params
			{
				unsigned short dirX;
				unsigned short dirY;
				//dirZ는, LightTransformBuffer의 radius에 해당함

				Params() {}
				~Params() {}
			};

		private:
			Intersection::Frustum				_frustum;
			Shadow::DirectionalLightShadow*		_shadow;

		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox);
			virtual void CreateLightShadow();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Frustum, const Intersection::Frustum&, _frustum);
			GET_ACCESSOR(Shadow, const Shadow::DirectionalLightShadow*, static_cast<const Shadow::DirectionalLightShadow*>(_shadow));
		};
	}
}