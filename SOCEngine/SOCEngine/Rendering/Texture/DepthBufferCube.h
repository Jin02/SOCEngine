#pragma once

#include "ShaderResourceView.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBufferCube final
		{
		public:
			DepthBufferCube(void) = default;
			void Initialize(Device::DirectX& dx, const Size<uint>& size, bool useShaderResource);

			void Clear(Device::DirectX& dx, float depth, unsigned char stencil);

			GET_CONST_ACCESSOR(DepthStencilView,	const DXSharedResource<ID3D11DepthStencilView>&,	_depthStencilView);
			GET_ACCESSOR(RawDepthStencilView,		ID3D11DepthStencilView* const,						_depthStencilView.GetRaw());
			GET_ACCESSOR(RawTexture2D,				ID3D11Texture2D* const,								_texture.GetRaw());

		private:
			DXSharedResource<ID3D11DepthStencilView>		_depthStencilView;
			DXSharedResource<ID3D11Texture2D>				_texture;
			View::ShaderResourceView						_srv;
		};
	}
}
