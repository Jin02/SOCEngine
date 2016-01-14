#pragma once

#include "ShadowCommon.h"

namespace Rendering
{
	namespace Shadow
	{
		class PointLightShadow : public ShadowCommon
		{
		public:
			struct Param : ShadowCommon::CommonParam
			{
				float underScanSize;
			};

		private:
			float			_underScanSize;				// default is 4.25f
			Math::Matrix	_invNearFarViewProjMat[5];	// another viewMat is placed in LightForm.
			Math::Matrix	_viewProjMat[5];			// another viewProjMat is placed in LightForm.

			Math::Matrix	_invViewProjViewportMat[5];	// another invViewProjViewportMat is placed in LightForm.

			Math::Matrix	_prevViewProj;

		public:
			PointLightShadow(const Light::LightForm* owner);
			~PointLightShadow();

		public:
			void ComputeViewProjMatrix(const Math::Matrix& invViewportMat);

		public:
			void MakeParam(Param& outParam, uint lightIndex) const;
			void MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat, std::array<Math::Matrix, 6>& outInvVPVMat) const;

			GET_SET_ACCESSOR(UnderScanSize, float, _underScanSize);

			void GetInvNearFarViewProjMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetInvViewProjViewportMatrices(std::array<Math::Matrix, 6>& out) const;
		};
	}
}