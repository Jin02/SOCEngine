#include "DirectX.h"
#include <assert.h>
#include "Launcher.h"
#include "BaseShader.h"
#include "BaseBuffer.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Device;
using namespace Rendering::Shader;

DirectX::DirectX(void) :
	_device(), _swapChain(), _immediateContext(),
	_renderTargetView(), _rasterizerClockwiseDisableCulling(),
	_blendOpaque(), _blendAlphaToCoverage(),
	_depthDisableDepthTest(), _depthLess(), 
	_depthEqualAndDisableDepthWrite(), _depthGreater(),
	_depthGreaterAndDisableDepthWrite(), _blendAlpha(),
	_samplerAnisotropic(), _samplerLinear(), _samplerPoint(),
	_rasterizerClockwiseDefault(), _rasterizerCounterClockwiseDisableCulling(),
	_rasterizerCounterClockwiseDefault(), _samplerShadowLessEqualComp(), _samplerShadowGreaterEqualComp(), _samplerShadowLinear(),
	_rasterizerClockwiseDisableCullingWithClip(),
	_depthLessEqual(), _samplerConeTracing(), _depthGreaterEqualAndDisableDepthWrite(),
	_backBufferSize(0, 0)
{
	memset(&_msaaDesc, 0, sizeof(DXGI_SAMPLE_DESC));
}

void DirectX::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;

	assert(SUCCEEDED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)));

	ID3D11RenderTargetView* rtv = nullptr;
	assert(SUCCEEDED(_device->CreateRenderTargetView(backBuffer, nullptr, &rtv)));
	_renderTargetView = DXResource<ID3D11RenderTargetView>(rtv);

	backBuffer->Release();
}

void DirectX::CreateDeviceAndSwapChain(const WinApp& win, const Size<uint>& viewportSize, bool useMSAA)
{
	//swapChain setting
	DXGI_SWAP_CHAIN_DESC	sd;
	memset(&sd, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

	sd.BufferDesc.Width						= viewportSize.w;
	sd.BufferDesc.Height					= viewportSize.h;
	sd.BufferCount							= 1;
	sd.BufferUsage							= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow							= win.GetHandle();
	sd.Windowed								= win.GetIsWindowMode();
	sd.SwapEffect							= DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags								= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferDesc.Format					= DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator		= 0;
	sd.BufferDesc.RefreshRate.Denominator	= 1;
	sd.SampleDesc.Count						= useMSAA ? 4 : 1;
	sd.SampleDesc.Quality					= 0;

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE( driverTypes );

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};

	int numFeatureLevels = ARRAYSIZE( featureLevels );

	unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	ID3D11Device*			device = nullptr;
	IDXGISwapChain*			swapChain = nullptr;
	ID3D11DeviceContext*	immediateContext = nullptr;

	uint driverTypeIndex = 0;
	for(; driverTypeIndex<numDriverTypes; driverTypeIndex++ )
    {
        _driverType = driverTypes[driverTypeIndex];
        HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &swapChain, &device, &_featureLevel, &immediateContext);

		if (SUCCEEDED(hr))
		{
			_device = DXResource<ID3D11Device>(device);
			_swapChain = DXResource<IDXGISwapChain>(swapChain);
			_immediateContext = DXResource<ID3D11DeviceContext>(immediateContext);

			break;
		}
    }

	assert(driverTypeIndex < numDriverTypes);
	_msaaDesc = sd.SampleDesc;
}

void DirectX::InitViewport(const Rect<uint>& rect)
{
	D3D11_VIEWPORT vp;

	vp.TopLeftX		= static_cast<float>(rect.x);
	vp.TopLeftY		= static_cast<float>(rect.y);
	vp.Width		= static_cast<float>(rect.size.w);
	vp.Height		= static_cast<float>(rect.size.h);
	vp.MinDepth		= 0.0f;
	vp.MaxDepth		= 1.0f;

	_immediateContext->RSSetViewports( 1, &vp );

	_backBufferSize.w = static_cast<uint>(vp.Width);
	_backBufferSize.h = static_cast<uint>(vp.Height);
}

void DirectX::CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format)
{
	unsigned int ableMultiSample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
	for(ableMultiSample; ableMultiSample >= 0; ableMultiSample/=2)
	{
		DXGI_SAMPLE_DESC desc;
		HRESULT hr = _device->CheckMultisampleQualityLevels( format, ableMultiSample, &desc.Quality);

		if( FAILED(hr) )
			break;

		desc.Count = ableMultiSample;
		desc.Quality -= 1;

		outDescs.push_back(desc);
	}
}

