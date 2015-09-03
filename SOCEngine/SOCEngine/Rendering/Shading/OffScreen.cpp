#include "OffScreen.h"
#include "Utility.h"

using namespace Rendering::TBDR;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace GPGPU::DirectCompute;

OffScreen::OffScreen()
	: FullScreen()
{
}

OffScreen::~OffScreen()
{
}

void OffScreen::Initialize(const CSRWTexture* offScreen)
{
	FullScreen::Initialize("DeferredMainOffScreen", "PS");

	std::vector<BaseShader::TextureType> inputTextures;
	{
		auto texture = offScreen->GetRenderTexture();

		BaseShader::TextureType input(0, texture, false, false, false, true);
		inputTextures.push_back(input);
	}

	SetInputPSTextures(inputTextures);
}