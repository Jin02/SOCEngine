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

		public:
			PixelShader();
			~PixelShader(void);

		public:
			bool Create();
		};
	}
}
