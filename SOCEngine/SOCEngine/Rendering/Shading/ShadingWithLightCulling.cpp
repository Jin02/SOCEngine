#include "ShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;
using namespace Rendering::TBDR;

ShadingWithLightCulling::ShadingWithLightCulling() : 
	_offScreen(nullptr), _inputPointLightColorBuffer(nullptr),
	_inputSpotLightColorBuffer(nullptr), _inputDirectionalLightColorBuffer(nullptr),
	_inputDirectionalLightParamBuffer(nullptr), _inputDirectionalLightTransformBuffer(nullptr),
	_inputPointLightShadowColorBuffer(nullptr), _inputSpotLightShadowColorBuffer(nullptr), _inputDirectionalLightShadowColorBuffer(nullptr)
{
}

ShadingWithLightCulling::~ShadingWithLightCulling()
{
	Destory();
}

void ShadingWithLightCulling::Initialize(
	const Texture::DepthBuffer* opaqueDepthBuffer,
	const ShadowMapAtlases& shadowMapAtlases,
	const GBuffers& geometryBuffers,
	const Math::Size<uint>& backBufferSize,
	bool useDebugMode)
{
	Manager::LightManager* lightManager = Director::GetInstance()->GetCurrentScene()->GetLightManager();

	std::string filePath = "";
	{
		Factory::EngineFactory pathFind(nullptr);
		pathFind.FetchShaderFullPath(filePath, "TBDR");

		ASSERT_COND_MSG(filePath.empty() == false, "Error, File path is empty");
	}

	std::vector<ShaderMacro> macros;
	{
		if(useDebugMode)
			macros.push_back(ShaderMacro("DEBUG_MODE", ""));
	}

	LightCulling::Initialize(filePath, "TileBasedDeferredShadingCS", opaqueDepthBuffer, nullptr, &macros);

	// Additional Input buffer
	{
		// Point Light
		{
			uint idx = (uint)InputSRBufferSemanticIndex::PointLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetPointLightColorBufferSR();
			AddInputBufferToList(_inputPointLightColorBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferSemanticIndex::PointLightShadowColor;
			srBuffer = lightManager->GetPointLightShadowColorBufferSR();
			AddInputBufferToList(_inputPointLightShadowColorBuffer, idx, srBuffer);
		}

		// Spot Light
		{
			uint idx = (uint)InputSRBufferSemanticIndex::SpotLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetSpotLightColorBufferSR();
			AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferSemanticIndex::SpotLightShadowColor;
			srBuffer = lightManager->GetSpotLightShadowColorBufferSR();
			AddInputBufferToList(_inputSpotLightShadowColorBuffer, idx, srBuffer);
		}

		// Directional Light
		{
			// Center With DirZ
			{
				uint idx = (uint)InputSRBufferSemanticIndex::DirectionalLightCenterWithDirZ;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightTransformBufferSR();
				AddInputBufferToList(_inputDirectionalLightTransformBuffer, idx, srBuffer);
			}

			// Color
			{
				uint idx = (uint)InputSRBufferSemanticIndex::DirectionalLightColor;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightColorBufferSR();
				AddInputBufferToList(_inputDirectionalLightColorBuffer, idx, srBuffer);

				idx = (uint)InputSRBufferSemanticIndex::DirectionalLightShadowColor;
				srBuffer = lightManager->GetDirectionalLightShadowColorBufferSR();
				AddInputBufferToList(_inputDirectionalLightShadowColorBuffer, idx, srBuffer);
			}

			// Param half / DirX, DirY
			{
				uint idx = (uint)InputSRBufferSemanticIndex::DirectionalLightParam;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightParamBufferSR();
				AddInputBufferToList(_inputDirectionalLightParamBuffer, idx, srBuffer);
			}
		}
	}

	// Input Texture
	{
		// Albedo_Emission
		{
			uint idx = (uint)InputSRBufferSemanticIndex::GBuffer_Albedo_Emission;
			AddTextureToInputTextureList(idx, geometryBuffers.albedo_emission);
		}

		// Specular_Metallic
		{
			uint idx = (uint)InputSRBufferSemanticIndex::GBuffer_Specular_Metallic;
			AddTextureToInputTextureList(idx, geometryBuffers.specular_metallic);
		}

		// Normal_Roughness
		{
			uint idx = (uint)InputSRBufferSemanticIndex::GBuffer_Normal_Roughness;
			AddTextureToInputTextureList(idx, geometryBuffers.normal_roughness);
		}

		// ShadowMap Atlas
		{
			uint idx = (uint)InputSRBufferSemanticIndex::PointLightShadowMapAtlas;
			AddTextureToInputTextureList(idx, shadowMapAtlases.pointLightDepthBuffer);

			idx = (uint)InputSRBufferSemanticIndex::SpotLightShadowMapAtlas;
			AddTextureToInputTextureList(idx, shadowMapAtlases.spotLightDepthBuffer);

			idx = (uint)InputSRBufferSemanticIndex::DirectionalLightShadowMapAtlas;
			AddTextureToInputTextureList(idx, shadowMapAtlases.directionalLightDepthBuffer);
		}
	}

	// Offscreen
	{
		Math::Size<uint> bufferSize = backBufferSize;
		{
			const DXGI_SAMPLE_DESC& msaaDesc = Director::GetInstance()->GetDirectX()->GetMSAADesc();
			if(msaaDesc.Count > 1)
			{
				bufferSize.w *= 2;
				bufferSize.h *= 2;
			}
		}

		_offScreen = new CSRWTexture;
		_offScreen->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, 0);

		ComputeShader::Output output;
		{
			output.idx		= (uint)OutputBufferShaderIndex::OutScreen;
			output.output	= _offScreen;
		}

		std::vector<ComputeShader::Output> outputs;
		outputs.push_back(output);

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void ShadingWithLightCulling::Destory()
{
	_inputPointLightColorBuffer				= nullptr;
	_inputSpotLightColorBuffer				= nullptr;
	_inputDirectionalLightTransformBuffer	= nullptr;
	_inputDirectionalLightColorBuffer		= nullptr;
	_inputDirectionalLightParamBuffer		= nullptr;
	_inputPointLightShadowColorBuffer		= nullptr;
	_inputSpotLightShadowColorBuffer		= nullptr;
	_inputDirectionalLightShadowColorBuffer	= nullptr;

	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}