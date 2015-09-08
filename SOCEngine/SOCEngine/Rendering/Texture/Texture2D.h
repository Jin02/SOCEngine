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
			Texture2D(ID3D11ShaderResourceView* srv, ID3D11Texture2D* tex, bool hasAlpha);
			virtual ~Texture2D();

		protected:
			// if SampleCount = -1, sampleCount = msaa.count
			void Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT format, unsigned int bindFlags, unsigned int sampleCount);

		public:
			Math::Size<uint> FetchSize() const;

		public:
			GET_ACCESSOR(Texture, ID3D11Texture2D*, _texture);
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView* const*, &_srv);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		};
	}
}