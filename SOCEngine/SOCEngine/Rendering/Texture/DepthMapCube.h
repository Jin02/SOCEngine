#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthMapCube
		{
		public:
			DepthMapCube() = default;
			void Initialize(Device::DirectX& dx, const Size<uint>& size, bool useSRV = false);
			void Destroy();

			void Clear(Device::DirectX& dx, float depth, unsigned char stencil);

		public:
			GET_ACCESSOR(RawDepthStencilView,		ID3D11DepthStencilView* const,	_dsv.GetRaw());
			GET_CONST_ACCESSOR(Size,				const auto&,					_size);

		private:
			DXSharedResource<ID3D11DepthStencilView>		_dsv;
			DXSharedResource<ID3D11Texture2D>				_texture;

			View::ShaderResourceView						_srv;
			Size<uint>										_size = Size<uint>(0, 0);
		};
	}
}