void DirectX::CreateBlendStates()
{
	D3D11_BLEND_DESC blendDesc;
	memset(&blendDesc, 0, sizeof(D3D11_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable				= false;
	blendDesc.IndependentBlendEnable			= false;

	D3D11_RENDER_TARGET_BLEND_DESC renderTargetBlendDesc;

	renderTargetBlendDesc.BlendEnable			= false;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_ONE; 
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_ZERO; 

	renderTargetBlendDesc.BlendOp				= D3D11_BLEND_OP_ADD;
	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_ONE; 

	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_ZERO; 
	renderTargetBlendDesc.BlendOpAlpha			= D3D11_BLEND_OP_ADD;

	renderTargetBlendDesc.RenderTargetWriteMask	= D3D11_COLOR_WRITE_ENABLE_ALL;

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	blendDesc.RenderTarget[1] = renderTargetBlendDesc;
	blendDesc.RenderTarget[2] = renderTargetBlendDesc;

	{
		ID3D11BlendState* blendOpaque = nullptr;
		assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendOpaque)));
		_blendOpaque = DXResource<ID3D11BlendState>(blendOpaque);
	}

	{
		ID3D11BlendState* blendAlphaToCoverage = nullptr;
		blendDesc.AlphaToCoverageEnable = true;
		assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendAlphaToCoverage)));
		_blendAlphaToCoverage = DXResource<ID3D11BlendState>(blendAlphaToCoverage);
	}

	blendDesc.AlphaToCoverageEnable				= false;
	renderTargetBlendDesc.BlendEnable			= true;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;

	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	blendDesc.RenderTarget[1] = renderTargetBlendDesc;
	blendDesc.RenderTarget[2] = renderTargetBlendDesc;

	{
		ID3D11BlendState* blendAlpha = nullptr;
		assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendAlpha)));
		_blendAlpha = DXResource<ID3D11BlendState>(blendAlpha);
	}
}

DXResource<ID3D11ShaderResourceView> Device::DirectX::CreateShaderResourceView(ID3D11Resource * rawResource, const D3D11_SHADER_RESOURCE_VIEW_DESC & desc)
{
	ID3D11ShaderResourceView* srv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateShaderResourceView(rawResource, &desc, &srv));

	return DXResource<ID3D11ShaderResourceView>(srv);
}

DXResource<ID3D11UnorderedAccessView> Device::DirectX::CreateUnorderedAccessView(ID3D11Resource * rawResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC & desc)
{
	ID3D11UnorderedAccessView* uav = nullptr;
	ASSERT_SUCCEEDED(_device->CreateUnorderedAccessView(rawResource, &desc, &uav));

	return DXResource<ID3D11UnorderedAccessView>(uav);
}

DXResource<ID3D11RenderTargetView> Device::DirectX::CreateRenderTargetView(ID3D11Resource * rawResource, const D3D11_RENDER_TARGET_VIEW_DESC & desc)
{
	ID3D11RenderTargetView* rtv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateRenderTargetView(rawResource, &desc, &rtv));

	return DXResource<ID3D11RenderTargetView>(rtv);
}

DXResource<ID3D11DepthStencilView> Device::DirectX::CreateDepthStencilView(ID3D11Resource * rawResource, const D3D11_DEPTH_STENCIL_VIEW_DESC & desc)
{
	ID3D11DepthStencilView* dsv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateDepthStencilView(rawResource, &desc, &dsv));

	return DXResource<ID3D11DepthStencilView>(dsv);
}

DXResource<ID3D11Buffer> Device::DirectX::CreateBuffer(const D3D11_BUFFER_DESC & desc, const void * data)
{
	ID3D11Buffer* buffer = nullptr;
	ASSERT_SUCCEEDED(_device->CreateBuffer(&desc, nullptr, &buffer));

	return DXResource<ID3D11Buffer>(buffer);
}

DXResource<ID3D11GeometryShader> Device::DirectX::CreateGeometryShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11GeometryShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXResource<ID3D11GeometryShader>(shader);
}

DXResource<ID3D11PixelShader> Device::DirectX::CreatePixelShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11PixelShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXResource<ID3D11PixelShader>(shader);
}

DXResource<ID3D11VertexShader> Device::DirectX::CreateVertexShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11VertexShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXResource<ID3D11VertexShader>(shader);
}

DXResource<ID3D11ComputeShader> Device::DirectX::CreateComputeShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11ComputeShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXResource<ID3D11ComputeShader>(shader);
}

DXResource<ID3D11InputLayout> Device::DirectX::CreateInputLayout(BaseShader& baseShader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert( (blob != nullptr) && (vertexDeclations.empty() == false) );

	ID3D11InputLayout* layout = nullptr;
	ASSERT_SUCCEEDED(
		_device->CreateInputLayout(vertexDeclations.data(), vertexDeclations.size(),
		blob->GetBufferPointer(), blob->GetBufferSize(), &layout)
		);
		
	return DXResource<ID3D11InputLayout>(layout);
}

