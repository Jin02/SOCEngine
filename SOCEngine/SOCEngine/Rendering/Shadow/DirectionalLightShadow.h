#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "DirectionalLight.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow final
		{
		public:
			struct Param
			{
				float invProj_34 = 1.0f;
				float invProj_44 = 1.0f;
			};

			Math::Matrix MakeMatrixParam() const;
			void ComputeViewProjMatrix(const Light::LightPool<Light::DirectionalLight>& lightPool, const Core::TransformPool& tfPool, const Intersection::BoundBox& sceneBoundBox);

			GET_ACCESSOR(ViewProjectionMatrix, const auto&, _viewProjMat);
			GET_CONST_ACCESSOR(Param, const Param&, _param);
			GET_CONST_ACCESSOR(Base, const BaseShadow&, _base);

			GET_SET_ACCESSOR(ProjectionSize, float, _projectionSize);
			GET_SET_ACCESSOR(UseAutoProjectionLocation, bool, _useAutoProjectLocation);

		public:
			constexpr static float FrustumMinZ = 1.0f;
			constexpr static float FrustumMaxZ = 10000.0f;

		private:
			BaseShadow		_base;
			Math::Matrix	_viewProjMat;
			Param			_param;

			float			_projectionSize = 0.0f;
			bool			_useAutoProjectLocation = false;
		};
	}
}