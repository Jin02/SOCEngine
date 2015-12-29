#pragma once

#include "FullScreen.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class BackBufferMaker : public FullScreen
		{
		public:
			enum class TextureBindIndex : uint
			{
				RenderScene	= 0,
				UIScene		= 1
			};

		private:
			bool _useUI;

		public:
			BackBufferMaker();
			virtual ~BackBufferMaker();

		private:
			void Render(const Texture::RenderTexture* outResultRT, ID3D11SamplerState* sampler){}

		public:
			void Initialize(bool useUI);
			void Render(
				ID3D11RenderTargetView* outResultRTV,
				const Texture::RenderTexture*& renderScene,
				const Texture::RenderTexture* uiScene,
				const Buffer::ConstBuffer* const& tbrParamConstBuffer
				);
		};
	}
}