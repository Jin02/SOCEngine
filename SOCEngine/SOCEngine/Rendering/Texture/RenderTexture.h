#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include "Texture.h"

namespace Device
{
	class DirectX;
};

namespace Rendering
{
	namespace Texture
	{
		class DepthBuffer;
		class RenderTexture : public Texture
		{
		protected:
			ID3D11Texture2D*			_texture;
			ID3D11RenderTargetView*		_renderTargetView;

		public:
			RenderTexture();
			virtual ~RenderTexture();

		protected:
			bool _Create(const Math::Size<unsigned int>& size, DXGI_FORMAT format, unsigned int bindFlags);

		public:
			bool Create(const Math::Size<unsigned int>& size);
			void Destroy();
			void SetRenderTarget(const DepthBuffer* depthBuffer, const Device::DirectX* dx = nullptr);
			void Clear(const Color& color, const Device::DirectX* dx = nullptr);

		public:
			GET_ACCESSOR(Texture, ID3D11Texture2D*, _texture);
			GET_ACCESSOR(RenderTargetView, ID3D11RenderTargetView*, _renderTargetView);
		};
	}
}