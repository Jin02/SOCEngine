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

		public:
			Texture(ID3D11ShaderResourceView* srv);
			~Texture();

		public:
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView*, _srv);
		};
	}
}