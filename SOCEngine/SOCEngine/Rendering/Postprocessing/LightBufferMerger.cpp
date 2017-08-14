#include "LightBufferMerger.h"
#include "BindIndexInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Renderer;
using namespace Rendering;
using namespace Device;

void LightBufferMerger::Initialize(Device::DirectX& dx, Manager::ShaderManager& shaderMgr)
{
	FullScreen::InitParam param;
	{
		param.psMacros			= nullptr;
		param.psName			= "PS";
		param.shaderFileName	= "DeferredMainOffScreen";
	}

	_screen.Initialize(dx, param, shaderMgr);
}

void LightBufferMerger::Render(DirectX& dx, RenderTexture& outResultRT, RenderTexture& diffuse, RenderTexture& specular)
{
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0), diffuse.GetTexture2D().GetShaderResourceView());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(1), specular.GetTexture2D().GetShaderResourceView());

	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, dx.GetSamplerStateLinear());

	_screen.Render(dx, outResultRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(1));

	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
}
