#include "GaussianBlur.h"
#include "BindIndexInfo.h"

#include "AutoBinder.hpp"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Device;

void GaussianBlur::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	// Init Shader
	{
		std::vector<ShaderMacro> macros{ ShaderMacro("BLUR_VERTICAL") };

		FullScreen::InitParam param;
		{
			param.psMacros			= &macros;
			param.psName			= "GuassianBlur_InFullScreen_PS";
			param.shaderFileName	= "GaussianBlur";
		}

		_vertical.Initialize(dx, param, shaderMgr);	

		macros.back().SetName("BLUR_HORIZONTAL");
		_horizontal.Initialize(dx, param, shaderMgr);
	}

	_paramCB.Initialize(dx);
	_paramCB.UpdateSubResource(dx, ParamCBData());
}

void GaussianBlur::Destroy()
{
	_vertical.Destroy();
	_horizontal.Destroy();


	_paramCB.Destroy();
}

void GaussianBlur::UpdateParamCB(Device::DirectX & dx, const ParamCBData & param)
{
	_paramCB.UpdateSubResource(dx, param);
}

void GaussianBlur::Render(Device::DirectX& dx, RenderTexture& outResultRT, const RenderTexture& inputColorMap, RenderTexture& tempMap) const
{
	AutoBinderSRV<PixelShader> inputColorMap(dx,	TextureBindIndex(0),		inputColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderCB<PixelShader> paramCB(dx,			ConstBufferBindIndex(0),	_paramCB);
	AutoBinderSampler<PixelShader> sampler(dx,		SamplerStateBindIndex(0),	SamplerState::Linear);
	_vertical.Render(dx, tempMap, true);

	AutoBinderSRV<PixelShader> tmpMap(dx,			TextureBindIndex(0),		tempMap.GetTexture2D()->GetShaderResourceView());
	_horizontal.Render(dx, outResultRT, true);
}
