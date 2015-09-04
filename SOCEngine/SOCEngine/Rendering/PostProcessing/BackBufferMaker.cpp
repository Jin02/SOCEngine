#include "BackBufferMaker.h"
#include "Director.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
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

		const ShaderMacro& msaaMacro = Director::GetInstance()->GetDirectX()->GetMSAAShaderMacro();
		macros.push_back(msaaMacro);
	}

	FullScreen::Initialize("FinalScreenMerger", "PS", &macros);

	_useUI = useUI;
}

void BackBufferMaker::Render(
	const Texture::RenderTexture* outResultRT,
	const Texture::RenderTexture* renderScene,
	const Texture::RenderTexture* uiScene)
{
	ID3D11RenderTargetView* rtv = outResultRT->GetRenderTargetView();	

	auto dx = Director::GetInstance()->GetDirectX();
	ID3D11DeviceContext* context = dx->GetContext();

	ID3D11DepthStencilView* nullDSV = nullptr;
	context->OMSetRenderTargets(1, &rtv, nullDSV);
	context->OMSetDepthStencilState(dx->GetDepthStateDisableDepthTest(), 0x00);

	uint stride = 0;
	uint offset = 0;
	ID3D11Buffer* nullBuffer[] = {nullptr};

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	_vertexShader->SetShaderToContext(context);
	_vertexShader->SetInputLayoutToContext(context);
	
	_pixelShader->SetShaderToContext(context);

	//_pixelShader->UpdateResources(context, nullptr, &_inputPSTextures, nullptr);
	{
		context->PSSetShaderResources(
			(uint)InputTextureShaderIndex::RenderScene, 
			1, renderScene->GetShaderResourceView());

		if(_useUI)
		{

			context->PSSetShaderResources(
			(uint)InputTextureShaderIndex::UIScene, 
			1, uiScene->GetShaderResourceView());
		}
	}

	ID3D11SamplerState* linerSampler = dx->GetSamplerStateLinear();
	context->PSSetSamplers(0, 1, &linerSampler);

	if(_useUI)
	{
		ID3D11SamplerState* pointSampler = dx->GetSamplerStatePoint();
		context->PSSetSamplers(1, 1, &pointSampler);
	}

	context->Draw(3, 0);
}