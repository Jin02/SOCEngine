#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include "Texture.h"
#include "DepthBuffer.h"

namespace Device
{
	class DirectX;
};

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture : public Texture
		{
		protected:
			ID3D11RenderTargetView*		_renderTargetView;

		public:
			RenderTexture();
			virtual ~RenderTexture();

		public:
			bool Create(const Math::Size<unsigned int>& size, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT);
			void Destroy();
			void SetRenderTarget(const DepthBuffer* depthBuffer, const Device::DirectX* dx = nullptr);
			void Clear(const Color& color, const Device::DirectX* dx = nullptr);

		public:
			GET_ACCESSOR(RenderTargetView, ID3D11RenderTargetView*, _renderTargetView);
		};
	}
}