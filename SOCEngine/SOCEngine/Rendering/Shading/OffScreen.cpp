#include "OffScreen.h"
#include "Utility.h"

using namespace Rendering::DeferredShading;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;

OffScreen::OffScreen()
	: FullScreen()
{
}

OffScreen::~OffScreen()
{
}

void OffScreen::Initialize(const Rendering::Texture::RenderTexture* offScreenRT)
{
	_offScreenRT = offScreenRT;
	FullScreen::Initialize("DeferredMainRT", "PS");

	std::vector<BaseShader::TextureType> inputTextures;
	{
		BaseShader::TextureType offScreen(0, offScreenRT, false, false, false, true);
		inputTextures.push_back(offScreen);
	}

	SetInputPSTextures(inputTextures);
}

void OffScreen::Render()
{
	if(_offScreenRT)
		FullScreen::Render(_offScreenRT);
	else
	{
		DEBUG_LOG("OffScreen is null. Plz Check your code");
	}
}

void OffScreen::Destroy()
{
	_offScreenRT = nullptr;
	FullScreen::Destroy();
}