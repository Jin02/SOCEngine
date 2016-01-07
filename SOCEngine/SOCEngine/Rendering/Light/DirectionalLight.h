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
			Intersection::Frustum		_frustum;
			float						_projectionSize;
			bool						_useAutoProjectLocation;

		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			virtual void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox);
			virtual void CreateLightShadow(const std::function<void()>& addUpdateCounter);

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Params& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Frustum,	const Intersection::Frustum&,		_frustum);
			GET_ACCESSOR(Shadow,	Shadow::DirectionalLightShadow*,	static_cast<Shadow::DirectionalLightShadow*>(_shadow));

			GET_SET_ACCESSOR(ProjectionSize,			float,	_projectionSize);
			GET_SET_ACCESSOR(UseAutoProjectionLocation,	bool,	_useAutoProjectLocation);
		};
	}
}