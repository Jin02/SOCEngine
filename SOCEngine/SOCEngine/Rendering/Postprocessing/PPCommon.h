#pragma once

#include "MainRenderer.h"
#include "MainCamera.h"

#include "RenderTexture.h"
#include <array>

namespace Rendering
{
	namespace PostProcessing
	{
		struct TempTextures
		{
			std::array<Texture::RenderTexture, 4>	downScaledTextures;

			Texture::RenderTexture					originSizeMap;
			Texture::RenderTexture					halfSizeMap;
			Texture::RenderTexture					minSizeMap;
		};

		struct Main
		{
			Renderer::MainRenderer& renderer;
			Camera::MainCamera&		camera;

			Main(Renderer::MainRenderer& _renderer, Camera::MainCamera& _camera)
				: camera(_camera), renderer(_renderer)
			{
			}
		};
	}
}