#include "MainSceneMaker.h"
#include "AutoBinder.hpp"

using namespace Rendering;
using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering::Manager;
using namespace Rendering::Texture;
using namespace Rendering::RenderState;
using namespace Rendering::PostProcessing;

void MainSceneMaker::Initialize(DirectX& dx, ShaderManager& shaderMgr, const Size<uint>& renderSize)
{
	std::vector<ShaderMacro> macros{ dx.GetMSAAShaderMacro() };

	FullScreen::InitParam param;
	{
		param.shaderFileName	= "MainSceneMaker";
		param.psName			= "PS";
		param.psMacros			= &macros;
	}

	_screen.Initialize(dx, param, shaderMgr);
}

void MainSceneMaker::Render(DirectX& dx, RenderTexture& output, const RenderParam&& param)
{
	AutoBinderSRV<PixelShader>	colorMap(dx,		TextureBindIndex(0),			param.opaqueMap.GetShaderResourceView());
	AutoBinderSRV<PixelShader>	giMap(dx,			TextureBindIndex(1),			param.giMap.GetShaderResourceView());
	AutoBinderSRV<PixelShader>	transparentMap(dx,	TextureBindIndex(2),			param.transparentMap.GetShaderResourceView());

	AutoBinderSampler<PixelShader> sampler(dx,		SamplerStateBindIndex(0),		SamplerState::Linear);

	_screen.Render(dx, output, true);
}
