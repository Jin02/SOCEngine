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

			Math::Matrix	_prevViewProj;

		public:
			PointLightShadow(const Light::LightForm* owner);
			~PointLightShadow();

		public:
			void ComputeViewProjMatrix();

		public:
			void MakeParam(Param& outParam, uint lightIndex) const;
			void MakeMatrixParam(std::array<Math::Matrix, 6>& outViewProjMat) const;

			GET_ACCESSOR(UnderScanSize,			float,	_underScanSize);
			SET_SHADOW_ACCESSOR(UnderScanSize,	float,	_underScanSize, _paramUpdateCounter);

			void GetInvNearFarViewProjMatrices(std::array<Math::Matrix, 6>& out) const;
			void GetViewProjectionMatrices(std::array<Math::Matrix, 6>& out) const;
		};
	}
}