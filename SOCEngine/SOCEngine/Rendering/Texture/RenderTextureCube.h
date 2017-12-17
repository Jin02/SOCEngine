#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class RenderTextureCube
		{
		public:
			RenderTextureCube() = default;
			void Initialize(Device::DirectX& dx, const Size<uint>& size, DXGI_FORMAT format);
			void Destroy();

			void GenerateMips(Device::DirectX& dx);

		public:
			GET_ACCESSOR(Raw,						ID3D11RenderTargetView* const,	_renderTargetView.GetRaw());

			GET_CONST_ACCESSOR(ShaderResourceView,	const auto&,					_srv);
			GET_CONST_ACCESSOR(Size,				const Size<uint>&,				_size);
			GET_CONST_ACCESSOR(CanUse,				bool,							_texture.IsCanUse());

		private:
			DXSharedResource<ID3D11RenderTargetView>	_renderTargetView;
			DXSharedResource<ID3D11Texture2D>			_texture;

			View::ShaderResourceView					_srv;
			Size<uint>									_size = Size<uint>(0, 0);
		};
	}
}
