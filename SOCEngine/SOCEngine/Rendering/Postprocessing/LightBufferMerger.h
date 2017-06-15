#pragma once

#include "FullScreen.h"

namespace Rendering
{
	namespace Renderer
	{
		class LightBufferMerger final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT,
				Texture::RenderTexture& diffuse, Texture::RenderTexture& specular);

		private:
			PostProcessing::FullScreen	_screen;
		};
	}
}