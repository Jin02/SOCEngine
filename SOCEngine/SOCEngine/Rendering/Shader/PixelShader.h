#pragma once

#include "ShaderForm.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public ShaderForm
		{
		private:
			ID3D11PixelShader* _shader;

		public:
			PixelShader(ID3DBlob* blob, const std::string& key);
			virtual ~PixelShader(void);

		public:
			bool Create(ID3D11Device* device);
			
			void BindShaderToContext(ID3D11DeviceContext* context);
			void UnBindShaderToContext(ID3D11DeviceContext* context);

			void BindResourcesToContext(ID3D11DeviceContext*	context, 
				const std::vector<InputConstBuffer>*		constBuffers, 
				const std::vector<InputTexture>*		textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);

			void Clear(ID3D11DeviceContext*				context,
				const std::vector<InputConstBuffer>*		constBuffers, 
				const std::vector<InputTexture>*		textures,
				const std::vector<InputShaderResourceBuffer>*	srBuffers);
			
			static void BindTexture(ID3D11DeviceContext* context,				TextureBindIndex bind,		const Texture::TextureForm* tex);
			static void BindSamplerState(ID3D11DeviceContext* context,			SamplerStateBindIndex bind,	ID3D11SamplerState* samplerState);
			static void BindConstBuffer(ID3D11DeviceContext* context,			ConstBufferBindIndex bind,	const Buffer::ConstBuffer* cb);
			static void BindShaderResourceBuffer(ID3D11DeviceContext* context,	TextureBindIndex bind,		const Buffer::ShaderResourceBuffer* srBuffer);
		};
	}
}
