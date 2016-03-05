#pragma once

#include "Texture2D.h"
#include "DirectX.h"

namespace Rendering
{
	namespace Texture
	{
		class DepthBufferCube : public Texture2D
		{
		private:
			ID3D11DepthStencilView		*_depthStencilView;

		public:
			DepthBufferCube(void);
			virtual ~DepthBufferCube();

		public:
			bool Initialize(const Math::Size<unsigned int>& size, bool useShaderResource);
			void Destroy();

			void Clear(ID3D11DeviceContext* context, float depth, unsigned char stencil);

		public:
			GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);
		};
	}
}