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
			struct GBuffers
			{
				const Texture::RenderTexture* albedo_emission;
				const Texture::RenderTexture* specular_metallic;
				const Texture::RenderTexture* normal_roughness;
			};

		private:
			GPGPU::DirectCompute::CSRWTexture*					_offScreen;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightTransformBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightParamBuffer;

			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputPointLightShadowColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputSpotLightShadowColorBuffer;
			GPGPU::DirectCompute::ComputeShader::InputShaderResourceBuffer*	_inputDirectionalLightShadowColorBuffer;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const GBuffers& geometryBuffers, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

		public:
			GET_ACCESSOR(OffScreen, const GPGPU::DirectCompute::CSRWTexture*, _offScreen);
		};
	}
}