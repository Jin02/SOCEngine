#pragma once

#include "Shader.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public Shader
		{
		private:
			ID3D11PixelShader* _shader;
			ID3D11InputLayout*	_layout;

		public:
			PixelShader();
			~PixelShader(void);

		public:
			bool Create();
		};
	}
}
