#pragma once

#include "FullScreen.h"
#include "TileBasedShadingHeader.h"

namespace Rendering
{
	class MainSceneMaker final
	{
	public:
		void Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr, const Size<uint>& renderSize);
		void Destroy();

		struct RenderParam
		{
			const Texture::Texture2D&	opaqueMap;
			const Texture::Texture2D&	giMap;
			const Texture::Texture2D&	transparentMap;
			const Texture::Texture2D&	skyBoxMap;
		};
		void Render(Device::DirectX& dx, Texture::RenderTexture& output, const RenderParam&& param);

	private:
		PostProcessing::FullScreen	_screen;
	};
}