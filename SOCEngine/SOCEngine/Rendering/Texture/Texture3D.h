#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Vector3.h"
#include "TextureForm.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture3D : public TextureForm
		{
		protected:
			ID3D11Texture3D*				_texture;
			Math::Vector3					_size;
			ID3D11RenderTargetView*			_rtv;


		public:
			Texture3D();
			virtual ~Texture3D();

		public:
			void Initialize(uint width, uint height, uint depth,
							DXGI_FORMAT typelessFormat, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat,
							uint optionBindFlag, uint mipLevels);
			void Destory();

		public:
			GET_ACCESSOR(Texture,				ID3D11Texture3D*,				_texture);
			GET_ACCESSOR(RenderTargetView,		ID3D11RenderTargetView*,		_rtv);
			GET_ACCESSOR(Size,					const Math::Vector3&,			_size);
		};
	}
}