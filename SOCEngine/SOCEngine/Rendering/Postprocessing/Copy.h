#pragma once

#include "FullScreen.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace PostProcessing
	{
		class Copy final
		{
		public:
			void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr);
			void Render(Device::DirectX& dx, Texture::RenderTexture& outResultRT, const Texture::Texture2D& inputColorMap) const;

		private:
			FullScreen	_screen;;
		};
	}
}