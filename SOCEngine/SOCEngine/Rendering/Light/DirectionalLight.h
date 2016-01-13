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
				unsigned short dirX;
				unsigned short dirY;
				//dirZ는, LightTransformBuffer의 radius에 해당함

				Param() {}
				~Param() {}
			};

		private:
			Math::Matrix				_invNearFarViewProjMat;
			Math::Matrix				_viewProjMat;
			Math::Matrix				_invViewProjViewportMat;

			Intersection::Frustum		_frustum;
			
			float						_projectionSize;
			bool						_useAutoProjectLocation;

		public:
			DirectionalLight();
			virtual ~DirectionalLight();

		public:
			void ComputeViewProjMatrix(const Intersection::BoundBox& sceneBoundBox, const Math::Matrix& invViewportMat);
			virtual void CreateShadow();

		public:
			bool Intersect(const Intersection::Sphere &sphere) const;
			void MakeLightBufferElement(LightTransformBuffer& outTransform, Param& outParam) const;

		public:
			virtual Core::Component* Clone() const;

		public:
			GET_ACCESSOR(Frustum,							const Intersection::Frustum&,		_frustum);
			GET_ACCESSOR(Shadow,							Shadow::DirectionalLightShadow*,	static_cast<Shadow::DirectionalLightShadow*>(_shadow));
			GET_ACCESSOR(InvNearFarViewProjectionMatrix,	const Math::Matrix&,				_invNearFarViewProjMat);
			GET_ACCESSOR(InvViewProjViewportMatrix,			const Math::Matrix&,				_invViewProjViewportMat);
			GET_ACCESSOR(ViewProjectionMatrix,				const Math::Matrix&,				_viewProjMat);

			GET_SET_ACCESSOR(ProjectionSize,			float,	_projectionSize);
			GET_SET_ACCESSOR(UseAutoProjectionLocation,	bool,	_useAutoProjectLocation);
		};
	}
}