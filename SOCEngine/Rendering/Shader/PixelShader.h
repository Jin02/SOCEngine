#pragma once

#include "BaseShader.h"
#include "Sampler.h"

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
			~PixelShader(void);

		public:
			bool CreateShader();

			typedef std::pair<int, const Sampler*> SamplerType;
			void UpdateShader(ID3D11DeviceContext* context, const std::vector<BufferType>* constBuffers, const std::vector<const Texture::Texture*>& textures, const SamplerType& sampler);
		};
	}
}
