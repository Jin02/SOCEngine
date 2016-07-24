#include "GaussianBlur.h"
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

GaussianBlur::GaussianBlur()
	: _vertical(nullptr), _horizontal(nullptr), _tempBuffer(nullptr)
{
}

GaussianBlur::~GaussianBlur()
{
	SAFE_DELETE(_vertical);
	SAFE_DELETE(_horizontal);
	SAFE_DELETE(_tempBuffer);
}

void GaussianBlur::Initialize(const Math::Size<uint>& size, DXGI_FORMAT format)
{
	_filteringSize = size;

	std::vector<ShaderMacro> macros;
	macros.push_back(Director::SharedInstance()->GetDirectX()->GetMSAAShaderMacro());

	// Init Shader
	{
		if(_vertical == nullptr)	_vertical	= new FullScreen;
		macros.push_back(ShaderMacro("BLUR_VERTICAL", ""));
		_vertical->Initialize("GaussianBlur", "GuassianBlur_InFullScreen_PS", &macros);
	
		if(_horizontal == nullptr)	_horizontal	= new FullScreen;
		macros.back().SetName("BLUR_HORIZONTAL");
		_horizontal->Initialize("GaussianBlur", "GuassianBlur_InFullScreen_PS", &macros);

		if(_tempBuffer == nullptr)	_tempBuffer = new RenderTexture;
		_tempBuffer->Initialize(size, format, format, DXGI_FORMAT_UNKNOWN, 0, 1);
	}
}

void GaussianBlur::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const RenderTexture* inputColorMap)
{
	ID3D11DeviceContext* context	= dx->GetContext();

	// Setting Viewport
	{
		D3D11_VIEWPORT vp;
	
		vp.TopLeftX	= 0.0f;
		vp.TopLeftY	= 0.0f;
		vp.Width	= _filteringSize.Cast<float>().w;
		vp.Height	= _filteringSize.Cast<float>().h;
		vp.MinDepth	= 0.0f;
		vp.MaxDepth	= 1.0f;
	
		context->RSSetViewports( 1, &vp );
	}


	PixelShader::BindTexture(context, TextureBindIndex(0), inputColorMap);
	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, dx->GetSamplerStateLinear());
	
	_vertical->Render(dx, _tempBuffer);

	PixelShader::BindTexture(context, TextureBindIndex(0), _tempBuffer);

	_horizontal->Render(dx, outResultRT);

	PixelShader::BindTexture(context, TextureBindIndex(0), nullptr);
	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, nullptr);
}

void GaussianBlur::Destroy()
{
	_vertical->Destroy();
	_horizontal->Destroy();
	_tempBuffer->Destroy();
}
