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

	ID3D11Texture2D			*_depthStencil;
	ID3D11DepthStencilView	*_depthStencilView;

	ID3D11RenderTargetView	*_renderTargetView;

public:
	DX()
	{

	}

	bool InitDevice(const Win32* win)
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

		DXGI_SWAP_CHAIN_DESC	backBufferInfo;
		D3D_DRIVER_TYPE			driverType;
		D3D_FEATURE_LEVEL		featureLevel;


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

		//Multi Sampling
		backBufferInfo.SampleDesc.Count = 1;
		backBufferInfo.SampleDesc.Quality = 0;	

		HRESULT hr;
		for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
		{
			driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(
				NULL, driverType, NULL, 
				createDeviceFlags, featureLevels,
				numFeatureLevels, D3D11_SDK_VERSION,
				&backBufferInfo, &_swapChain,
				&_device, &featureLevel,
				&_immediateContext );

			if( SUCCEEDED( hr ) )
				break;
		}

		if( FAILED(hr) )
			return false;

		ID3D11Texture2D* backBuffer = nullptr;
		hr = _swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&backBuffer);
		backBuffer->Release();

		if( FAILED(hr) )
			return false;

		D3D11_TEXTURE2D_DESC descDepth;
		memset( &descDepth, 0, sizeof(D3D11_TEXTURE2D_DESC) );

		descDepth.Width = winSize.w;
		descDepth.Height = winSize.h;
		descDepth.MipLevels = 1;
		descDepth.ArraySize = 1;
		descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		descDepth.SampleDesc.Count = 1;
		descDepth.SampleDesc.Quality = 0;
		descDepth.Usage = D3D11_USAGE_DEFAULT;
		descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		descDepth.CPUAccessFlags = 0;
		descDepth.MiscFlags = 0;

		hr = _device->CreateTexture2D( &descDepth, nullptr, &_depthStencil );
		if( FAILED( hr ) )
			return false;

		D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
		memset( &descDSV, 0, sizeof(D3D11_TEXTURE2D_DESC) );
		descDSV.Format = descDepth.Format;
		descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descDSV.Texture2D.MipSlice = 0;
		hr = _device->CreateDepthStencilView( _depthStencil, &descDSV, &_depthStencilView );
		if( FAILED( hr ) )
			return hr;

		_immediateContext->OMSetRenderTargets( 1, &_renderTargetView, _depthStencilView );

		// Setup the viewport
		D3D11_VIEWPORT vp;
		vp.Width = (FLOAT)winSize.w;
		vp.Height = (FLOAT)winSize.h;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;
		_immediateContext->RSSetViewports( 1, &vp );
	}

	~DX(void)
	{

	}

public:
};


}