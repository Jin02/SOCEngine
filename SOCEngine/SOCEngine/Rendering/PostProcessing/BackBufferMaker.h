#pragma once

#include "FullScreen.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class BackBufferMaker : public FullScreen
		{
		public:
			enum class InputTextureShaderIndex : uint
			{
				RenderScene	= 0,
				UIScene		= 1
			};

		public:
			BackBufferMaker();
			virtual ~BackBufferMaker();

		private:
			void Render(const Texture::RenderTexture* outResultRT, ID3D11SamplerState* sampler){}

		public:
			void Initialize(bool useUI);
			void Render(
				const Texture::RenderTexture* outResultRT,
				const Texture::RenderTexture* renderScene,
				const Texture::RenderTexture* uiScene
				);
		};
	}
}