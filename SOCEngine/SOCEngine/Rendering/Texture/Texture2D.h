#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include "ShaderResourceView.h"
#include "UnorderedAccessView.h"

namespace Rendering
{
	namespace Texture
	{
		class Texture2D
		{
		protected:
			ID3D11Texture2D*			_texture;

			View::ShaderResourceView*	_srv;
			View::UnorderedAccessView*	_uav;

			bool						_hasAlpha;

		public:
			Texture2D();
			Texture2D(ID3D11ShaderResourceView* srv, ID3D11Texture2D* tex, bool hasAlpha);
			virtual ~Texture2D();

		protected:
			// if SampleCount = 0, sampleCount = msaa.count
			void Initialize(uint width, uint height, DXGI_FORMAT format, uint bindFlags, uint sampleCount, uint mipLevels);
			void Destroy();

		public:
			Math::Size<uint> FetchSize() const;

		public:
			GET_ACCESSOR(Texture, ID3D11Texture2D*, _texture);
			GET_ACCESSOR(HasAlpha, bool, _hasAlpha);

			GET_ACCESSOR(ShaderResourceView,	const View::ShaderResourceView*,	_srv);
			GET_ACCESSOR(UnorderedAccessView,	const View::UnorderedAccessView*,	_uav);
		};
	}
}