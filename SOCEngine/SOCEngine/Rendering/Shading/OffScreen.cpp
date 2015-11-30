#include "OffScreen.h"
#include "Utility.h"

using namespace Rendering::TBDR;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Texture;

OffScreen::OffScreen()
	: FullScreen()
{
}

OffScreen::~OffScreen()
{
}

void OffScreen::Initialize(const RenderTexture* inputRenderTexture)
{
	FullScreen::Initialize("DeferredMainOffScreen", "PS", nullptr);

	std::vector<ShaderForm::InputTexture> inputTextures;
	{
		ShaderForm::InputTexture input(0, inputRenderTexture, false, false, false, true);
		inputTextures.push_back(input);
	}

	SetInputPSTextures(inputTextures);
}