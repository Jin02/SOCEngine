#include "BilateralFiltering.h"
#include "Director.h"
#include "BindIndexInfo.h"
#include "ResourceManager.h"
#include "SkyBox.h"

using namespace Rendering::PostProcessing;
using namespace Rendering::Texture;
using namespace Rendering::Shader;
using namespace Rendering::Buffer;
using namespace Rendering::Camera;
using namespace Rendering::TBDR;
using namespace Rendering::Sky;
using namespace Device;
using namespace Resource;

BilateralFiltering::BilateralFiltering()
	: _vertical(nullptr), _horizontal(nullptr)
{
}

BilateralFiltering::~BilateralFiltering()
{
	SAFE_DELETE(_vertical);
	SAFE_DELETE(_horizontal);
}

void BilateralFiltering::Initialize(BilateralFiltering::Type type)
{
	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());

	std::string psName = "";
	{
		if(type == Type::Near)		psName = "BilateralGaussNear_InFullScreen_PS";
		else if(type == Type::Far)	psName = "BilateralGaussFar_InFullScreen_PS";
	}

	ASSERT_COND_MSG(psName.empty() == false, "Error, psName is null");
	{
		_vertical	= new FullScreen;
		macros.push_back(ShaderMacro("BLUR_VERTICAL", ""));
		_vertical->Initialize("BilateralFiltering", psName, &macros);
	
		_horizontal	= new FullScreen;
		macros.back().SetName("BLUR_HORIZONTAL");
		_horizontal->Initialize("BilateralFiltering", psName, &macros);
	}
}

void BilateralFiltering::Render(const Device::DirectX* dx, const RenderTexture* outResultRT,
								const DepthBuffer* depthBuffer, const RenderTexture* inputColorMap)
{
	auto BindTexturesToPixelShader = [](ID3D11DeviceContext* context, uint bindIndex, const Texture2D* tex)
	{
		ID3D11ShaderResourceView* srv = tex ? tex->GetShaderResourceView()->GetView() : nullptr;
		context->PSSetShaderResources(bindIndex, 1, &srv);
	};


	ID3D11DeviceContext* context	= dx->GetContext();
	BindTexturesToPixelShader(context, 0, inputColorMap );
	BindTexturesToPixelShader(context, 1, depthBuffer );

	ID3D11SamplerState* samplerState = dx->GetSamplerStateLinear();
	context->PSSetSamplers(0, 1, &samplerState);
	
	ID3D11SamplerState* shadowSamplerState = dx->GetShadowSamplerState();
	context->PSSetSamplers(1, 1, &shadowSamplerState);

	_vertical->Render(dx, nullptr);
	_horizontal->Render(dx, nullptr);

	BindTexturesToPixelShader(context, 0, nullptr );
	BindTexturesToPixelShader(context, 1, nullptr );

	samplerState = nullptr;
	context->PSSetSamplers(0, 1, &samplerState);
	context->PSSetSamplers(1, 1, &samplerState);
}