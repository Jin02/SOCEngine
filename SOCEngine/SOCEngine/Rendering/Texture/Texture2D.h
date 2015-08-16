#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture2D
		{
		protected:
			ID3D11Texture2D*			_texture;
			ID3D11ShaderResourceView*	_srv;
			bool						_hasAlpha;

		public:
			Texture2D(ID3D11ShaderResourceView* srv = nullptr, bool hasAlpha = false);
			virtual ~Texture2D();

		protected:
			void Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT format, unsigned int bindFlags = 0);

		public:
			Math::Size<uint> FetchSize() const;

		public:
			GET_ACCESSOR(Texture, ID3D11Texture2D*, _texture);
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		};
	}
}