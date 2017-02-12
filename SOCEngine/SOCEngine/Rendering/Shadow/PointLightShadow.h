#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow : public ShadowCommon
		{
		private:
			Math::Matrix	_viewProjMat[5];
			Math::Matrix	_prevViewProj;

		public:
			PointLightShadow(const Light::LightForm* owner);
			~PointLightShadow();

		public:
			void ComputeViewProjMatrix();

		public:
			void MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat) const;

			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;
		};
	}
}