#pragma once

#include "Texture2D.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBuffer
		{
		public:
			DepthBuffer() = default;

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, const Size<uint>& size, bool useShaderResource, uint sampleCount = 0);
			void Clear(Device::DirectX& dx, float depth, unsigned char stencil);

			GET_CONST_ACCESSOR(DepthStencilView, const DXResource<ID3D11DepthStencilView>&, _depthStencilView);

		private:
			DXResource<ID3D11DepthStencilView>		_depthStencilView;
			Texture2D					_tex2D;
		};
	}
}
