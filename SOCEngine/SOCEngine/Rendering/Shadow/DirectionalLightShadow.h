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
			};

		public:
			DirectionalLightShadow(const Light::LightForm* owner);
			~DirectionalLightShadow();

		public:
			void MakeParam(Param& outParam) const;
		};
	}
}