#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class SpotLightShadow : public ShadowCommon
		{
		public:
			typedef ShadowCommon::CommonParam Param;

		public:
			SpotLightShadow(const Light::LightForm* owner);
			~SpotLightShadow();

		public:
			void ComputeViewProjMatrix(const Math::Matrix& invViewportMat);

		public:
			void MakeMatrixParam(Math::Matrix& outViewProjMat, Math::Matrix& outInvVPVMat) const;
		};
	}
}