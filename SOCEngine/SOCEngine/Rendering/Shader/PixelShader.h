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
			virtual ~PixelShader(void);

		public:
			bool CreateShader();
			
			void SetShaderToContext(ID3D11DeviceContext* context);
			void UpdateResources(ID3D11DeviceContext* context, 
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*				textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);

			void Clear(ID3D11DeviceContext* context,
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*				textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);
		};
	}
}
