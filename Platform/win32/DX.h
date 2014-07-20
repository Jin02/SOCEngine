#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include "Common.h"
#include "Win32.h"

namespace Device
{

class DX
{
private:
	ID3D11Device			*_device;
	IDXGISwapChain			*_swapChain;
	ID3D11DeviceContext		*_immediateContext;
	IDXGIAdapter			*_adapter;

	ID3D11Texture2D			*_depthStencil;
	ID3D11DepthStencilView	*_depthStencilView;

	ID3D11RenderTargetView	*_renderTargetView;
	ID3D11RasterizerState	*_rasterizerState;

private:
	int _multiSample;

public:
	DX()
	{

	}

	bool CreateReaterize()
	{
		D3D11_RASTERIZER_DESC rsState;
		rsState.FillMode = D3D11_FILL_SOLID;
		rsState.CullMode = D3D11_CULL_BACK;
		rsState.FrontCounterClockwise = true;
		rsState.DepthBias = 0;
		rsState.DepthBiasClamp = 0;
		rsState.SlopeScaledDepthBias = 0;
		rsState.DepthClipEnable = false;
		rsState.ScissorEnable = false;
		rsState.MultisampleEnable = _multiSample > 1;
		rsState.AntialiasedLineEnable = false;

		if( FAILED(_device->CreateRasterizerState(&rsState, &_rasterizerState)) )
			return false;

		_immediateContext->RSSetState( _rasterizerState );
		return true;
	}

	bool CreateRenderTargetView()
	{
		ID3D11Texture2D* backBuffer = nullptr;
		
		HRESULT hr = _swapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (void**)&backBuffer );
		
		if( FAILED(hr) )
			return false;

		hr = _device->CreateRenderTargetView( backBuffer, nullptr, &_renderTargetView );
		if( FAILED(hr) )
			return false;

		backBuffer->Release();

		return true;
	}

	bool CreateDefaultDepthStencilView(const Math::Size<int>& winSize)
	{
		ID3D11Texture2D* backBuffer = nullptr;
		ID3D11Texture2D* depthBuffer = nullptr;

		D3D11_TEXTURE2D_DESC descDepth;
		memset(&descDepth, 0, sizeof(D3D11_TEXTURE2D_DESC));
		descDepth.Width = winSize.w;
		descDepth.Height = winSize.h;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		if( FAILED( _device->CreateTexture2D( &descDepth, nullptr, &_depthStencil ) ) )
			return false;

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		switch( descDepth.Format )
		{
		case DXGI_FORMAT_R8_TYPELESS:
			descDSV.Format = DXGI_FORMAT_R8_UNORM;
			break;
		case DXGI_FORMAT_R16_TYPELESS:
			descDSV.Format = DXGI_FORMAT_D16_UNORM;
			break;
		case DXGI_FORMAT_R32_TYPELESS:
			descDSV.Format = DXGI_FORMAT_D32_FLOAT;
			break;
		case DXGI_FORMAT_R24G8_TYPELESS:
			descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
			break;
		default:
			descDSV.Format = descDepth.Format;
		}

		descDSV.ViewDimension = _multiSample > 1 ? 
D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;

		if( FAILED(_device->CreateDepthStencilView( depthBuffer, &descDSV, &_depthStencilView )) )
			return false;

		D3D11_DEPTH_STENCIL_DESC dsState;
		memset(&dsState, 0, sizeof(D3D11_DEPTH_STENCIL_DESC));
		dsState.DepthEnable = true;
		dsState.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		dsState.DepthFunc = D3D11_COMPARISON_LESS;
		dsState.StencilEnable = false;

		ID3D11DepthStencilState* depthStencilState = nullptr;
		if( FAILED(_device->CreateDepthStencilState( &dsState, &depthStencilState)) )
			return false;

		_immediateContext->OMSetDepthStencilState(depthStencilState, 0);

		return true;
	}

	bool CreateDevice()
	{
		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		UINT numDriverTypes = ARRAYSIZE( driverTypes );

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};

