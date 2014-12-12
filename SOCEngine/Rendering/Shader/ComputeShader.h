#pragma once

#include "BaseShader.h"

namespace Rendering
{
	namespace Shader
	{
		class ComputeShader : public BaseShader
		{
		private:
			ID3D11ComputeShader*	_shader;

		public:
			ComputeShader(ID3DBlob* blob);
			~ComputeShader(void);
		public:
			bool CreateShader();
		};
	}
}
