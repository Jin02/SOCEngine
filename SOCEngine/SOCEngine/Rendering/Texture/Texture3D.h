#pragma once

#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture3D final
		{
		public:
			Texture3D() = default;

			void Initialize(Device::DirectX& dx,
					uint width, uint height, uint depth,
					DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
					uint optionBindFlag, uint mipLevels);

			GET_CONST_ACCESSOR(Texture,				const DXSharedResource<ID3D11Texture3D>&,			_texture);
			GET_CONST_ACCESSOR(RenderTargetView,	const DXSharedResource<ID3D11RenderTargetView>&,	_rtv);
			GET_ACCESSOR(RawRenderTargetView,		auto,										_rtv.GetRaw());
			GET_ACCESSOR(RawTexture,				auto,										_texture.GetRaw());

			GET_CONST_ACCESSOR(Size,				const Math::Vector3&,				_size);

		private:
			DXSharedResource<ID3D11Texture3D>			_texture;
			Math::Vector3					_size = Math::Vector3(0.0f, 0.0f, 0.0f);
			DXSharedResource<ID3D11RenderTargetView>		_rtv;
			View::ShaderResourceView			_srv;
			View::UnorderedAccessView			_uav;
		};
	}
}
