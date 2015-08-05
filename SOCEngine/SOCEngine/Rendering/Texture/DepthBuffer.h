#pragma once

#include "Texture.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBuffer : public Texture
		{
		private:
			ID3D11DepthStencilView		*_depthStencilView;

		public:
			DepthBuffer(void);
			virtual ~DepthBuffer();

		public:
			bool Initialize(const Math::Size<unsigned int>& size, bool useShaderResource = false);
			void Destroy();

			void Clear(ID3D11DeviceContext* context, float depth, unsigned char stencil);

		public:
			GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);
		};
	}
}