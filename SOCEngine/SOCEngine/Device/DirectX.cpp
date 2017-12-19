#include "DirectX.h"
#include <assert.h>
#include "Launcher.h"
#include "BaseShader.hpp"
#include "BaseBuffer.h"
#include "RenderTexture.h"
#include "DepthMap.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

using namespace Device;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::View;
using namespace Rendering::RenderState;

DirectX::DirectX(const Rect<float>& backBufferRect) : _backBufferRect(backBufferRect)
{
	memset(&_msaaDesc, 0, sizeof(DXGI_SAMPLE_DESC));
}

void DirectX::CreateRenderTargetView()
{
	ID3D11Texture2D* backBuffer = nullptr;

	assert(SUCCEEDED(_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer)));

	ID3D11RenderTargetView* rtv = nullptr;
	assert(SUCCEEDED(_device->CreateRenderTargetView(backBuffer, nullptr, &rtv)));
//	backBuffer->Release();

	_backBufferRenderTexture = RenderTexture(DXSharedResource<ID3D11RenderTargetView>(rtv), DXSharedResource<ID3D11Texture2D>(backBuffer), _backBufferRect.size.Cast<uint>());
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

	ID3D11Device*			device				= nullptr;
	IDXGISwapChain*			swapChain			= nullptr;
	ID3D11DeviceContext*	immediateContext	= nullptr;

	uint driverTypeIndex = 0;
	for(; driverTypeIndex<numDriverTypes; driverTypeIndex++ )
	{
		_driverType = driverTypes[driverTypeIndex];
		
		HRESULT hr = D3D11CreateDeviceAndSwapChain( NULL, _driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
								D3D11_SDK_VERSION, &sd, &swapChain, &device, &_featureLevel, &immediateContext);
		
		if (SUCCEEDED(hr))
			break;
	}

	assert(driverTypeIndex < numDriverTypes);

	_device				= DXUniqueResource<ID3D11Device>(device);
	_swapChain			= DXUniqueResource<IDXGISwapChain>(swapChain);
	_immediateContext	= DXUniqueResource<ID3D11DeviceContext>(immediateContext);	
	_msaaDesc			= sd.SampleDesc;
}

void DirectX::SetViewport(const Rect<float>& rect)
{
	D3D11_VIEWPORT vp{rect.x, rect.y, rect.size.w, rect.size.h, 0.0f, 1.0f};
	_immediateContext->RSSetViewports(1, &vp);
}

void DirectX::RestoreViewportToBackBuffer()
{
	SetViewport(_backBufferRect);
}

void DirectX::ReSetRenderTargets(const uint size)
{
	ID3D11RenderTargetView* rtvs[8]	= { nullptr, };
	_immediateContext->OMSetRenderTargets(size, rtvs, nullptr);
}

void DirectX::SetUAVsWithoutRenderTarget(uint uavStartSlot, const uint numUAVs, UnorderedAccessView* const* uavs)
{
	ID3D11UnorderedAccessView* rawUAVs[8]	= { nullptr, };
	{
		for(uint i=0; i<numUAVs; ++i)
			rawUAVs[i] = uavs[i]->GetRaw();
	}

	_immediateContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, uavStartSlot, numUAVs, rawUAVs, nullptr);
}

void DirectX::ReSetUAVsWithoutRenderTarget(const uint uavStartSlot, const uint numUAVs)
{
	ID3D11UnorderedAccessView* rawUAVs[8]	= { nullptr, };
	_immediateContext->OMSetRenderTargetsAndUnorderedAccessViews(0, nullptr, nullptr, uavStartSlot, numUAVs, rawUAVs, nullptr);
}

void DirectX::SetDepthMapWithoutRenderTarget(DepthMap& depthMap)
{
	ID3D11RenderTargetView* nullRTV = nullptr;
	_immediateContext->OMSetRenderTargets(1, &nullRTV, depthMap.GetRawDepthStencilView());
}

void DirectX::SetRenderTarget(RenderTexture& rt, ID3D11DepthStencilView* dsv)
{
	ID3D11RenderTargetView* rtv = rt.GetRaw();
	_immediateContext->OMSetRenderTargets(1, &rtv, dsv);
}

void DirectX::SetRenderTarget(RenderTexture& rt, DepthMap& depthMap)
{
	SetRenderTarget(rt, depthMap.GetRawDepthStencilView());
}

void DirectX::SetRenderTarget(RenderTextureCube& target, ID3D11DepthStencilView* dsv)
{
	ID3D11RenderTargetView* rtv = target.GetRaw();
	_immediateContext->OMSetRenderTargets(1, &rtv, dsv);
}

