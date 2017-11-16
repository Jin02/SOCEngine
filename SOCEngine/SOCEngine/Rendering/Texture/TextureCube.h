#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class TextureCube
		{
		public:
			TextureCube() = default;
			
			void Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT format, bool useRTV, bool useMipmap);
			void Clear(Device::DirectX& dx);

			GET_CONST_ACCESSOR(UseMipmap,			bool,												_useMipmap);
			GET_CONST_ACCESSOR(RenderTargetView,	const DXSharedResource<ID3D11RenderTargetView>&,	_rtv);
			GET_ACCESSOR(RawRenderTargetView,		ID3D11RenderTargetView* const,						_rtv.GetRaw());
			GET_ACCESSOR(RawTexture,				ID3D11Texture2D* const,								_texture.GetRaw());
			GET_ACCESSOR_REF(ShaderResourceView,														_srv);

		private:
			DXSharedResource<ID3D11RenderTargetView>	_rtv;
			DXSharedResource<ID3D11Texture2D>			_texture;
			View::ShaderResourceView					_srv;

			bool										_useMipmap = false;
		};
	}
}
