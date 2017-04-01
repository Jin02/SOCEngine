#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture final : public Texture2D
		{
		public:
			RenderTexture();
			~RenderTexture();

		public:
			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat, uint optionalBindFlags, uint sampleCount = 0, uint mipLevel = 1);
			void Initialize(Device::DirectX& dx, const DXResource<ID3D11RenderTargetView>& rtv, const Size<uint>& size);

			void Destroy();

			void Clear(Device::DirectX& dx, const Color& color);

			GET_CONST_ACCESSOR(RenderTargetView, const DXResource<ID3D11RenderTargetView>&, _renderTargetView);

		private:
			using Texture2D::Initialize;
			using Texture2D::Destroy;

		private:
			DXResource<ID3D11RenderTargetView>	_renderTargetView;
		};
	}
}