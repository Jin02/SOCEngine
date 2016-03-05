#pragma once

#include "Texture2D.h"

namespace Rendering
{
	namespace Texture
	{
		class TextureCube : public Texture2D
		{
		private:
			ID3D11Texture2D*			_depthMap;
			ID3D11DepthStencilView*		_dsv;
			ID3D11RenderTargetView*		_rtv;
			bool						_useMipmap;

		public:
			TextureCube();
			virtual ~TextureCube();

		public:
			void Initialize(const Math::Size<uint>& size, DXGI_FORMAT format, bool useRTV, bool useDSV, bool useMipmap);
			void Destroy();

		public:
			void Clear(const Device::DirectX* dx);

		public:
			GET_ACCESSOR(UseMipmap,			bool,						_useMipmap);
			GET_ACCESSOR(RenderTargetView,	ID3D11RenderTargetView*,	_rtv);
			GET_ACCESSOR(DepthStencilView,	ID3D11DepthStencilView*,	_dsv);
		};
	}
}