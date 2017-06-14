#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture final
		{
		public:
			RenderTexture() = default;

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat, uint optionalBindFlags, uint sampleCount = 0, uint mipLevel = 1);
			void Initialize(Device::DirectX& dx, const DXSharedResource<ID3D11RenderTargetView>& rtv, const Size<uint>& size);

			void Clear(Device::DirectX& dx, const Color& color);

			//GET_CONST_ACCESSOR(RenderTargetView, const DXSharedResource<ID3D11RenderTargetView>&, _renderTargetView);
			GET_ACCESSOR(Raw, auto, _renderTargetView.GetRaw());
			GET_CONST_ACCESSOR(Size, const auto&, _tex2D.GetSize());
			GET_ACCESSOR(Texture2D, auto&, _tex2D);

		private:
			DXSharedResource<ID3D11RenderTargetView>	_renderTargetView;
			Texture2D									_tex2D;
		};
	}
}
