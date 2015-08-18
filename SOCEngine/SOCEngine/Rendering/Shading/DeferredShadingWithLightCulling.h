#pragma once

#include "LightCulling.h"
#include "CSRWTexture.h"

namespace Rendering
{
	class DeferredShadingWithLightCulling : public Light::LightCulling
	{
	public:
		enum class InputBufferShaderIndex : unsigned int
		{
		//	PointLightRadiusWithCenter		= 0,
			PointLightColor					= 1,
		//	SpotLightRadiusWithCenter		= 2,
			SpotLightColor					= 3,
		//	SpotLightParam					= 4,
			DirectionalLightCenterWithDirZ	= 5,
			DirectionalLightColor			= 6,
			DirectionalLightParam			= 7
		};

		enum class InputTextureShaderIndex : unsigned int
		{
			GBuffer_Albedo_Metallic			= 8,
			GBuffer_Specular_Fresnel0		= 9,
			GBuffer_Normal_Roughness		= 10,
		//	GBuffer_Depth					= 11,
		//	GBuffer_BlendedDepth			= 12
		};

		enum class OutputBufferShaderIndex : unsigned int
		{
			OutScreen = 0
		};

		enum class CostBufferShaderIndex : unsigned int
		{
			TileBasedDeferredShadingParam	= 0,
		//	GlobalData						= 1
		};

		static const int DIRECTIONAL_LIGHT_BUFFER_MAX_NUM	= 1024;

	private:
		GPGPU::DirectCompute::CSRWTexture*					_offScreen;

		GPGPU::DirectCompute::ComputeShader::InputBuffer*	_inputPointLightColorBuffer;
		GPGPU::DirectCompute::ComputeShader::InputBuffer*	_inputSpotLightColorBuffer;
		GPGPU::DirectCompute::ComputeShader::InputBuffer*	_inputDirectionalLightTransformBuffer;
		GPGPU::DirectCompute::ComputeShader::InputBuffer*	_inputDirectionalLightColorBuffer;
		GPGPU::DirectCompute::ComputeShader::InputBuffer*	_inputDirectionalLightParamBuffer;

		uint _directionalLightUpdateCounter;

	public:
		DeferredShadingWithLightCulling();
		virtual ~DeferredShadingWithLightCulling();

	public:
		void Initialize(const Texture::DepthBuffer* opaqueDepthBuffer, const Texture::RenderTexture* gbuffer_albedo_metallic, const Texture::RenderTexture* gbuffer_specular_fresnel0, const Texture::RenderTexture* gbuffer_normal_roughness, const Math::Size<uint>& screenSize);
		void UpdateInputDatas(const Device::DirectX* dx, const GlobalData* globalData, const Rendering::Manager::LightManager* lightManager);

		void Destory();

	public:
		GET_ACCESSOR(OffScreen, const GPGPU::DirectCompute::CSRWTexture*, _offScreen);
	};
}