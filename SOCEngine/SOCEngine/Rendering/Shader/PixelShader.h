#pragma once

#include "BaseShader.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public BaseShader
		{
		private:
			ID3D11PixelShader* _shader;

		public:
			PixelShader(ID3DBlob* blob);
			~PixelShader(void);

		public:
			bool CreateShader();
			
			void UpdateShader(ID3D11DeviceContext* context);
			void UpdateResources(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<TextureType>* textures);

			void ClearResource(ID3D11DeviceContext* context, const std::vector<TextureType>* textures);
		};
	}
}
