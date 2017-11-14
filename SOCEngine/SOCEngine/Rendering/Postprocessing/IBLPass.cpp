#include "IBLPass.h"
#include "BindIndexInfo.h"
#include "SkyBox.h"
#include "SkyBoxMaterial.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Rendering::Sky;
using namespace Rendering::Manager;
using namespace Rendering::Material;
using namespace Rendering::RenderState;
using namespace Rendering;
using namespace Device;

void IBLPass::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "PS";
		param.shaderFileName	= "IBLPass";
	}

	_screen.Initialize(dx, param, shaderMgr);

	_envBRDFMap.CreatePreBRDFMap(dx, shaderMgr);
}

void IBLPass::Render(	DirectX& dx, RenderTexture& outResultRT,
						const MainRenderingSystemParam&& mains, const SkyBoxMaterial& skyBox) const
{

	auto& gbuffer = mains.renderer.GetGBuffers();
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Albedo_Occlusion,					gbuffer.albedo_occlusion.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Emission_MaterialFlag,			gbuffer.emission_materialFlag.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	gbuffer.velocity_metallic_specularity.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Normal_Roughness,					gbuffer.normal_roughness.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth,							gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());

	PixelShader::BindShaderResourceView(dx, TextureBindIndex::IBLPass_IlluminationMap,					mains.renderer.GetResultMap().GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::IBLPass_PreIntegrateEnvBRDFMap,			_envBRDFMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::AmbientCubeMap,							skyBox.GetCubeMap().GetShaderResourceView());

	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam,	mains.renderer.GetTBRParamCB());
	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::Camera,		mains.camera.GetCameraCB());

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::AmbientCubeMapSamplerState, SamplerState::Linear);

	_screen.Render(dx, outResultRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Albedo_Occlusion);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Emission_MaterialFlag);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Velocity_Metallic_Specularity);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Normal_Roughness);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::IBLPass_IlluminationMap);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::IBLPass_PreIntegrateEnvBRDFMap);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::AmbientCubeMap);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::Camera);

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::AmbientCubeMapSamplerState);
}
