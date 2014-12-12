#pragma once

#include <d3d11.h>
#include "Color.h"

namespace Device
{
	class DirectX;
};

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture
		{
		private:
			ID3D11Texture2D*			_texture;
			ID3D11RenderTargetView*		_renderTargetView;
			ID3D11ShaderResourceView*	_shaderResourceView;

		public:
			RenderTexture();
			~RenderTexture();

		public:
			bool Create(const Device::DirectX* dx, int width, int height);
			void Destroy();
			void SetRenderTarget(const Device::DirectX* dx);
			void Clear(const Device::DirectX* dx, const Color& color);
		};
	}
}