#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class SpotLightShadow : public ShadowCommon
		{
		public:
			struct Param : public ShadowCommon::CommonParam
			{
				Math::Matrix	viewProjMat;
			};

		public:
			SpotLightShadow(const Light::LightForm* owner, const std::function<void()>& ownerUpdateCounter);
			~SpotLightShadow();

		public:
			void MakeParam(Param& outParam, bool useVSM) const;
		};
	}
}