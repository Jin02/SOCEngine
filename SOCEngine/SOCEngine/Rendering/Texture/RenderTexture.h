#pragma once

#include <d3d11.h>
#include "Color.h"
#include "Size.h"
#include "Texture2D.h"
#include "DepthBuffer.h"

namespace Device
{
	class DirectX;
};

namespace Rendering
{
	namespace Texture
	{
		class RenderTexture : public Texture2D
		{
		protected:
			ID3D11RenderTargetView*		_renderTargetView;

		public:
			RenderTexture();
			virtual ~RenderTexture();

		public:
			// if SampleCount = 0, sampleCount = msaa.count
			bool Initialize(const Math::Size<unsigned int>& size, DXGI_FORMAT srvFormat, DXGI_FORMAT rtvFormat, DXGI_FORMAT uavFormat, uint optionalBindFlags, uint sampleCount = 0, uint mipLevel = 1);
			bool Initialize(ID3D11RenderTargetView* rtv, const Math::Size<uint>& size);

			void Destroy();

		public:
			void Clear(ID3D11DeviceContext* context, const Color& color);

		public:
			GET_ACCESSOR(RenderTargetView, ID3D11RenderTargetView*, _renderTargetView);
		};
	}
}