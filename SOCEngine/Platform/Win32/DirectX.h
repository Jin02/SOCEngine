#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

#include "Common.h"
#include "Win32.h"

namespace Device
{
	class DirectX
	{
	private:
		ID3D11Device			*_device;
		IDXGISwapChain			*_swapChain;
		ID3D11DeviceContext		*_immediateContext;
		//IDXGIAdapter			*_adapter;

		ID3D11Texture2D			*_depthStencil;
		ID3D11DepthStencilView	*_depthStencilView;

		ID3D11RenderTargetView	*_renderTargetView;
		ID3D11RasterizerState	*_rasterizerState;

		D3D_FEATURE_LEVEL		 _featureLevel;
		D3D_DRIVER_TYPE			 _driverType;

	private:
		int _multiSample;

	public:
		DirectX();
		~DirectX(void);

	private:
		bool CreateRasterize();
		bool CreateRenderTargetView();
		bool CreateDefaultDepthStencilView(const Math::Size<int>& winSize);
		bool CreateDeviceAndSwapChain(const Win32* win);
		bool CreateViewport(const Math::Size<int>& winSize);
		bool CreateSwapChain(const Win32* win);
		void CalcMaxMultiSampler(unsigned int& outAbleMultiSample, unsigned int& outQualityLevel, DXGI_FORMAT format);

	public:
		bool InitDevice(const Win32* win);

	public:
		GET_ACCESSOR(Device, ID3D11Device*, _device);
		GET_ACCESSOR(SwapChain, IDXGISwapChain*, _swapChain);
		GET_ACCESSOR(Context, ID3D11DeviceContext*, _immediateContext);
		//GET_ACCESSOR(Adapter, IDXGIAdapter*, _adapter);

		GET_ACCESSOR(DetpthStencilTexture, ID3D11Texture2D*, _depthStencil);
		GET_ACCESSOR(DepthStencilView, ID3D11DepthStencilView*, _depthStencilView);

		GET_ACCESSOR(RenderTarget, ID3D11RenderTargetView*, _renderTargetView);
		GET_ACCESSOR(RasterizerState, ID3D11RasterizerState*, _rasterizerState);
	};
}