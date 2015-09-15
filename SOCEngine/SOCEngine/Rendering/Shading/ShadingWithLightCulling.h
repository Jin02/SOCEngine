#pragma once

#include "LightCulling.h"
#include "CSRWTexture.h"

namespace Rendering
{
	namespace TBDR
	{
		class ShadingWithLightCulling : public Light::LightCulling
		{
		public:
			enum class OutputBufferShaderIndex : unsigned int
			{
				OutScreen = 0
			};

		private:
			GPGPU::DirectCompute::CSRWTexture*					_offScreen;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightParamBuffer;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::RenderTexture* gbuffer_albedo_emission, const Texture::RenderTexture* gbuffer_specular_metallic, const Texture::RenderTexture* gbuffer_normal_roughness, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

		public:
			GET_ACCESSOR(OffScreen, const GPGPU::DirectCompute::CSRWTexture*, _offScreen);
		};
	}
}