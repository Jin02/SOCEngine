#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include "TextureForm.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture2D : public TextureForm
		{
		protected:
			ID3D11Texture2D*			_texture;
			bool						_hasAlpha;

			Math::Size<uint>			_size;

		public:
			Texture2D();
			Texture2D(ID3D11ShaderResourceView* srv, ID3D11Texture2D* tex, bool hasAlpha);
			virtual ~Texture2D();

		protected:
			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(uint width, uint height, DXGI_FORMAT format, uint bindFlags, uint sampleCount, uint mipLevels);
			void Destroy();

		public:
			const Math::Size<uint>& FetchSize();

		public:
			GET_ACCESSOR(Texture, ID3D11Texture2D*, _texture);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);

			GET_ACCESSOR(Size, const Math::Size<uint>&, _size);
		};
	}
}