#pragma once

#include "RenderTexture.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBuffer : public RenderTexture
		{
		private:
			ID3D11DepthStencilView		*_depthStencilView;

		public:
			DepthBuffer();
			~DepthBuffer();

		public:
			bool Create(const Math::Size<unsigned int>& size);
			void Destroy();

			void SetRenderTarget(const Device::DirectX* dx = nullptr);
			void Clear(float depth, unsigned char stencil, const Device::DirectX* dx = nullptr);

		public:
			GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);
		};
	}
}