#include "SSAO.h"
#include "AutoBinder.hpp"

using namespace Rendering;
using namespace Rendering::Buffer;
using namespace Rendering::Shader;
using namespace Rendering::PostProcessing;
using namespace Rendering::Renderer;
using namespace Rendering::Texture;
using namespace Rendering::RenderState;

void SSAO::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr)
{
	std::vector<ShaderMacro> psMacro{ dx.GetMSAAShaderMacro() };
	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "SSAO_InFullScreen_PS";
		param.shaderFileName	= "SSAO";
		param.psMacros			= &psMacro;
	}
	_screen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());
}

void SSAO::UpdateParamCB(Device::DirectX& dx)
{
	if(_dirty == false)
		return;

	_paramCB.UpdateSubResource(dx, _paramData);
	_dirty = false;
}

void SSAO::Render(Device::DirectX& dx, RenderTexture& outRT, const RenderTexture& inColorMap, const MainRenderer& mainRenderer) const
{	
	AutoBinderSRV<PixelShader> color(dx,		TextureBindIndex(0),						inColorMap.GetTexture2D().GetShaderResourceView());
	AutoBinderSRV<PixelShader> depth(dx,		TextureBindIndex::GBuffer_Depth,			mainRenderer.GetGBuffers().opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
	AutoBinderSRV<PixelShader> normal(dx,		TextureBindIndex::GBuffer_Normal_Roughness,	mainRenderer.GetGBuffers().normal_roughness.GetTexture2D().GetShaderResourceView());
	
	AutoBinderCB<PixelShader> tbrParam(dx,		ConstBufferBindIndex::TBRParam,				mainRenderer.GetTBRParamCB());
	AutoBinderCB<PixelShader> ssaoParam(dx,		ConstBufferBindIndex(1),					_paramCB);

	AutoBinderSampler<PixelShader> sampler(dx,	SamplerStateBindIndex::DefaultSamplerState,	SamplerState::Linear);

	_screen.Render(dx, outRT, true);
}