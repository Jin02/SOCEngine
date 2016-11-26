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
	: _vertical(nullptr), _horizontal(nullptr), _tempMap(nullptr), _paramCB(nullptr)
{
}

GaussianBlur::~GaussianBlur()
{
	SAFE_DELETE(_vertical);
	SAFE_DELETE(_horizontal);
	SAFE_DELETE(_tempMap);

	SAFE_DELETE(_paramCB);
}

void GaussianBlur::Initialize(const Device::DirectX* dx, const Math::Size<uint>& size)
{
	_filteringSize = size;

	std::vector<ShaderMacro> macros;
	macros.push_back(dx->GetMSAAShaderMacro());

	// Init Shader
	{
		if(_vertical == nullptr)	_vertical	= new FullScreen;
		macros.push_back(ShaderMacro("BLUR_VERTICAL", ""));
		_vertical->Initialize("GaussianBlur", "GuassianBlur_InFullScreen_PS", &macros);
	
		if(_horizontal == nullptr)	_horizontal	= new FullScreen;
		macros.back().SetName("BLUR_HORIZONTAL");
		_horizontal->Initialize("GaussianBlur", "GuassianBlur_InFullScreen_PS", &macros);
	}

	_paramCB = new ConstBuffer;
	_paramCB->Initialize(sizeof(ParamCB));

	ParamCB param;
	{
		param.blurSize			= 2.5f;
		param.sigma				= 6.0f;
		param.numPixelPerSide	= 8.0f;
		param.scale				= 1.0f;
	}

	UpdateParam(dx, param);
}

void GaussianBlur::Initialize(const Device::DirectX* dx, const Math::Size<uint>& size, DXGI_FORMAT format)
{
	Initialize(dx, size);

	if(_tempMap == nullptr)	_tempMap = new RenderTexture;
	_tempMap->Initialize(size, format, format, DXGI_FORMAT_UNKNOWN, 0, 1);
}

void GaussianBlur::UpdateParam(const Device::DirectX* dx, const ParamCB& param)
{
	_paramCB->UpdateSubResource(dx->GetContext(), &param);
}

void GaussianBlur::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const RenderTexture* inputColorMap)
{
	Render(dx, outResultRT, inputColorMap, _tempMap);
}

void GaussianBlur::Render(const Device::DirectX* dx, const RenderTexture* outResultRT, const RenderTexture* inputColorMap, const RenderTexture* tempMap)
{
	ASSERT_COND_MSG(tempMap, "Error, temp map is null!");

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
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex(0), _paramCB);
	
	_vertical->Render(dx, tempMap);

	PixelShader::BindTexture(context, TextureBindIndex(0), tempMap);

	_horizontal->Render(dx, outResultRT);

	PixelShader::BindTexture(context, TextureBindIndex(0), nullptr);
	PixelShader::BindSamplerState(context, SamplerStateBindIndex::DefaultSamplerState, nullptr);
	PixelShader::BindConstBuffer(context, ConstBufferBindIndex(0), nullptr);
}

void GaussianBlur::Destroy()
{
	_vertical->Destroy();
	_horizontal->Destroy();

	if(_tempMap)
		_tempMap->Destroy();
}
