#pragma once

#include "Texture2D.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBuffer : public Texture2D
		{
		private:
			ID3D11DepthStencilView		*_depthStencilView;

		public:
			DepthBuffer(void);
			virtual ~DepthBuffer();

		public:
			// if SampleCount = 0, sampleCount = msaa.count
			bool Initialize(const Math::Size<unsigned int>& size, bool useShaderResource, uint sampleCount = 0);
			void Destroy();

			void Clear(ID3D11DeviceContext* context, float depth, unsigned char stencil);

		public:
			GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);
		};
	}
}