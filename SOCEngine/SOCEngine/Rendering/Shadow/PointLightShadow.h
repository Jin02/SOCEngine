#pragma once

#include "BaseShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow final
		{
		public:
			void ComputeViewProjMatrix();

		public:
			std::array<Math::Matrix, 6> MakeMatrixParam() const;
			std::array<Math::Matrix, 6> GetViewProjectionMatrices() const;

		private:
			Math::Matrix	_viewProjMat[6];
			BaseShadow		_base;
		};
	}
}