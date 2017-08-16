#include "SSAO.h"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Renderer;
using namespace Rendering::Texture;

void SSAO::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr)
{
	FullScreen::InitParam param;
	{
		param.psMacros = nullptr;
		param.psName = "SSAO_InFullScreen_PS";
		param.shaderFileName = "SSAO";
	}
	_screen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());
}

void SSAO::UpdateParamCB(Device::DirectX& dx)
{
	if(_dirty == false) return;

	_paramCB.UpdateSubResource(dx, _paramData);
	_dirty = false;
}

void SSAO::Render(Device::DirectX& dx, RenderTexture& outRT, RenderTexture& inColorMap, MainRenderer& mainRenderer)
{
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0), inColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth, mainRenderer.GetGBuffers().opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex::GBuffer_Normal_Roughness, mainRenderer.GetGBuffers().normal_roughness.GetTexture2D().GetShaderResourceView());

	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::TBRParam, mainRenderer.GetTBRParamCB());
	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex(1), _paramCB);

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, dx.GetSamplerStateLinear());

	_screen.Render(dx, outRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Depth);
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex::GBuffer_Normal_Roughness);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex::TBRParam);
	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex(1));

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
}