#include "BackBufferMaker.h"
#include "Director.h"
#include "BindIndexInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::TBDR;
using namespace Rendering;
using namespace Device;

BackBufferMaker::BackBufferMaker()
	: FullScreen(), _useUI(false)
{
}

BackBufferMaker::~BackBufferMaker()
{
}

void BackBufferMaker::Initialize(bool useUI)
{
	std::vector<ShaderMacro> macros;
	{
		if(useUI)
			macros.push_back(ShaderMacro("USE_UI", ""));

		const ShaderMacro& msaaMacro = Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);
	}

	FullScreen::Initialize("FinalScreenMerger", "PS", true, &macros);

	_useUI = useUI;
}

void BackBufferMaker::Render(
	ID3D11RenderTargetView* outResultRTV,
	const RenderTexture* renderScene,
	const RenderTexture* uiScene,
	const ConstBuffer* const& tbrParamConstBuffer)
{
	auto dx = Director::SharedInstance()->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &outResultRTV, nullDSV);
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	uint stride = 0;
	uint offset = 0;
	ID3D11Buffer* nullBuffer[] = {nullptr};

	_vertexShader->BindShaderToContext(context);
	_vertexShader->BindInputLayoutToContext(context);
	
	_pixelShader->BindShaderToContext(context);

	//_pixelShader->BindResourcesToContext(context, nullptr, &_inputPSTextures, nullptr);
	{
		PixelShader::BindConstBuffer(context, ConstBufferBindIndex::TBRParam, tbrParamConstBuffer);
		PixelShader::BindTexture(context, Rendering::TextureBindIndex(TextureBindIndex::RenderScene), renderScene);

		if(_useUI)
			PixelShader::BindTexture(context, Rendering::TextureBindIndex(TextureBindIndex::UIScene), uiScene);
	}

	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());

	if(_useUI)
		PixelShader::BindSamplerState(context, SamplerStateBindIndex::UISamplerState, dx->GetSamplerStatePoint());

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
