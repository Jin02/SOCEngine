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

void IBLPass::Initialize(const SkyForm* sky)
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());
	
	std::vector<ShaderForm::InputTexture> inputTextures;
	{
		#define AddInputTexture(bind, texture)	inputTextures.push_back(ShaderForm::InputTexture(uint(bind), texture, false, false, false, true));

		const Texture2D* preIntegrateEnvBRDFMap = ResourceManager::SharedInstance()->GetPreIntegrateEnvBRDFMap();
		AddInputTexture(TextureBindIndex::PreIntegrateEnvBRDFMap, preIntegrateEnvBRDFMap);
		AddInputTexture(TextureBindIndex::SkyCubeMap, sky->GetSkyCubeMap());
	}

	std::vector<ShaderForm::InputConstBuffer> inputConstBuffers;
	{
		inputConstBuffers.push_back(ShaderForm::InputConstBuffer(uint(ConstBufferBindIndex::SkyMapInfoParam), sky->GetSkyMapInfoConstBuffer(),	false, false, false, true));
	}

	FullScreen::Initialize("IBLPass", "PS", &macros);
}

void IBLPass::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const Camera::MeshCamera* meshCam)
{
	auto BindTexturesToPixelShader = [](ID3D11DeviceContext* context, TextureBindIndex bind, const Texture2D* tex)
	{
		ID3D11ShaderResourceView* srv = tex ? tex->GetShaderResourceView()->GetView() : nullptr;
		context->PSSetShaderResources(uint(bind), 1, &srv);
	};

	ID3D11DeviceContext* context	= dx->GetContext();

	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Albedo_Occlusion,			meshCam->GetGBufferAlbedoOcclusion());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Emission_Specularity,		meshCam->GetGBufferEmissionSpecularity());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_MotionXY_Height_Metallic,	meshCam->GetGBufferMotionXYHeightMetallic());
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Normal_Roughness,			meshCam->GetGBufferNormalRoughness());	
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Depth,						meshCam->GetOpaqueDepthBuffer());

	ID3D11Buffer* buffer = meshCam->GetTBRParamConstBuffer()->GetBuffer();
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &buffer);

	ID3D11SamplerState* sampler		= dx->GetSamplerStateLinear();
	context->PSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);

	FullScreen::Render(dx, outResultRT);

	sampler = nullptr;
	context->PSSetSamplers(uint(SamplerStateBindIndex::SkyCubeMapSamplerState), 1, &sampler);

	buffer = nullptr;
	context->PSSetConstantBuffers(uint(ConstBufferBindIndex::TBRParam), 1, &buffer);

	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Albedo_Occlusion,			nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Emission_Specularity,		nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_MotionXY_Height_Metallic,	nullptr);
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Normal_Roughness,			nullptr);	
	BindTexturesToPixelShader(context, TextureBindIndex::GBuffer_Depth,						nullptr);
}