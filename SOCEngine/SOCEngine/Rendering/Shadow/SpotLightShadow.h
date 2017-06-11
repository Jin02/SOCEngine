#pragma once

#include "BaseShadow.h"
#include "BaseLight.h"
#include "Transform.h"
#include "SpotLight.h"

namespace Rendering
{
	namespace Shadow
	{
		class SpotLightShadow final
		{
		public:
			void ComputeViewProjMatrix(const Light::LightPool<Light::SpotLight>& lightPool, const Core::TransformPool& tfPool);
			Math::Matrix MakeMatrixParam() const;

		private:
			Math::Matrix	_viewProjMat;
			BaseShadow		_base;
		};
	}
}