#pragma once

#include "FullScreen.h"

namespace Rendering
{
	namespace DeferredShading
	{
		class OffScreen : private PostProcessing::FullScreen
		{
		private:
			const Rendering::Texture::RenderTexture* _offScreenRT;

		public:
			OffScreen();
			virtual ~OffScreen();

		public:
			void Initialize(const Rendering::Texture::RenderTexture* offScreenRT);
			void Render();
			void Destroy();
		};
	}
}