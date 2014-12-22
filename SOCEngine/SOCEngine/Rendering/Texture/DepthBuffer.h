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
			const Device::DirectX		*_dx;

		public:
			DepthBuffer(const Device::DirectX* dx);
			~DepthBuffer();

		private:
			void SetRenderTarget(const Device::DirectX* dx);

		public:
			bool Create(const Math::Size<unsigned int>& size, bool useShaderResource = false);
			void Destroy();

			void Clear(float depth, unsigned char stencil);

		public:
			GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);
		};
	}
}