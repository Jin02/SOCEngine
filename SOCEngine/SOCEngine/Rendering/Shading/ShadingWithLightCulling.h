#pragma once

#include "LightCulling.h"
#include "RenderTexture.h"

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
			Rendering::Texture::RenderTexture*				_offScreen;

			Shader::ShaderForm::InputShaderResourceBuffer*	_inputPointLightColorBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputSpotLightColorBuffer;

			Shader::ShaderForm::InputShaderResourceBuffer*	_inputDirectionalLightTransformBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputDirectionalLightColorBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputDirectionalLightParamBuffer;

			Shader::ShaderForm::InputShaderResourceBuffer*	_inputDirectionalLightShadowParamBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputPointLightShadowParamBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputSpotLightShadowParamBuffer;

			Shader::ShaderForm::InputShaderResourceBuffer*	_inputDirectionalLightShadowColorBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputPointLightShadowColorBuffer;
			Shader::ShaderForm::InputShaderResourceBuffer*	_inputSpotLightShadowColorBuffer;

		public:
			ShadingWithLightCulling();
			virtual ~ShadingWithLightCulling();

		public:
			void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const GBuffers& geometryBuffers, const Math::Size<uint>& backBufferSize, bool useDebugMode = false);
			void Destory();

			void Dispatch(const Device::DirectX* dx, const Buffer::ConstBuffer* tbrConstBuffer, const Buffer::ConstBuffer* shadowGlobalParamConstBuffer);

		public:
			GET_ACCESSOR(OffScreen, const Rendering::Texture::RenderTexture*, _offScreen);
		};
	}
}