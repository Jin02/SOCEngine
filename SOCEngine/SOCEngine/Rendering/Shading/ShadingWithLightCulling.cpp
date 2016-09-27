#include "ShadingWithLightCulling.h"
#include "EngineShaderFactory.hpp"
#include "Director.h"
#include "ResourceManager.h"

using namespace Device;
using namespace Core;
using namespace Rendering;
using namespace Resource;
using namespace Rendering::Light;
using namespace Rendering::Buffer;
using namespace Rendering::View;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace GPGPU::DirectCompute;
using namespace Rendering::TBDR;

ShadingWithLightCulling::ShadingWithLightCulling()
	: _diffuseLightBuffer(nullptr), _specularLightBuffer(nullptr),
	_perLightIndicesBuffer(nullptr), _perLightIndicesBufferUAV(nullptr)
{

}

ShadingWithLightCulling::~ShadingWithLightCulling()
{
	Destroy();
}

void ShadingWithLightCulling::Initialize(
	const Texture::DepthBuffer* opaqueDepthBuffer,
	const GBuffers& geometryBuffers,
	const Math::Size<uint>& backBufferSize,
	bool fetchLightIndexBuffer,
	bool useDebugMode)
{
	const Core::Scene* scene				= Director::SharedInstance()->GetCurrentScene();
	Manager::LightManager* lightManager		= scene->GetLightManager();
	Shadow::ShadowRenderer* shadowMgr		= scene->GetShadowManager();

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

		if(shadowMgr->GetNeverUseVSM())
			macros.push_back(ShaderMacro("NEVER_USE_VSM", ""));

		if(fetchLightIndexBuffer)
			macros.push_back(ShaderMacro("STORE_PER_LIGHT_INDICES_IN_TILE", ""));
	}

	LightCulling::Initialize(filePath, "TileBasedDeferredShadingCS", opaqueDepthBuffer, nullptr, &macros);

	// Input Shader Resource Buffers
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightCenterWithDirZ),		lightManager->GetDirectionalLightTransformSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightColor),					lightManager->GetDirectionalLightColorSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightParam),					lightManager->GetDirectionalLightParamSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightShadowParam),			shadowMgr->GetDirectionalLightShadowParamSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightShadowIndex),			lightManager->GetDirectionalLightShadowIndexSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::DirectionalLightShadowViewProjMatrix),	shadowMgr->GetDirectionalLightShadowViewProjSRBuffer());

	// Point Light transform은 LightCulling::Initialize에서 등록하고 있다.
	AddInputBufferToList(uint(TextureBindIndex::PointLightColor),						lightManager->GetPointLightColorSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::PointLightShadowParam),					shadowMgr->GetPointLightShadowParamSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::PointLightShadowIndex),					lightManager->GetPointLightShadowIndexSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::PointLightShadowViewProjMatrix),		shadowMgr->GetPointLightShadowViewProjSRBuffer());

	// Spot Light transform와 Param은 LightCulling::Initialize에서 등록하고 있다.
	AddInputBufferToList(uint(TextureBindIndex::SpotLightColor),						lightManager->GetSpotLightColorSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::SpotLightShadowParam),					shadowMgr->GetSpotLightShadowParamSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::SpotLightShadowIndex),					lightManager->GetSpotLightShadowIndexSRBuffer());
	AddInputBufferToList(uint(TextureBindIndex::SpotLightShadowViewProjMatrix),			shadowMgr->GetSpotLightShadowViewProjSRBuffer());

	// Input Texture
	{
		ResourceManager* resMgr = ResourceManager::SharedInstance();

		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Albedo_Occlusion),					geometryBuffers.albedo_occlusion);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_MotionXY_Metallic_Specularity),		geometryBuffers.motionXY_metallic_specularity);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Normal_Roughness),					geometryBuffers.normal_roughness);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Emission_MaterialFlag),				geometryBuffers.emission_materialFlag);

		// ShadowMap Atlas
		{
			AddTextureToInputTextureList(uint(TextureBindIndex::PointLightShadowMapAtlas),			shadowMgr->GetPointLightShadowMapAtlas());
			AddTextureToInputTextureList(uint(TextureBindIndex::SpotLightShadowMapAtlas),			shadowMgr->GetSpotLightShadowMapAtlas());
			AddTextureToInputTextureList(uint(TextureBindIndex::DirectionalLightShadowMapAtlas),	shadowMgr->GetDirectionalLightShadowMapAtlas());

			if(shadowMgr->GetNeverUseVSM() == false)
			{
				AddTextureToInputTextureList(uint(TextureBindIndex::PointLightMomentShadowMapAtlas),		shadowMgr->GetPointLightMomentShadowMapAtlas());
				AddTextureToInputTextureList(uint(TextureBindIndex::SpotLightMomentShadowMapAtlas),			shadowMgr->GetSpotLightMomentShadowMapAtlas());
				AddTextureToInputTextureList(uint(TextureBindIndex::DirectionalLightMomentShadowMapAtlas),	shadowMgr->GetDirectionalLightMomentShadowMapAtlas());
			}
		}
	}

	// Outputs
	{
		Math::Size<uint> bufferSize = backBufferSize;
		{
			const DXGI_SAMPLE_DESC& msaaDesc = Director::SharedInstance()->GetDirectX()->GetMSAADesc();
			if(msaaDesc.Count > 1)
			{
				bufferSize.w *= 2;
				bufferSize.h *= 2;
			}
		}

		_diffuseLightBuffer = new RenderTexture;
		_diffuseLightBuffer->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);

		_specularLightBuffer = new RenderTexture;
		_specularLightBuffer->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);

		std::vector<ShaderForm::InputUnorderedAccessView> outputs;
		{
			outputs.push_back( ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::TBDR_OutDiffuseLightBuffer), _diffuseLightBuffer->GetUnorderedAccessView()) );
			outputs.push_back( ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::TBDR_OutSpecularLightBuffer), _specularLightBuffer->GetUnorderedAccessView()) );
		}

		if(fetchLightIndexBuffer)
		{
			Math::Size<unsigned int> size = CalcThreadGroupSize();
			uint num = CalcMaxNumLightsInTile() * size.w * size.h;
	
			_perLightIndicesBuffer = new ShaderResourceBuffer;
			_perLightIndicesBuffer->Initialize(4, num, DXGI_FORMAT_R32_UINT, nullptr, false, D3D11_BIND_UNORDERED_ACCESS, D3D11_USAGE_DEFAULT);
	
			_perLightIndicesBufferUAV = new UnorderedAccessView;
			_perLightIndicesBufferUAV->Initialize(DXGI_FORMAT_R32_UINT, num, _perLightIndicesBuffer->GetBuffer(), D3D11_UAV_DIMENSION_BUFFER);

			outputs.push_back( ShaderForm::InputUnorderedAccessView(uint(UAVBindIndex::TBDR_OutPerLightIndicesInTile), _perLightIndicesBufferUAV) );
		}

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void ShadingWithLightCulling::Destroy()
{
	SAFE_DELETE(_diffuseLightBuffer);
	SAFE_DELETE(_specularLightBuffer);

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
		icb.bindIndex	= (uint)ConstBufferBindIndex::ShadowGlobalParam;

		additionalConstBuffers.push_back(icb);
	}

	LightCulling::Dispatch(dx, tbrConstBuffer, &additionalConstBuffers);
}