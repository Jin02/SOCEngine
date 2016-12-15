#pragma once

#include "ShaderForm.h"
#include "ConstBuffer.h"
#include "BindIndexInfo.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader : public ShaderForm
		{
		public:
			struct SemanticInfo
			{
				std::string name;
				unsigned int semanticIndex;
				unsigned int size;
			};

		private:
			std::vector<SemanticInfo> _semanticInfo;

		private:
			ID3D11VertexShader* _shader;
			ID3D11InputLayout*	_layout;

		public:
			VertexShader(ID3DBlob* blob, const std::string& key);
			virtual ~VertexShader(void);

		public:
			GET_ACCESSOR(SemanticInfos, const std::vector<SemanticInfo>&, _semanticInfo);

		public:
			bool Create(const Device::DirectX* dx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);
			void Clear(ID3D11DeviceContext* context,
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*			textures,
				const std::vector<InputShaderResourceBuffer>*		srBuffers);

		public:
			void BindShaderToContext(ID3D11DeviceContext* context);
			void BindInputLayoutToContext(ID3D11DeviceContext* context);
			void UnBindBasicInputs(ID3D11DeviceContext* context);

			void BindResourcesToContext(ID3D11DeviceContext* context,
				const std::vector<InputConstBuffer>*			constBuffers, 
				const std::vector<InputTexture>*			textures,
				const std::vector<InputShaderResourceBuffer>*		srBuffers);
				
			static void BindTexture(ID3D11DeviceContext* context, TextureBindIndex bind, const Texture::TextureForm* tex);
			static void BindSamplerState(ID3D11DeviceContext* context, SamplerStateBindIndex bind, ID3D11SamplerState* samplerState);
			static void BindConstBuffer(ID3D11DeviceContext* context, ConstBufferBindIndex bind, const Buffer::ConstBuffer* cb);
			static void BindShaderResourceBuffer(ID3D11DeviceContext* context, TextureBindIndex bind, const Buffer::ShaderResourceBuffer* srBuffer);
		};
	}
}
