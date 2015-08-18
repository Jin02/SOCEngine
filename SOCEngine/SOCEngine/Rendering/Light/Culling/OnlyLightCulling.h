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
			void Initialize(const std::string& filePath, const std::string& mainFunc, bool useRenderBlendedMesh, const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::DepthBuffer* blendedDepthBuffer);
			void Destroy();
		};
	}
}