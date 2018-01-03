#include "BilateralFiltering.h"
#include "BindIndexInfo.h"

#include "AutoBinder.hpp"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Device;

void BilateralFiltering::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr,
									const Size<uint>& size, DXGI_FORMAT format)
{
	std::vector<ShaderMacro> macros{ ShaderMacro("BLUR_VERTICAL") };

	FullScreen::InitParam param;
	{
		param.psMacros			= &macros;
		param.psName			= "BilateralGauss_InFullScreen_PS";
		param.shaderFileName	= "BilateralFiltering";
	}
	_vertical.Initialize(dx, param, shaderMgr);
	
	macros.back().SetName("BLUR_HORIZONTAL");
	_horizontal.Initialize(dx, param, shaderMgr);

	_paramCB.Initialize(dx);
	UpdateParamCB(dx, ParamCBData());
}

void BilateralFiltering::UpdateParamCB(Device::DirectX& dx, const ParamCBData& data)
{
	_paramCB.UpdateSubResource(dx, data);
}

void BilateralFiltering::Render(Device::DirectX& dx, RenderTexture& outResultRT,
								const DepthMap& depthMap, const RenderTexture& inputColorMap, RenderTexture& tempMap) const
{
	AutoBinderSRV<PixelShader> inputColorMap(dx,		TextureBindIndex(0),		inputColorMap.GetTexture2D()->GetShaderResourceView());
	AutoBinderSRV<PixelShader> depthMap(dx,				TextureBindIndex(1),		depthMap.GetTexture2D().GetShaderResourceView());

	AutoBinderSampler<PixelShader> linearSampler(dx,	SamplerStateBindIndex(0),	SamplerState::Linear);
	AutoBinderSampler<PixelShader> shadowLinear(dx,		SamplerStateBindIndex(1),	SamplerState::ShadowLinear);

	AutoBinderCB<PixelShader> paramCB(dx,				ConstBufferBindIndex(0),	_paramCB);
	_vertical.Render(dx, tempMap, true);

	AutoBinderSRV<PixelShader> tmpMap(dx,				TextureBindIndex(0),		tempMap.GetTexture2D()->GetShaderResourceView());
	_horizontal.Render(dx, outResultRT, true);
}