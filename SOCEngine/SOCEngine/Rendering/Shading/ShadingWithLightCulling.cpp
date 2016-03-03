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
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace GPGPU::DirectCompute;
using namespace Rendering::TBDR;

ShadingWithLightCulling::ShadingWithLightCulling() : _offScreen(nullptr)
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

		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Albedo_Occlusion),			geometryBuffers.albedo_occlusion);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_MotionXY_Height_Metallic),	geometryBuffers.motionXY_height_metallic);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Normal_Roughness),			geometryBuffers.normal_roughness);
		AddTextureToInputTextureList(uint(TextureBindIndex::GBuffer_Emission_Specularity),					geometryBuffers.emission);

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

	// Offscreen
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

		_offScreen = new RenderTexture;
		_offScreen->Initialize(bufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, D3D11_BIND_UNORDERED_ACCESS, 1);

		ShaderForm::InputUnorderedAccessView output;
		{
			output.bindIndex	= (uint)UAVBindIndex::TBDR_OutScreen;
			output.uav			= _offScreen->GetUnorderedAccessView();
		}

		std::vector<ShaderForm::InputUnorderedAccessView> outputs;
		outputs.push_back(output);

		SetOuputBuferToCS(outputs);
	}

	SetInputsToCS();
}

void ShadingWithLightCulling::Destory()
{
	SAFE_DELETE(_offScreen);
	LightCulling::Destroy();
}

void ShadingWithLightCulling::Dispatch(const Device::DirectX* dx,
									   const Buffer::ConstBuffer* tbrConstBuffer,
									   const Buffer::ConstBuffer* shadowGlobalParamConstBuffer, const Sky::SkyForm* sky)
{
	std::vector<ShaderForm::InputConstBuffer> additionalConstBuffers;
	if(shadowGlobalParamConstBuffer)
	{
		ShaderForm::InputConstBuffer icb;
		icb.buffer		= shadowGlobalParamConstBuffer;
		icb.bindIndex	= (uint)ConstBufferBindIndex::ShadowGlobalParam;

		additionalConstBuffers.push_back(icb);
	}

	ID3D11DeviceContext* context = dx->GetContext();

	//if(sky)
	//{
	//	additionalConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::SkyMapInfoParam), sky->GetSkyMapInfoConstBuffer()));

	//	ID3D11ShaderResourceView* srv = sky->GetSkyCubeMap()->GetShaderResourceView()->GetView();
	//	context->CSSetShaderResources(uint(TextureBindIndex::SkyCubeMap), 1, &srv);
	//	
	//	ID3D11SamplerState* sampler = dx->GetSamplerStateLinear();
	//	context->CSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);
	//}

	LightCulling::Dispatch(dx, tbrConstBuffer, &additionalConstBuffers);

	//if(sky)
	//{
	//	ID3D11ShaderResourceView* srv = nullptr;
	//	context->CSSetShaderResources(uint(TextureBindIndex::SkyCubeMap), 1, &srv);
	//	
	//	ID3D11SamplerState* sampler = nullptr;
	//	context->CSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);
	//}
}