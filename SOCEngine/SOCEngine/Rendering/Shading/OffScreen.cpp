#include "OffScreen.h"
#include "Utility.h"

using namespace Device;
using namespace Rendering::TBDR;
using namespace Rendering::PostProcessing;
using namespace Rendering::Shader;
using namespace Rendering::Texture;

OffScreen::OffScreen()
	: FullScreen(), _useIndirectColorMap(false)
{
}

OffScreen::~OffScreen()
{
}

void OffScreen::ReCompile(bool useIndirectColorMap)
{
	if( (_useIndirectColorMap == useIndirectColorMap) &&
		(_vertexShader && _pixelShader) )
		return;

	std::vector<ShaderMacro> macros;
	if(useIndirectColorMap)
		macros.push_back(ShaderMacro("USE_GI", ""));

	FullScreen::Initialize("DeferredMainOffScreen", "PS", &macros);
	_useIndirectColorMap = useIndirectColorMap;
}

void OffScreen::Initialize(const RenderTexture* directColorMap, bool useIndirectColorMap)
{
	ReCompile(useIndirectColorMap);
	_inputPSTextures.push_back(ShaderForm::InputTexture(0, directColorMap, false, false, false, true));
}

void OffScreen::Render(const DirectX* dx,
					   const RenderTexture* outResultRT,
					   const RenderTexture* indirectColorMap)
{
	if(_useIndirectColorMap)
	{
		ID3D11DeviceContext* context = dx->GetContext();
		ID3D11ShaderResourceView* srv = indirectColorMap->GetShaderResourceView()->GetView();
		context->PSSetShaderResources(1, 1, &srv);
	}

	FullScreen::Render(dx, outResultRT);
}