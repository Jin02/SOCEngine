#include "Copy.h"
#include "BindIndexInfo.h"
#include "AutoBinder.hpp"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Manager;
using namespace Rendering::RenderState;
using namespace Device;

void Copy::Initialize(Device::DirectX& dx, ShaderManager& shaderMgr)
{
	_screen.Initialize(dx, FullScreen::InitParam("Copy", "Copy_InFullScreen_PS", nullptr), shaderMgr);
}

void Copy::Render(Device::DirectX& dx, RenderTexture& outResultRT, const Texture2D& inputColorMap) const
{
	AutoBinderSampler<PixelShader> sampler(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Linear);
	AutoBinderSRV<PixelShader> inputMap(dx, TextureBindIndex(0), inputColorMap.GetShaderResourceView());
	
	_screen.Render(dx, outResultRT, true);
}
