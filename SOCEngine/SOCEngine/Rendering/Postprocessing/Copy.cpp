#include "Copy.h"
#include "BindIndexInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Device;

void Copy::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	_screen.Initialize(dx, FullScreen::InitParam("Copy", "Copy_InFullScreen_PS", nullptr), shaderMgr);
}

void Copy::Render(Device::DirectX& dx, RenderTexture& outResultRT, RenderTexture& inputColorMap)
{
	PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, dx.GetSamplerStateLinear());
	PixelShader::BindShaderResourceView(dx, TextureBindIndex(0), inputColorMap.GetTexture2D().GetShaderResourceView());
	
	_screen.Render(dx, outResultRT, true);

	PixelShader::UnBindShaderResourceView(dx, TextureBindIndex(0));
	PixelShader::UnBindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState);
}
