#pragma once

#include "BaseShadow.h"

namespace Rendering
{
	namespace Shadow
	{
		class DirectionalLightShadow final
		{
		public:
			using ViewProjMatType = Math::Matrix;

		public:
			void MakeMatrixParam(ViewProjMatType& outViewProjMat) const;

		private:
			BaseShadow		_base;
			Math::Matrix	_viewProjMat;
		};
	}
}