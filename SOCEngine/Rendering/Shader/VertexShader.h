#pragma once

#include "Shader.h"

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
			VertexShader();
			~VertexShader(void);

		public:
			bool Create(const D3D11_INPUT_ELEMENT_DESC* vertexDeclations, unsigned int count);
		};
	}
}
