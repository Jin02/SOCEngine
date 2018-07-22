#include "MotionBlur.h"
#include "BindIndexInfo.h"
#include "AutoBinder.hpp"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Rendering::Light;
using namespace Rendering::Camera;
using namespace Rendering::Renderer;
using namespace Device;
using namespace Core;
using namespace Math;

void MotionBlur::Initialize(DirectX& dx, ShaderManager& shaderMgr)
{
	std::vector<ShaderMacro> psMacro{ dx.GetMSAAShaderMacro() };

	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "MotionBlurPS";
		param.shaderFileName	= "MotionBlur";
		param.psMacros			= &psMacro;
	}

	_screen.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
}

void MotionBlur::Destroy()
{
	_screen.Destroy();
}

void MotionBlur::UpdateParamCB(DirectX& dx)
{
	if(_dirty == false)
		return;

	_paramCB.UpdateSubResource(dx, _paramData);
	_dirty = false;
}

void MotionBlur::Render(DirectX& dx, RenderTexture& outRT, const RenderTexture& inColorMap, const MainRenderer& mainRenderer, const ConstBuffer& hdrGlobalParamCB)
{
	AutoBinderSRV<PixelShader> color(dx,	TextureBindIndex(0),										inColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> velocity(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,	mainRenderer.GetGBuffers().velocity_metallic_specularity.GetTexture2D()->GetShaderResourceView());
	AutoBinderCB<PixelShader> tbrParam(dx,	ConstBufferBindIndex::TBRParam,								mainRenderer.GetTBRParamCB());
	AutoBinderCB<PixelShader> hdrParam(dx,	ConstBufferBindIndex::HDRGlobalParamCB,						hdrGlobalParamCB);
	AutoBinderCB<PixelShader> param(dx,		ConstBufferBindIndex(2),									_paramCB);

	AutoBinderSampler<PixelShader> pointSampler(dx,		SamplerStateBindIndex(0), SamplerState::Point);
	AutoBinderSampler<PixelShader> linearSampler(dx,	SamplerStateBindIndex(1), SamplerState::Linear);

	_screen.Render(dx, outRT, true);
}
