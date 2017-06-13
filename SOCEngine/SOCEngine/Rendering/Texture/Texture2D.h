#pragma once

#include "DirectX.h"
#include "Color.h"
#include "Rect.h"

#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Manager
	{
		class Texture2DManager;
	}

	namespace Texture
	{
		class Texture2D final
		{
		public:
			Texture2D() = default;
			explicit Texture2D(const View::ShaderResourceView& srv, const DXSharedResource<ID3D11Texture2D>& tex, bool hasAlpha, const Size<uint>& size = Size<uint>(0, 0));

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, uint width, uint height, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint bindFlags, uint sampleCount, uint mipLevels);
			void GenerateMips(Device::DirectX& dx);

			const Size<uint>& FetchSize();

			GET_CONST_ACCESSOR(Texture, DXSharedResource<ID3D11Texture2D>, _texture);
			GET_ACCESSOR(RawTexture, auto, _texture.GetRaw());

			GET_CONST_ACCESSOR(Size, const Size<uint>&, _size);
			GET_CONST_ACCESSOR(CanUse, bool, _texture.IsCanUse());

			SET_ACCESSOR(Size, const Size<uint>&, _size);
			SET_ACCESSOR(Texture, const DXSharedResource<ID3D11Texture2D>&, _texture);

			GET_ACCESSOR(ShaderResourceView, auto&, _srv);
			GET_ACCESSOR(UnorderedAccessView, auto&, _uav);

		private:
			DXSharedResource<ID3D11Texture2D>		_texture;
			Size<uint>				_size = Size<uint>(0, 0);
			View::ShaderResourceView		_srv;
			View::UnorderedAccessView		_uav;
		};
	}
}
