#pragma once

#include "BaseTexture.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture3D final
		{
		public:
			Texture3D();
			~Texture3D();

			void Initialize(Device::DirectX& dx,
							uint width, uint height, uint depth,
							DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels);
			void Destroy();

			GET_CONST_ACCESSOR(Texture,				const DXResource<ID3D11Texture3D>&,			_texture);
			GET_CONST_ACCESSOR(RenderTargetView,	const DXResource<ID3D11RenderTargetView>&,	_rtv);
			GET_CONST_ACCESSOR(Size,				const Math::Vector3&,						_size);

		private:
			BaseTexture								_base;
			DXResource<ID3D11Texture3D>				_texture;
			Math::Vector3							_size;
			DXResource<ID3D11RenderTargetView>		_rtv;
		};
	}
}