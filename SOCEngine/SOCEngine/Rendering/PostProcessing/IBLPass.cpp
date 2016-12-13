#include "IBLPass.h"
#include "Director.h"
#include "BindIndexInfo.h"
#include "ResourceManager.h"
#include "SkyBox.h"

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

void IBLPass::Initialize()
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());
	FullScreen::Initialize("IBLPass", "PS", true, &macros);
	
	const Texture2D* preIntegrateEnvBRDFMap = ResourceManager::SharedInstance()->GetPreIntegrateEnvBRDFMap();
	_inputTextures.push_back(ShaderForm::InputTexture(uint(TextureBindIndex::IBLPass_PreIntegrateEnvBRDFMap), preIntegrateEnvBRDFMap, false, false, false, true));
}

void IBLPass::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const MeshCamera* meshCam, const SkyForm* sky)
{
	ID3D11DeviceContext* context	= dx->GetContext();

	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Albedo_Occlusion,				meshCam->GetGBufferAlbedoOcclusion());
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Emission_MaterialFlag,			meshCam->GetGBufferEmissionMaterialFlag());
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_MotionXY_Metallic_Specularity,		meshCam->GetGBufferMotionXYMetallicSpecularity());
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Normal_Roughness,				meshCam->GetGBufferNormalRoughness());	
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Depth,					meshCam->GetOpaqueDepthBuffer());
	PixelShader::BindTexture(context, TextureBindIndex::IBLPass_IlluminationMap,				meshCam->GetRenderTarget());

	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::TBRParam, meshCam->GetTBRParamConstBuffer());
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::Camera, meshCam->GetCameraConstBuffer());

	// Sky Cube Map
	if(sky)
	{
		const Texture2D* cubeMap = nullptr;

		if(sky->GetType() == SkyForm::Type::Box)
			cubeMap = dynamic_cast<const SkyBox*>(sky)->GetSkyCubeMap();
		else
			ASSERT_MSG("cant support");

		PixelShader::BindTexture(context, TextureBindIndex::AmbientCubeMap, cubeMap);
		PixelShader::BindConstBuffer(context, ConstBufferBindIndex::SkyMapInfoParam, sky->GetSkyMapInfoConstBuffer());
	}

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::AmbientCubeMapSamplerState, dx->GetSamplerStateLinear());

	FullScreen::Render(dx, outResultRT);

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::AmbientCubeMapSamplerState,	nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::SkyMapInfoParam,			nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::TBRParam,				nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex::Camera,				nullptr);

	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Albedo_Occlusion,			nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Emission_MaterialFlag,		nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_MotionXY_Metallic_Specularity,	nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Normal_Roughness,			nullptr);	
	PixelShader::BindTexture(context, TextureBindIndex::GBuffer_Depth,				nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::AmbientCubeMap,				nullptr);
	PixelShader::BindTexture(context, TextureBindIndex::IBLPass_IlluminationMap,			nullptr);
}
