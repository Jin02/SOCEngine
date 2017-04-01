#pragma once

#include "Texture2D.h"
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

			GET_CONST_ACCESSOR(DepthStencilView, const DXResource<ID3D11DepthStencilView>&, _depthStencilView);

		private:
			DXResource<ID3D11DepthStencilView>		_depthStencilView;
			DXResource<ID3D11Texture2D>				_texture;
		};
	}
}