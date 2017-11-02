#include "GaussianBlur.h"
#include "BindIndexInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
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

inline void GaussianBlur::UpdateParamCB(Device::DirectX & dx, const ParamCBData & param)
{
	_paramCB.UpdateSubResource(dx, param);
}

void GaussianBlur::Render(Device::DirectX& dx, RenderTexture& outResultRT,
	RenderTexture& inputColorMap, RenderTexture& tempMap)
{
	PixelShader::BindShaderResourceView(dx,	TextureBindIndex(0),						inputColorMap.GetTexture2D().GetShaderResourceView());
	PixelShader::BindConstBuffer(dx,		ConstBufferBindIndex(0),					_paramCB);
	PixelShader::BindSamplerState(dx,		SamplerStateBindIndex::DefaultSamplerState,	dx.GetSamplerStateLinear());
	
	_vertical.Render(dx, tempMap, true);

	PixelShader::BindShaderResourceView(dx,	TextureBindIndex(0), tempMap.GetTexture2D().GetShaderResourceView());
	_horizontal.Render(dx, outResultRT, true);

	PixelShader::UnBindShaderResourceView(dx,	TextureBindIndex(0));
	PixelShader::UnBindSamplerState(dx,			SamplerStateBindIndex::DefaultSamplerState);
	PixelShader::UnBindConstBuffer(dx,			ConstBufferBindIndex(0));
}