		int numFeatureLevels = ARRAYSIZE( featureLevels );

		unsigned int createDeviceFlags = 0;
#ifdef _DEBUG
		createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		D3D_DRIVER_TYPE			driverType;
		D3D_FEATURE_LEVEL		featureLevel;

		HRESULT hr = D3D11CreateDevice(
			_adapter, driverType, nullptr,
			createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &_device, &featureLevel, &_immediateContext);

		if( FAILED(hr) )
			return false;

		DXGI_ADAPTER_DESC desc;
		_adapter->GetDesc( &desc );

		return true;
	}

	bool CreateViewport(const Math::Size<int>& winSize)
	{
		D3D11_VIEWPORT vp;

		vp.Width = (FLOAT)winSize.w;
		vp.Height = (FLOAT)winSize.h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		_immediateContext->RSSetViewports( 1, &vp );
	}

	bool CreateAdapter()
	{
		IDXGIFactory* factory = nullptr;

		HRESULT hr = CreateDXGIFactory( __uuidof(IDXGIFactory), (void**)(&factory) );		
		if( FAILED(hr) )
			return false;

		hr = factory->EnumAdapters(0, &_adapter);
		SAFE_RELEASE(factory);

		if( FAILED(hr) )
			return false;

		return true;
	}

	bool CreateSwapChain(const Win32* win)
	{
		DXGI_SWAP_CHAIN_DESC	backBufferInfo;
		memset(&backBufferInfo, 0, sizeof(DXGI_SWAP_CHAIN_DESC));

		const Math::Size<int>& winSize = win->GetSize();
		backBufferInfo.BufferDesc.Width = winSize.w;
		backBufferInfo.BufferDesc.Height = winSize.h;

		backBufferInfo.BufferCount = 1;
		backBufferInfo.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		backBufferInfo.BufferDesc.RefreshRate.Numerator = 60;
		backBufferInfo.BufferDesc.RefreshRate.Denominator = 1;
		backBufferInfo.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		backBufferInfo.OutputWindow = win->GetHandle();
		backBufferInfo.Windowed = win->GetIsWindowMode();
		backBufferInfo.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		backBufferInfo.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		backBufferInfo.SampleDesc.Count = _multiSample;
		backBufferInfo.SampleDesc.Quality = 0;	

		unsigned int& multiSampleCount = backBufferInfo.SampleDesc.Count;
		unsigned int& multiSampleQuaility = backBufferInfo.SampleDesc.Quality;

		if( multiSampleCount != 0 )
		{
			HRESULT hr = _device->CheckMultisampleQualityLevels( backBufferInfo.BufferDesc.Format,
				multiSampleCount, &multiSampleQuaility );

			if( FAILED(hr) )
			{
				multiSampleCount = 1;
				multiSampleQuaility = 0;
			}
		}

		IDXGIFactory* factory = nullptr;
		if( FAILED(_adapter->GetParent( __uuidof(IDXGIFactory), (void**)factory)) )
			return false;

		if( FAILED( factory->CreateSwapChain(_device, &backBufferInfo, &_swapChain) ) )
			return false;

		return true;
	}

	void CalcMaxMultiSampler(unsigned int& outAbleMultiSample, unsigned int& outQualityLevel, DXGI_FORMAT format)
	{
		unsigned int qualityLevel = 0;
		unsigned int ableMultiSample = D3D11_MAX_MULTISAMPLE_SAMPLE_COUNT;
		for(ableMultiSample; ableMultiSample >= 0; ableMultiSample/=2)
		{
			HRESULT hr = _device->CheckMultisampleQualityLevels( format, ableMultiSample, &qualityLevel);

			if( SUCCEEDED(hr) )
				break;
		}

		outAbleMultiSample = ableMultiSample;
		outQualityLevel = qualityLevel;
	}

	bool InitDevice(const Win32* win)
	{

	}

	~DX(void)
	{

	}

public:
};


}