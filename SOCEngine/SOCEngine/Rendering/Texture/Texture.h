#pragma once

#include "DirectX.h"
#include "Color.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture
		{
		private:
			ID3D11ShaderResourceView *_srv;
			bool _hasAlpha;

		public:
			Texture(ID3D11ShaderResourceView* srv, bool hasAlpha);
			~Texture();

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);
		};
	}
}