#include "IBLPass.h"
#include "Director.h"
#include "BindIndexInfo.h"
#include "ResourceManager.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::TBDR;
using namespace Rendering::Sky;
using namespace Device;
using namespace Resource;

IBLPass::IBLPass() : FullScreen()
{
}

IBLPass::~IBLPass()
{
}

void IBLPass::Initialize(const MeshCamera* meshCam, const SkyForm* sky)
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());
	
	std::vector<ShaderForm::InputTexture> inputTextures;
	{
		#define AddInputTexture(bind, texture)	inputTextures.push_back(ShaderForm::InputTexture(uint(bind), texture, false, false, false, true));

		AddInputTexture(TextureBindIndex::GBuffer_Albedo_Occlusion, meshCam->GetGBufferAlbedoOcclusion());
		AddInputTexture(TextureBindIndex::GBuffer_Emission_Specularity, meshCam->GetGBufferEmissionSpecularity());
		AddInputTexture(TextureBindIndex::GBuffer_MotionXY_Height_Metallic, meshCam->GetGBufferMotionXYHeightMetallic());
		AddInputTexture(TextureBindIndex::GBuffer_Normal_Roughness, meshCam->GetGBufferNormalRoughness());	
		AddInputTexture(TextureBindIndex::GBuffer_Depth, meshCam->GetOpaqueDepthBuffer());

		const Texture2D* preIntegrateEnvBRDFMap = ResourceManager::SharedInstance()->GetPreIntegrateEnvBRDFMap();
		AddInputTexture(TextureBindIndex::PreIntegrateEnvBRDFMap, preIntegrateEnvBRDFMap);
		AddInputTexture(TextureBindIndex::SkyCubeMap, sky->GetSkyCubeMap());
	}

	std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;
	{
		inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::TBRParam), meshCam->GetTBRParamConstBuffer(),		false, false, false, true));
		inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::SkyMapInfoParam), sky->GetSkyMapInfoConstBuffer(),	false, false, false, true));
	}

	FullScreen::Initialize("IBLPass", "PS", &macros);
}

void IBLPass::Render(const Device::DirectX* dx, const RenderTexture* outResultRT)
{
	ID3D11DeviceContext* context	= dx->GetContext();

	ID3D11SamplerState* sampler		= dx->GetSamplerStateLinear();
	context->PSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);

	FullScreen::Render(dx, outResultRT);

	sampler = nullptr;
	context->PSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);
}