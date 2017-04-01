#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class TextureCube
		{
		public:
			void Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT format, bool useRTV, bool useMipmap);
			void Destroy();

			void Clear(Device::DirectX& dx);

			GET_CONST_ACCESSOR(UseMipmap,			bool,										_useMipmap);
			GET_CONST_ACCESSOR(RenderTargetView,	const DXResource<ID3D11RenderTargetView>&,	_rtv);

		private:
			DXResource<ID3D11RenderTargetView>	_rtv;
			DXResource<ID3D11Texture2D>			_texture;
			bool								_useMipmap = false;
			View::ShaderResourceView			_srv;
		};
	}
}