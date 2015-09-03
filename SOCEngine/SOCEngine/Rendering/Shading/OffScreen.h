#pragma once

#include "FullScreen.h"
#include "CSRWTexture.h"

namespace Rendering
{
	namespace TBDR
	{
		class OffScreen : public PostProcessing::FullScreen
		{
		public:
			OffScreen();
			virtual ~OffScreen();

		public:
			void Initialize(const GPGPU::DirectCompute::CSRWTexture* offScreen);
		};
	}
}