#include "ShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace GPGPU::DirectCompute;
using namespace Rendering::TBDR;

ShadingWithLightCulling::ShadingWithLightCulling() : 
	_offScreen(nullptr), _inputPointLightColorBuffer(nullptr),
	_inputSpotLightColorBuffer(nullptr), _inputDirectionalLightColorBuffer(nullptr),
	_inputDirectionalLightParamBuffer(nullptr), _inputDirectionalLightTransformBuffer(nullptr),

	_inputDirectionalLightShadowParamBuffer(nullptr), _inputPointLightShadowParamBuffer(nullptr), _inputSpotLightShadowParamBuffer(nullptr),
	_inputDirectionalLightShadowColorBuffer(nullptr), _inputPointLightShadowColorBuffer(nullptr), _inputSpotLightShadowColorBuffer(nullptr)
{

}

ShadingWithLightCulling::~ShadingWithLightCulling()
{
	Destory();
}

void ShadingWithLightCulling::Initialize(
	const Texture::DepthBuffer* opaqueDepthBuffer,
	const GBuffers& geometryBuffers,
	const Math::Size<uint>& backBufferSize,
	bool useDebugMode)
{
	const auto* scene = Director::GetInstance()->GetCurrentScene();
	Manager::LightManager* lightManager = scene->GetLightManager();

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
			uint idx = (uint)InputSRBufferBindSlotIndex::PointLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetPointLightColorSRBuffer();
			AddInputBufferToList(_inputPointLightColorBuffer, idx, srBuffer);
		}

		// Spot Light
		{
			uint idx = (uint)InputSRBufferBindSlotIndex::SpotLightColor;
			const ShaderResourceBuffer* srBuffer = lightManager->GetSpotLightColorSRBuffer();
			AddInputBufferToList(_inputSpotLightColorBuffer, idx, srBuffer);
		}

		// Directional Light
		{
			// Center With DirZ
			{
				uint idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightCenterWithDirZ;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightTransformSRBuffer();
				AddInputBufferToList(_inputDirectionalLightTransformBuffer, idx, srBuffer);
			}

			// Color
			{
				uint idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightColor;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightColorSRBuffer();
				AddInputBufferToList(_inputDirectionalLightColorBuffer, idx, srBuffer);
			}

			// Param half / DirX, DirY
			{
				uint idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightParam;
				const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightParamSRBuffer();
				AddInputBufferToList(_inputDirectionalLightParamBuffer, idx, srBuffer);
			}
		}

		// Shadows
		{
			uint idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightShadowParam;
			const ShaderResourceBuffer* srBuffer = lightManager->GetDirectionalLightShadowParamSRBuffer();
			AddInputBufferToList(_inputDirectionalLightShadowParamBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferBindSlotIndex::PointLightShadowParam;
			srBuffer = lightManager->GetPointLightShadowParamSRBuffer();
			AddInputBufferToList(_inputPointLightShadowParamBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferBindSlotIndex::SpotLightShadowParam;
			srBuffer = lightManager->GetSpotLightShadowParamSRBuffer();
			AddInputBufferToList(_inputSpotLightShadowParamBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightShadowColor;
			srBuffer = lightManager->GetDirectionalLightShadowColorSRBuffer();
			AddInputBufferToList(_inputDirectionalLightShadowColorBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferBindSlotIndex::PointLightShadowColor;
			srBuffer = lightManager->GetPointLightShadowColorSRBuffer();
			AddInputBufferToList(_inputPointLightShadowColorBuffer, idx, srBuffer);

			idx = (uint)InputSRBufferBindSlotIndex::SpotLightShadowColor;
			srBuffer = lightManager->GetSpotLightShadowColorSRBuffer();
			AddInputBufferToList(_inputSpotLightShadowColorBuffer, idx, srBuffer);
		}
	}

	// Input Texture
	{
		// Albedo_Emission
		{
			uint idx = (uint)InputSRBufferBindSlotIndex::GBuffer_Albedo_Emission;
			AddTextureToInputTextureList(idx, geometryBuffers.albedo_emission);
		}

		// Specular_Metallic
		{
			uint idx = (uint)InputSRBufferBindSlotIndex::GBuffer_Specular_Metallic;
			AddTextureToInputTextureList(idx, geometryBuffers.specular_metallic);
		}

		// Normal_Roughness
		{
			uint idx = (uint)InputSRBufferBindSlotIndex::GBuffer_Normal_Roughness;
			AddTextureToInputTextureList(idx, geometryBuffers.normal_roughness);
		}

		// ShadowMap Atlas
		{
			Shadow::ShadowRenderer* shadowMgr = scene->GetShadowManager();

			uint idx = (uint)InputSRBufferBindSlotIndex::PointLightShadowMapAtlas;			
			AddTextureToInputTextureList(idx, shadowMgr->GetPointLightShadowMapAtlas());

			idx = (uint)InputSRBufferBindSlotIndex::SpotLightShadowMapAtlas;
			AddTextureToInputTextureList(idx, shadowMgr->GetSpotLightShadowMapAtlas());

			idx = (uint)InputSRBufferBindSlotIndex::DirectionalLightShadowMapAtlas;
			AddTextureToInputTextureList(idx, shadowMgr->GetDirectionalLightShadowMapAtlas());
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

		_offScreen = new RenderTexture;
		_offScreen->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);

		ComputeShader::Output output;
		{
			output.bindIndex	= (uint)OutputBufferShaderIndex::OutScreen;
			output.output		= _offScreen->GetUnorderedAccessView();
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

	_inputDirectionalLightShadowParamBuffer	= nullptr;
	_inputPointLightShadowParamBuffer		= nullptr;
	_inputSpotLightShadowParamBuffer		= nullptr;
	_inputDirectionalLightShadowColorBuffer	= nullptr;
	_inputPointLightShadowColorBuffer		= nullptr;
	_inputSpotLightShadowColorBuffer		= nullptr;

	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}

void ShadingWithLightCulling::Dispatch(const Device::DirectX* dx,
									   const Buffer::ConstBuffer* tbrConstBuffer,
									   const Buffer::ConstBuffer* shadowGlobalParamConstBuffer)
{
	std::vector<ShaderForm::InputConstBuffer> additionalConstBuffers;
	if(shadowGlobalParamConstBuffer)
	{
		ShaderForm::InputConstBuffer icb;
		icb.buffer		= shadowGlobalParamConstBuffer;
		icb.bindIndex	= (uint)InputConstBufferBindSlotIndex::ShadowGlobalParam;

		additionalConstBuffers.push_back(icb);
	}

	LightCulling::Dispatch(dx, tbrConstBuffer, &additionalConstBuffers);
}