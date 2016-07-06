#include "OffScreen.h"
#include "Utility.h"
#include "BindIndexInfo.h"

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

	std::vector<ShaderForm::InputTexture> prevInputTextures = _inputTextures;
	FullScreen::Initialize("DeferredMainOffScreen", "PS", &macros);
	_inputTextures = prevInputTextures;

	_useIndirectColorMap = useIndirectColorMap;
}

void OffScreen::Initialize(const RenderTexture* diffuseLightBuffer, const RenderTexture* specularLightBuffer, bool useIndirectColorMap)
{
	ReCompile(useIndirectColorMap);
	_inputTextures.push_back(ShaderForm::InputTexture(uint( TextureBindIndex::OffScreen_DirectDiffuseLightBuffer ), diffuseLightBuffer, false, false, false, true));
	_inputTextures.push_back(ShaderForm::InputTexture(uint( TextureBindIndex::OffScreen_DirectSpecularLightBuffer), specularLightBuffer, false, false, false, true));
}

void OffScreen::Render(const DirectX* dx,
					   const RenderTexture* outResultRT,
					   const RenderTexture* indirectColorMap)
{
	if(_useIndirectColorMap)
	{
		ID3D11DeviceContext* context = dx->GetContext();
		ID3D11ShaderResourceView* srv = indirectColorMap->GetShaderResourceView()->GetView();
		context->PSSetShaderResources( uint(TextureBindIndex::OffScreen_InDirectLightBuffer), 1, &srv);
	}

	FullScreen::Render(dx, outResultRT);
}