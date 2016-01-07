#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow : public ShadowCommon
		{
		public:
			struct Param
			{
				uint	indexWithBias;
				float	underScanSize;

				std::array<Math::Matrix, 6>	viewProjMat;
			};
		private:
			float _underScanSize; //default is 4.25f

		public:
			PointLightShadow(const Light::LightForm* owner, const std::function<void()>& ownerUpdateCounter);
			~PointLightShadow();

		public:
			void MakeParam(Param& outParam, bool useVSM) const;
			GET_SET_ACCESSOR(UnderScanSize, float, _underScanSize);
		};
	}
}