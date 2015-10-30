#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow : public ShadowCommon
		{
		public:
			struct Param : public ShadowCommon::CommonParam
			{
				Math::Matrix	viewProjMat;

				Param() : ShadowCommon::CommonParam() {}
				~Param() {}
			};

		public:
			DirectionalLightShadow(const Light::LightForm* owner, const std::function<void()>& ownerUpdateCounter);
			~DirectionalLightShadow();

		public:
			void MakeParam(Param& outParam) const;
		};
	}
}