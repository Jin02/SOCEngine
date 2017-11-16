#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthMap
		{
		public:
			DepthMap() = default;

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, const Size<uint>& size, bool useShaderResource, uint sampleCount = 0);
			void Destroy();

			void Clear(Device::DirectX& dx, float depth, unsigned char stencil);

			GET_ACCESSOR_REF(DepthStencilView,										_depthStencilView);
			GET_ACCESSOR_REF(Texture2D,												_tex2D);

			GET_ACCESSOR(RawDepthStencilView,		ID3D11DepthStencilView* const,	_depthStencilView.GetRaw());
			GET_CONST_ACCESSOR(Size,				const auto&,					_tex2D.GetSize());

		private:
			DXSharedResource<ID3D11DepthStencilView>		_depthStencilView;
			Texture2D										_tex2D;
		};
	}
}
