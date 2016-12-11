#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow : public ShadowCommon
		{
		private:
			Math::Matrix	_invNearFarViewProjMat[5];	// another viewMat is placed in LightForm.
			Math::Matrix	_viewProjMat[5];			// another viewProjMat is placed in LightForm.

			Math::Matrix	_prevViewProj;

		public:
			PointLightShadow(const Light::LightForm* owner);
			~PointLightShadow();

		public:
			void ComputeViewProjMatrix();

		public:
			void MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat) const;

			void GetInvNearFarViewProjMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;
		};
	}
}