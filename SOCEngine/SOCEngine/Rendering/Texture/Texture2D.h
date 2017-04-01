#pragma once

#include "DirectX.h"
#include "Color.h"
#include "Rect.h"
#include "BaseTexture.h"

namespace Rendering
{
	namespace Manager
	{
		class Texture2DManager;
	}

	namespace Texture
	{
		class Texture2D
		{
		public:
			Texture2D();
			explicit Texture2D(const View::ShaderResourceView& srv, const DXResource<ID3D11Texture2D>& tex, bool hasAlpha, const Size<uint>& size = Size<uint>(0, 0));
			virtual ~Texture2D();

			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(Device::DirectX& dx, uint width, uint height, DXGI_FORMAT srvFormat, DXGI_FORMAT uavFormat, uint bindFlags, uint sampleCount, uint mipLevels);
			void Destroy();
			void GenerateMips(Device::DirectX& dx);

			const Size<uint>& FetchSize();

			GET_CONST_ACCESSOR(Texture, DXResource<ID3D11Texture2D>, _texture);
			GET_CONST_ACCESSOR(Size, const Size<uint>&, _size);
			GET_CONST_ACCESSOR(CanUse, bool, _texture.IsCanUse());

		protected:
			SET_ACCESSOR(Size, const Size<uint>&, _size);
			SET_ACCESSOR(Texture, const DXResource<ID3D11Texture2D>&, _texture);
			GET_ACCESSOR(Base, BaseTexture&, _base);

		private:
			BaseTexture						_base;
			DXResource<ID3D11Texture2D>		_texture;
			Size<uint>						_size;
		};
	}
}