DXResource<ID3D11Texture2D> Device::DirectX::CreateTexture2D(const D3D11_TEXTURE2D_DESC & desc) 
{
	ID3D11Texture2D* texture = nullptr;
	ASSERT_SUCCEEDED(_device->CreateTexture2D(&desc, nullptr, &texture));

	return DXResource<ID3D11Texture2D>(texture);
}

DXResource<ID3D11Texture3D> Device::DirectX::CreateTexture3D(const D3D11_TEXTURE3D_DESC & desc) 
{
	ID3D11Texture3D* texture = nullptr;
	ASSERT_SUCCEEDED(_device->CreateTexture3D(&desc, nullptr, &texture));

	return DXResource<ID3D11Texture3D>(texture);
}

void DirectX::Initialize(const WinApp& win, const Rect<uint>& viewport, bool useMSAA)
{
	CreateDeviceAndSwapChain(win, viewport.size, useMSAA);
	CreateRenderTargetView();

	_immediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );

	InitViewport(viewport);

	//Initialize rasterizer State
	{
		D3D11_RASTERIZER_DESC desc;
		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_NONE;		//ÄÃ¸µ ²û
		desc.FrontCounterClockwise	= true;
		desc.DepthBias				= 0;
		desc.DepthBiasClamp			= 0.0f;
		desc.SlopeScaledDepthBias	= 0.0f;
		desc.DepthClipEnable		= true;
		desc.ScissorEnable			= false;
		desc.MultisampleEnable		= false;
		desc.AntialiasedLineEnable	= false;

		ID3D11RasterizerState* rss = nullptr;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerCounterClockwiseDisableCulling = DXResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= false;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerClockwiseDisableCulling = DXResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= true;
		desc.CullMode = D3D11_CULL_BACK;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerCounterClockwiseDefault = DXResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= false;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerClockwiseDefault = DXResource<ID3D11RasterizerState>(rss);

		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_NONE;
		desc.DepthClipEnable		= false;
		desc.FrontCounterClockwise	= false;

		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerClockwiseDisableCullingWithClip = DXResource<ID3D11RasterizerState>(rss);

	}
	
	//Initialize Blend State
	CreateBlendStates();

	//Initialize Depth State
	//using inverted 32bit float depth
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable = true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc = D3D11_COMPARISON_GREATER; //inverted depth
		desc.StencilEnable = false;
		desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		{
			ID3D11DepthStencilState* dss = nullptr;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthGreater = DXResource<ID3D11DepthStencilState>(dss);
		}

		//disable depth write
		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthDisableDepthWrite = DXResource<ID3D11DepthStencilState>(dss);
		}

		//disable depth test
		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthEnable = false;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthDisableDepthTest = DXResource<ID3D11DepthStencilState>(dss);
		}

		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthEnable = true;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
			desc.DepthFunc = D3D11_COMPARISON_GREATER;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthGreaterAndDisableDepthWrite = DXResource<ID3D11DepthStencilState>(dss);
		}

		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthGreaterEqualAndDisableDepthWrite = DXResource<ID3D11DepthStencilState>(dss);
		}

		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthFunc = D3D11_COMPARISON_EQUAL;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthEqualAndDisableDepthWrite = DXResource<ID3D11DepthStencilState>(dss);
		}

		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			desc.DepthFunc = D3D11_COMPARISON_LESS;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthLess = DXResource<ID3D11DepthStencilState>(dss);
		}

		{
			ID3D11DepthStencilState* dss = nullptr;
			desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
			assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));

			_depthLessEqual = DXResource<ID3D11DepthStencilState>(dss);
		}
	}

	//sampler
	
	D3D11_SAMPLER_DESC desc;
	memset(&desc, 0, sizeof(D3D11_SAMPLER_DESC));

	desc.Filter			= D3D11_FILTER_ANISOTROPIC;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy	= 16;
	desc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	desc.MinLOD			= -D3D11_FLOAT32_MAX;
	desc.MaxLOD			= D3D11_FLOAT32_MAX;

	ID3D11SamplerState* ss = nullptr;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerAnisotropic = DXResource<ID3D11SamplerState>(ss);

	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerLinear = DXResource<ID3D11SamplerState>(ss);

	desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerPoint = DXResource<ID3D11SamplerState>(ss);

	// Shadow Sampler State
	desc.Filter			= D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.ComparisonFunc	= D3D11_COMPARISON_LESS_EQUAL;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy	= 1;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerShadowLessEqualComp = DXResource<ID3D11SamplerState>(ss);

	desc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerShadowGreaterEqualComp = DXResource<ID3D11SamplerState>(ss);

	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_BORDER;
	desc.MipLODBias		= 0.0f;
	desc.MaxAnisotropy	= 0;
	desc.ComparisonFunc	= D3D11_COMPARISON_NEVER;//D3D11_COMPARISON_ALWAYS;
	desc.BorderColor[0]	= desc.BorderColor[1] = desc.BorderColor[2] = desc.BorderColor[3] = 0;
	desc.MinLOD			= 0;
	desc.MaxLOD			= 0;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerShadowLinear = DXResource<ID3D11SamplerState>(ss);

	ZeroMemory(&desc, sizeof(desc));
	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	desc.MaxAnisotropy	= 0;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_BORDER;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_BORDER;
	desc.BorderColor[0]	= 0.0f;
	desc.BorderColor[1]	= 0.0f;
	desc.BorderColor[2]	= 0.0f;
	desc.BorderColor[3]	= 1.0f;
	desc.ComparisonFunc	= D3D11_COMPARISON_NEVER;
	desc.MinLOD			= 0;
	desc.MaxLOD			= D3D11_FLOAT32_MAX;

	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerConeTracing = DXResource<ID3D11SamplerState>(ss);
}

