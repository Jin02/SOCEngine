#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "PointLight.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow final
		{
		public:
			void ComputeViewProjMatrix(const Light::LightPool<Light::PointLight>& lightPool, const Core::TransformPool& tfPool);
			std::array<Math::Matrix, 6> MakeMatrixParam() const;

		private:
			Math::Matrix	_viewProjMat[6];
			BaseShadow		_base;
		};
	}
}