void DirectX::SetRenderTarget(RenderTextureCube& target, DepthMapCube& targetDepthMap)
{
	SetRenderTarget(target, targetDepthMap.GetRawDepthStencilView());
}

void DirectX::SetRenderTargets(const uint size, RenderTexture* const * renderTextures, ID3D11DepthStencilView* dsv)
{
	ID3D11RenderTargetView* rtvs[8] = { nullptr, };
	{
		for(uint i=0; i<size; ++i)
			rtvs[i] = renderTextures[i]->GetRaw();
	}

	_immediateContext->OMSetRenderTargets(size, rtvs, dsv);
}

void DirectX::SetRenderTargets(const uint size, RenderTexture* const* renderTextures, DepthMap& depthMap)
{
	SetRenderTargets(size, renderTextures, depthMap.GetRawDepthStencilView());
}

std::vector<DXGI_SAMPLE_DESC> DirectX::CheckAbleMultiSampler(DXGI_FORMAT format)
{
	std::vector<DXGI_SAMPLE_DESC> outDescs;

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
	
	return outDescs;
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

	ID3D11BlendState* blendState = nullptr;
	assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendState)));
	_blendStates[static_cast<uint>(BlendState::Opaque)] = DXUniqueResource<ID3D11BlendState>(blendState);

	blendDesc.AlphaToCoverageEnable = true;
	assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendState)));
	_blendStates[static_cast<uint>(BlendState::AlphaToCoverage)] = DXUniqueResource<ID3D11BlendState>(blendState);

	blendDesc.AlphaToCoverageEnable				= false;
	renderTargetBlendDesc.BlendEnable			= true;

	renderTargetBlendDesc.SrcBlend				= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlend				= D3D11_BLEND_INV_SRC_ALPHA;

	renderTargetBlendDesc.SrcBlendAlpha			= D3D11_BLEND_SRC_ALPHA;
	renderTargetBlendDesc.DestBlendAlpha		= D3D11_BLEND_INV_SRC_ALPHA;

	blendDesc.RenderTarget[0] = renderTargetBlendDesc;
	blendDesc.RenderTarget[1] = renderTargetBlendDesc;
	blendDesc.RenderTarget[2] = renderTargetBlendDesc;

	assert(SUCCEEDED(_device->CreateBlendState(&blendDesc, &blendState)));
	_blendStates[static_cast<uint>(BlendState::Alpha)] = DXUniqueResource<ID3D11BlendState>(blendState);
}

DXSharedResource<ID3D11ShaderResourceView> DirectX::CreateShaderResourceView(ID3D11Resource* const rawResource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc)
{
	ID3D11ShaderResourceView* srv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateShaderResourceView(rawResource, &desc, &srv));

	return DXSharedResource<ID3D11ShaderResourceView>(srv);
}

DXSharedResource<ID3D11UnorderedAccessView> DirectX::CreateUnorderedAccessView(ID3D11Resource* const rawResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc)
{
	ID3D11UnorderedAccessView* uav = nullptr;
	ASSERT_SUCCEEDED(_device->CreateUnorderedAccessView(rawResource, &desc, &uav));

	return DXSharedResource<ID3D11UnorderedAccessView>(uav);
}

DXSharedResource<ID3D11RenderTargetView> DirectX::CreateRenderTargetView(ID3D11Resource* const rawResource, const D3D11_RENDER_TARGET_VIEW_DESC& desc)
{
	ID3D11RenderTargetView* rtv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateRenderTargetView(rawResource, &desc, &rtv));

	return DXSharedResource<ID3D11RenderTargetView>(rtv);
}

DXSharedResource<ID3D11DepthStencilView> DirectX::CreateDepthStencilView(ID3D11Resource* const rawResource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc)
{
	ID3D11DepthStencilView* dsv = nullptr;
	ASSERT_SUCCEEDED(_device->CreateDepthStencilView(rawResource, &desc, &dsv));

	return DXSharedResource<ID3D11DepthStencilView>(dsv);
}

DXSharedResource<ID3D11Buffer> DirectX::CreateBuffer(const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* data)
{
	ID3D11Buffer* buffer = nullptr;
	ASSERT_SUCCEEDED(_device->CreateBuffer(&desc, data, &buffer));

	return DXSharedResource<ID3D11Buffer>(buffer);
}

DXSharedResource<ID3D11GeometryShader> DirectX::CreateGeometryShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11GeometryShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateGeometryShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXSharedResource<ID3D11GeometryShader>(shader);
}

DXSharedResource<ID3D11PixelShader> DirectX::CreatePixelShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11PixelShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXSharedResource<ID3D11PixelShader>(shader);
}

