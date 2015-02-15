#pragma once

#include <d3d11.h>
#include "Color.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture
		{
		protected:
			ID3D11ShaderResourceView *_srv;
			bool _hasAlpha;

		public:
			Texture(ID3D11ShaderResourceView* srv, bool hasAlpha);
			virtual ~Texture();

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		};
	}
}