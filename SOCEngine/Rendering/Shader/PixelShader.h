#pragma once

#include "Shader.h"
#include "Sampler.h"

namespace Rendering
{
	namespace Shader
	{
		class PixelShader : public Shader
		{
		private:
			ID3D11PixelShader* _shader;

		public:
			PixelShader(ID3DBlob* blob);
			~PixelShader(void);

		public:
			bool CreateShader();

			typedef std::pair<int, const Sampler*> SamplerType;
			void UpdateShader(const std::vector<BufferType>& constBuffers, const std::vector<TextureType>& textures, const SamplerType& sampler);
		};
	}
}
