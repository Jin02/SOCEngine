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
			void Destroy();

			void Clear(Device::DirectX& dx, float depth, unsigned char stencil);

			GET_CONST_ACCESSOR(DepthStencilView, const DXSharedResource<ID3D11DepthStencilView>&, _depthStencilView);
			GET_CONST_ACCESSOR(RawDepthStencilView, auto, _depthStencilView.GetRaw());
			GET_ACCESSOR(Texture2D, Texture2D&, _tex2D);

		private:
			DXSharedResource<ID3D11DepthStencilView>		_depthStencilView;
			Texture2D										_tex2D;
		};
	}
}
