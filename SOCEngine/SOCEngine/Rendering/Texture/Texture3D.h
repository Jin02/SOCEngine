#pragma once

#include "TextureGPUView.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture3D final
		{
		public:
			Texture3D() = default;

			void Initialize(Device::DirectX& dx,
							uint width, uint height, uint depth,
							DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels);
			void Destroy();

			GET_CONST_ACCESSOR(Texture,				const DXResource<ID3D11Texture3D>&,			_texture);
			GET_CONST_ACCESSOR(RenderTargetView,	const DXResource<ID3D11RenderTargetView>&,	_rtv);
			GET_CONST_ACCESSOR(Size,				const Math::Vector3&,						_size);

		private:
			TextureGPUView							_base;
			DXResource<ID3D11Texture3D>				_texture;
			Math::Vector3							_size = Math::Vector3(0.0f, 0.0f, 0.0f);
			DXResource<ID3D11RenderTargetView>		_rtv;
		};
	}
}