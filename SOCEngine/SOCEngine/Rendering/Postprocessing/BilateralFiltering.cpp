#include "BilateralFiltering.h"
#include "BindIndexInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
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
								DepthMap& depthMap, RenderTexture& inputColorMap, RenderTexture& tempMap)
{
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex(0),						inputColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex(1),						depthMap.GetTexture2D().GetShaderResourceView());

	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex::DefaultSamplerState,	dx.GetSamplerStateLinear());
	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex(1),					dx.GetSamplerStateShadowLinear());

	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex(0),					_paramCB);

	_vertical.Render(dx, tempMap, true);

	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0),						tempMap.GetTexture2D().GetShaderResourceView());
	_horizontal.Render(dx, outResultRT, true);

	PixelShader::UnBindConstBuffer(dx, ConstBufferBindIndex(0));

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(1));

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex(1));
}