unsigned int DirectX::CalcFormatSize(DXGI_FORMAT format) const
{
	if(		DXGI_FORMAT_R32G32B32A32_TYPELESS	<= format && format <= DXGI_FORMAT_R32G32B32A32_SINT)
		return 16;
	else if(DXGI_FORMAT_R32G32B32_TYPELESS		<= format && format <= DXGI_FORMAT_R32G32B32_SINT)
		return 12;
	else if(DXGI_FORMAT_R16G16B16A16_TYPELESS	<= format && format <= DXGI_FORMAT_R16G16B16A16_SINT)
		return 8;
	else if(DXGI_FORMAT_R32G32_TYPELESS			<= format && format <= DXGI_FORMAT_R32G32_SINT)
		return 8;
	else if(DXGI_FORMAT_R8G8B8A8_TYPELESS		<= format && format <= DXGI_FORMAT_R8G8B8A8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16G16_TYPELESS			<= format && format <= DXGI_FORMAT_R16G16_SINT)
		return 4;
	else if(DXGI_FORMAT_R32_TYPELESS			<= format && format <= DXGI_FORMAT_R32_SINT)
		return 4;
	else if(DXGI_FORMAT_R8G8_TYPELESS			<= format && format <= DXGI_FORMAT_R8G8_SINT)
		return 4;
	else if(DXGI_FORMAT_R16_TYPELESS			<= format && format <= DXGI_FORMAT_R16_SINT)
		return 2;
	else if(DXGI_FORMAT_R8_TYPELESS				<= format && format <= DXGI_FORMAT_R8_SINT)
		return 1;
	else assert(0);

	return 0;
}

void DirectX::ClearDeviceContext()
{
    ID3D11ShaderResourceView* pSRVs[16]		= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11RenderTargetView* pRTVs[16]		= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11Buffer* pBuffers[16]				= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11SamplerState* pSamplers[16]		= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
	ID3D11DepthStencilView* pDSV			= NULL;

    // Constant buffers
    _immediateContext->VSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->PSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->CSSetConstantBuffers( 0, 14, pBuffers );
    _immediateContext->GSSetConstantBuffers( 0, 14, pBuffers );

    // Resources
    _immediateContext->VSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->PSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->CSSetShaderResources( 0, 16, pSRVs );
    _immediateContext->GSSetShaderResources( 0, 16, pSRVs );

    // Samplers
    _immediateContext->VSSetSamplers( 0, 16, pSamplers );
    _immediateContext->PSSetSamplers( 0, 16, pSamplers );
    _immediateContext->CSSetSamplers( 0, 16, pSamplers );
    _immediateContext->GSSetSamplers( 0, 16, pSamplers );

    // Render targets
    _immediateContext->OMSetRenderTargets( 8, pRTVs, pDSV );

    // States
    float BlendFactor[4] = { 0,0,0,0 };
    _immediateContext->OMSetBlendState( NULL, BlendFactor, 0xFFFFFFFF );
	_immediateContext->OMSetDepthStencilState( _depthGreater.GetRaw(), 0x00 );  // we are using inverted 32-bit float depth for better precision
	_immediateContext->RSSetState(_rasterizerClockwiseDefault.GetRaw());
}

const ShaderMacro DirectX::GetMSAAShaderMacro() const
{
	return ShaderMacro("MSAA_SAMPLES_COUNT", std::to_string(_msaaDesc.Count));
}

const Size<uint> DirectX::FetchBackBufferSize()
{
	uint num = 1;
	D3D11_VIEWPORT vp;
	_immediateContext->RSGetViewports(&num, &vp);

	_backBufferSize.w = static_cast<uint>(vp.Width);
	_backBufferSize.h = static_cast<uint>(vp.Height);

	return _backBufferSize;
}