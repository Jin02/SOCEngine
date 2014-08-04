#pragma once

#include "Shader.h"
#include "ConstBuffer.h"

namespace Rendering
{
	namespace Shader
	{
		class VertexShader : public Shader
		{
		private:
			ID3D11VertexShader* _shader;
			ID3D11InputLayout*	_layout;

		public:
			VertexShader(ID3DBlob* blob);
			~VertexShader(void);

		public:
			bool CreateShader(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count);

			void UpdateShader(const std::vector<BufferType>& constBuffers, const std::vector<TextureType>& textures);
		};
	}
}
