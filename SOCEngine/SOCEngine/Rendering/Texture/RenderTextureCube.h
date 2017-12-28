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
			void Initialize(Device::DirectX& dx, uint resolution, DXGI_FORMAT format);
			void Destroy();

			void GenerateMips(Device::DirectX& dx);

		public:
			GET_ACCESSOR(Raw, ID3D11RenderTargetView* const, _renderTargetView.GetRaw());
			GET_ACCESSOR_PTR(Texture2D, Texture::Texture2D, _tex2D);

		private:
			DXSharedResource<ID3D11RenderTargetView>	_renderTargetView;
			Texture::Texture2D							_tex2D;
		};
	}
}
