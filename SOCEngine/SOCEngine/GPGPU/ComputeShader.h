#pragma once

#include "BaseShader.h"

namespace GPGPU
{
	namespace DirectCompute
	{
		class ComputeShader : public Rendering::Shader::BaseShader
		{
		private:
			ID3D11ComputeShader*	_shader;

		public:
			ComputeShader(ID3DBlob* blob);
			~ComputeShader(void);
		public:
			bool Create();
		};
	}
}
