#include "DeferredShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

DeferredShadingWithLightCulling::DeferredShadingWithLightCulling() : 
	_offScreen(nullptr), _inputPointLightColorBuffer(nullptr),
	_inputSpotLightColorBuffer(nullptr), _inputDirectionalLightColorBuffer(nullptr),
	_inputDirectionalLightParamBuffer(nullptr), _inputDirectionalLightTransformBuffer(nullptr),
	_directionalLightUpdateCounter(0)
{
}

DeferredShadingWithLightCulling::~DeferredShadingWithLightCulling()
{
	Destory();
}

void DeferredShadingWithLightCulling::Initialize(const Texture::DepthBuffer* opaqueDepthBuffer,
												 const Texture::RenderTexture* gbuffer_albedo_metallic,  
												 const Texture::RenderTexture* gbuffer_specular_fresnel0, 
												 const Texture::RenderTexture* gbuffer_normal_roughness, 
												 const Math::Size<uint>& size)
{
	Manager::LightManager* lightManager = Director::GetInstance()->GetCurrentScene()->GetLightManager();

	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "TileBasedDeferredShading");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	// Input buffer
	{
		// Point Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::PointLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetPointLightColorBufferSR();
			AddInputBufferToList(_inputPointLightColorBuffer, idx, srBuffer);
		}

		// Spot Light Color
		{
			uint idx = (uint)InputBufferShaderIndex::SpotLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetSpotLightColorBufferSR();
			AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);
		}

		// Directional Light
		{
			// Center With DirZ
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightCenterWithDirZ;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightTransformBufferSR();
				AddInputBufferToList(_inputDirectionalLightTransformBuffer, idx, srBuffer);
			}

			// Color
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightColor;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightColorBufferSR();
				AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);
			}

			// Param half / DirX, DirY
			{
				uint idx = (uint)InputBufferShaderIndex::DirectionalLightParam;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightParamBufferSR();
				AddInputBufferToList(_inputDirectionalLightParamBuffer, idx, srBuffer);
			}
		}
	}

	// Input Texture
	{
		// Albedo_Metallic
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Albedo_Metallic;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_albedo_metallic);
		}

		// Specular_Fresnel0
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Specular_Fresnel0;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_specular_fresnel0);
		}

		// Normal_Roughness
		{
			uint idx = (uint)InputTextureShaderIndex::GBuffer_Normal_Roughness;
			_Set_InputTexture_And_Append_To_InputTextureList(nullptr, idx, gbuffer_normal_roughness);
		}
	}

	// Offscreen
	{
		_offScreen = new CSRWTexture;
		_offScreen->Initialize(size, DXGI_FORMAT_R8G8B8A8_UNORM, 0);

		ComputeShader::Output output;
		{
			output.idx		= (uint)OutputBufferShaderIndex::OutScreen;
			output.output	= _offScreen;
		}

		std::vector<ComputeShader::Output> outputs;
		outputs.push_back(output);

		SetOuputBuferToComputeShader(outputs);
	}

	LightCulling::Initialize(filePath, "CS", false, opaqueDepthBuffer, nullptr);
}

void DeferredShadingWithLightCulling::Destory()
{
	_inputPointLightColorBuffer				= nullptr;
	_inputSpotLightColorBuffer				= nullptr;
	_inputDirectionalLightTransformBuffer	= nullptr;
	_inputDirectionalLightColorBuffer		= nullptr;
	_inputDirectionalLightParamBuffer		= nullptr;

	_directionalLightUpdateCounter = 0;

	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}