DXSharedResource<ID3D11VertexShader> DirectX::CreateVertexShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11VertexShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXSharedResource<ID3D11VertexShader>(shader);
}

DXSharedResource<ID3D11ComputeShader> DirectX::CreateComputeShader(BaseShader& baseShader)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11ComputeShader* shader = nullptr;
	ASSERT_SUCCEEDED(_device->CreateComputeShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &shader));

	return DXSharedResource<ID3D11ComputeShader>(shader);
}

DXSharedResource<ID3D11InputLayout> DirectX::CreateInputLayout(BaseShader& baseShader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations)
{
	ID3DBlob* blob = baseShader.GetBlob().GetRaw();
	assert(blob != nullptr);

	ID3D11InputLayout* layout = nullptr;
	ASSERT_SUCCEEDED(
		_device->CreateInputLayout(vertexDeclations.data(), vertexDeclations.size(),
		blob->GetBufferPointer(), blob->GetBufferSize(), &layout)
		);
		
	return DXSharedResource<ID3D11InputLayout>(layout);
}

DXSharedResource<ID3D11Texture2D> DirectX::CreateTexture2D(const D3D11_TEXTURE2D_DESC& desc)
{
	ID3D11Texture2D* texture = nullptr;
	ASSERT_SUCCEEDED(_device->CreateTexture2D(&desc, nullptr, &texture));

	return DXSharedResource<ID3D11Texture2D>(texture);
}

DXSharedResource<ID3D11Texture3D> DirectX::CreateTexture3D(const D3D11_TEXTURE3D_DESC& desc)
{
	ID3D11Texture3D* texture = nullptr;
	ASSERT_SUCCEEDED(_device->CreateTexture3D(&desc, nullptr, &texture));

	return DXSharedResource<ID3D11Texture3D>(texture);
}

