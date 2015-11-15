#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include <tuple>

namespace Rendering
{
	namespace Texture
	{
		class Texture3D
		{
		protected:
			ID3D11Texture3D*			_texture;
			ID3D11ShaderResourceView*	_srv;

		public:
			Texture3D(ID3D11ShaderResourceView* srv, ID3D11Texture3D* tex);
			virtual ~Texture3D();

		protected:
			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(const Math::Size<unsigned int>& size, uint depth, DXGI_FORMAT format, unsigned int bindFlags, unsigned int mipLevels);

		public:
			// w, h, depth
			std::tuple<Math::Size<uint>, uint> FetchSize() const;

		public:
			GET_ACCESSOR(Texture, ID3D11Texture3D*, _texture);
			GET_ACCESSOR(ShaderResourceView, ID3D11ShaderResourceView* const*, &_srv);
		};
	}
}