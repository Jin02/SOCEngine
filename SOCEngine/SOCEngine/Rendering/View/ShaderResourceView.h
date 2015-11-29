#pragma once

#include <d3d11.h>
#include "Vector3.h"
#include "Common.h"

namespace Rendering
{
	namespace View
	{
		class ShaderResourceView
		{
		private:
			ID3D11ShaderResourceView*		_srv;

		public:
			ShaderResourceView(ID3D11ShaderResourceView* srv = nullptr);
			virtual ~ShaderResourceView();

		public:
			void Initialize(ID3D11Resource* resource, DXGI_FORMAT format, uint mipLevel, D3D11_SRV_DIMENSION viewDimension);
			void Destory();

		public:
			GET_ACCESSOR(View, ID3D11ShaderResourceView*, _srv);
		};
	}
}