#pragma once

#include "LightCulling.h"
#include "ShaderResourceBuffer.h"
#include "UnorderedAccessView.h"

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
			Buffer::ShaderResourceBuffer*	_shaderResourceBuffer;
			View::UnorderedAccessView*		_uav;

		public:
			OnlyLightCulling();
			virtual ~OnlyLightCulling();

		public:
			void Initialize(
				const Texture::DepthBuffer* opaqueDepthBuffer, 
				const Texture::DepthBuffer* blendedDepthBuffer,
				const std::vector<Shader::ShaderMacro>* opationalMacros = nullptr);
			void Destroy();

		public:
			GET_ACCESSOR(LightIndexSRBuffer,	const Buffer::ShaderResourceBuffer*,	_shaderResourceBuffer);
			GET_ACCESSOR(LightIndexUAV,			const View::UnorderedAccessView*,		_uav);
		};
	}
}