void DirectX::Initialize(const WinApp& win, const Rect<uint>& viewport, bool useMSAA)
{
	CreateDeviceAndSwapChain(win, viewport.size, useMSAA);
	CreateRenderTargetView();

	SetPrimitiveTopology(PrimitiveTopology::TriangleList);

	SetViewport(viewport.Cast<float>());

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
		_rasterizerStates[static_cast<uint>(RasterizerState::CCWDisableCulling)] = DXUniqueResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= false;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerStates[static_cast<uint>(RasterizerState::CWDisableCulling)] = DXUniqueResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= true;
		desc.CullMode = D3D11_CULL_BACK;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerStates[static_cast<uint>(RasterizerState::CCWDefault)] = DXUniqueResource<ID3D11RasterizerState>(rss);

		desc.FrontCounterClockwise	= false;
		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerStates[static_cast<uint>(RasterizerState::CWDefault)] = DXUniqueResource<ID3D11RasterizerState>(rss);

		desc.FillMode				= D3D11_FILL_SOLID;
		desc.CullMode				= D3D11_CULL_NONE;
		desc.DepthClipEnable		= false;
		desc.FrontCounterClockwise	= false;

		assert(SUCCEEDED(_device->CreateRasterizerState(&desc, &rss)));
		_rasterizerStates[static_cast<uint>(RasterizerState::CWDisableCullingWithClip)] = DXUniqueResource<ID3D11RasterizerState>(rss);
	}
	
	//Initialize Blend State
	CreateBlendStates();

	//Initialize Depth State
	//using inverted 32bit float depth
	{
		D3D11_DEPTH_STENCIL_DESC desc;
		memset(&desc, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
		desc.DepthEnable					= true;
		desc.DepthWriteMask					= D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc						= D3D11_COMPARISON_GREATER; //inverted depth
		desc.StencilEnable					= false;
		desc.StencilReadMask				= D3D11_DEFAULT_STENCIL_READ_MASK;
		desc.StencilWriteMask				= D3D11_DEFAULT_STENCIL_WRITE_MASK;

		desc.FrontFace.StencilFailOp		= D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilDepthFailOp	= D3D11_STENCIL_OP_INCR;
		desc.FrontFace.StencilPassOp		= D3D11_STENCIL_OP_KEEP;
		desc.FrontFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

		desc.BackFace.StencilFailOp			= D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilDepthFailOp	= D3D11_STENCIL_OP_DECR;
		desc.BackFace.StencilPassOp			= D3D11_STENCIL_OP_KEEP;
		desc.BackFace.StencilFunc			= D3D11_COMPARISON_ALWAYS;

		ID3D11DepthStencilState* dss = nullptr;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::Greater)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		//disable depth write
		desc.DepthWriteMask	= D3D11_DEPTH_WRITE_MASK_ZERO;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::DisableDepthWrite)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		//disable depth test
		desc.DepthEnable	= false;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::DisableDepthTestWrite)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		desc.DepthEnable	= true;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		desc.DepthFunc		= D3D11_COMPARISON_GREATER;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::GreaterAndDisableDepthWrite)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		desc.DepthFunc		= D3D11_COMPARISON_GREATER_EQUAL;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::GreaterEqualAndDisableDepthWrite)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		desc.DepthFunc		= D3D11_COMPARISON_EQUAL;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::EqualAndDisableDepthWrite)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		desc.DepthFunc		= D3D11_COMPARISON_LESS;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::Less)] = DXUniqueResource<ID3D11DepthStencilState>(dss);

		desc.DepthFunc		= D3D11_COMPARISON_LESS_EQUAL;
		assert(SUCCEEDED(_device->CreateDepthStencilState(&desc, &dss)));
		_depthStencilStates[static_cast<uint>(DepthState::LessEqual)] = DXUniqueResource<ID3D11DepthStencilState>(dss);
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
	_samplerStates[static_cast<uint>(SamplerState::Anisotropic)] = DXUniqueResource<ID3D11SamplerState>(ss);

	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerStates[static_cast<uint>(SamplerState::Linear)] = DXUniqueResource<ID3D11SamplerState>(ss);

	desc.Filter			= D3D11_FILTER_MIN_MAG_MIP_POINT;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerStates[static_cast<uint>(SamplerState::Point)] = DXUniqueResource<ID3D11SamplerState>(ss);

	// Shadow Sampler State
	desc.Filter			= D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
	desc.ComparisonFunc	= D3D11_COMPARISON_LESS_EQUAL;
	desc.AddressU		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW		= D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.MaxAnisotropy	= 1;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerStates[static_cast<uint>(SamplerState::ShadowLessEqualComp)] = DXUniqueResource<ID3D11SamplerState>(ss);

	desc.ComparisonFunc = D3D11_COMPARISON_GREATER_EQUAL;
	assert(SUCCEEDED(_device->CreateSamplerState(&desc, &ss)));
	_samplerStates[static_cast<uint>(SamplerState::ShadowGreaterEqualComp)] = DXUniqueResource<ID3D11SamplerState>(ss);

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
	_samplerStates[static_cast<uint>(SamplerState::ShadowLinear)] = DXUniqueResource<ID3D11SamplerState>(ss);

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
	_samplerStates[static_cast<uint>(SamplerState::ConeTracingLinear)] = DXUniqueResource<ID3D11SamplerState>(ss);
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
    ID3D11Buffer* pBuffers[16]				= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
    ID3D11SamplerState* pSamplers[16]		= { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

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
	ReSetRenderTargets(8);

    // States
    float BlendFactor[4] = { 0,0,0,0 };
	SetBlendState(BlendState::Opaque, BlendFactor, 0xFFFFFFFF);
	SetDepthStencilState(DepthState::Greater, 0x00);  // we are using inverted 32-bit float depth for better precision
	SetRasterizerState(RasterizerState::CWDefault);
}

void DirectX::SetRasterizerState(RasterizerState state)
{
	_immediateContext->RSSetState(_rasterizerStates[static_cast<uint>(state)]);
}

void DirectX::SetDepthStencilState(DepthState state, uint stencilRef)
{
	_immediateContext->OMSetDepthStencilState(_depthStencilStates[static_cast<uint>(state)], stencilRef);
}

void DirectX::SetBlendState(BlendState state, const float blendFactor[4], uint sampleMask)
{
	_immediateContext->OMSetBlendState(_blendStates[static_cast<uint>(state)], blendFactor, sampleMask);
}

void DirectX::SetBlendState(BlendState state)
{
	float blendFactor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
	_immediateContext->OMSetBlendState(_blendStates[static_cast<uint>(state)], blendFactor, 0xffffffff);
}

void DirectX::SetPrimitiveTopology(PrimitiveTopology topology)
{
	_immediateContext->IASetPrimitiveTopology(static_cast<D3D_PRIMITIVE_TOPOLOGY>(topology));
}

const ShaderMacro DirectX::GetMSAAShaderMacro() const
{
	return ShaderMacro("MSAA_SAMPLES_COUNT", std::to_string(_msaaDesc.Count));
}

const Rect<float> DirectX::FetchViewportRect()
{
	uint num = 1;
	D3D11_VIEWPORT vp;
	_immediateContext->RSGetViewports(&num, &vp);

	return Rect<float>(vp.TopLeftX, vp.TopLeftY, vp.Width, vp.Height);;
}
