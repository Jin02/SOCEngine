#pragma once

#include "LightCulling.h"

namespace Rendering
{
	namespace Light
	{
		class OnlyLightCulling : public LightCulling
		{
		public:
			enum class OutputBufferShaderIndex : unsigned int
			{
				LightIndexBuffer = 0
			};

		private:
			GPGPU::DirectCompute::CSRWBuffer*		_lightIndexBuffer;

		public:
			OnlyLightCulling();
			virtual ~OnlyLightCulling();

		public:
			void Initialize(
				const Texture::DepthBuffer* opaqueDepthBuffer, 
				const Texture::DepthBuffer* blendedDepthBuffer,
				RenderType renderType,
				const std::vector<Shader::ShaderMacro>* opationalMacros = nullptr);
			void Destroy();
		};
	}
}