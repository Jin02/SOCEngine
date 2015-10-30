#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow : public ShadowCommon
		{
		public:
			struct Param : public ShadowCommon::CommonParam
			{
				std::array<Math::Matrix, 6>	viewProjMat;
			};

		public:
			PointLightShadow(const Light::LightForm* owner, const std::function<void()>& ownerUpdateCounter);
			~PointLightShadow();

		public:
			void MakeParam(Param& outParam) const;
		};
	}
}