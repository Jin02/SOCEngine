#include "BackBufferMaker.h"
#include "Director.h"
#include "TBRShaderIndexSlotInfo.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::TBDR;
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
	ID3D11RenderTargetView* outResultRTV,
	const RenderTexture*& renderScene,
	const RenderTexture* uiScene,
	const ConstBuffer* const& tbrParamConstBuffer)
{
	auto dx = Director::GetInstance()->GetDirectX();
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
		ID3D11Buffer* cb = tbrParamConstBuffer->GetBuffer();
		context->PSSetConstantBuffers(
			(uint)InputConstBufferBindSlotIndex::TBRParam,
			1, &cb);

		ID3D11ShaderResourceView* srv = renderScene->GetShaderResourceView()->GetView();
		context->PSSetShaderResources(
			(uint)InputTextureShaderIndex::RenderScene, 
			1, &srv);

		if(_useUI)
		{
			srv = uiScene->GetShaderResourceView()->GetView();
			context->PSSetShaderResources(
			(uint)InputTextureShaderIndex::UIScene, 
			1, &srv);
		}
	}

	ID3D11SamplerState* linerSampler = dx->GetSamplerStateLinear();
	context->PSSetSamplers((uint)InputSamplerStateBindSlotIndex::DefaultSamplerState, 1, &linerSampler);

	if(_useUI)
	{
		ID3D11SamplerState* pointSampler = dx->GetSamplerStatePoint();
		context->PSSetSamplers((uint)InputSamplerStateBindSlotIndex::UISamplerState, 1, &pointSampler);
	}

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	context->RSSetState( nullptr );

	context->Draw(3, 0);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}