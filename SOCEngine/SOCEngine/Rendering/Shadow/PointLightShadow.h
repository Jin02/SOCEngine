#pragma once

#include "BaseShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow final
		{
		private:
			Math::Matrix	_viewProjMat[6];

		public:
			void ComputeViewProjMatrix();

		public:
			void MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat) const; //sr
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const; //cb
		};
	}
}