#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <vector>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxgi.lib")

#include "Common.h"
#include "Win32.h"

#include "Color.h"
#include "ShaderMacro.h"

#include "Matrix.h"
#include "Size.h"
#include "Rect.h"

namespace Device
{
	class DirectX
	{
	private:
		ID3D11Device				*_device;
		IDXGISwapChain				*_swapChain;
		ID3D11DeviceContext			*_immediateContext;

		ID3D11RenderTargetView		*_renderTargetView;

		D3D_FEATURE_LEVEL			_featureLevel;
		D3D_DRIVER_TYPE				_driverType;

		ID3D11RasterizerState		*_defaultCulling;
		ID3D11RasterizerState		*_disableCulling;

		ID3D11BlendState			*_opaqueBlend;
		ID3D11BlendState			*_opaqueBlendDepthOnly;

		ID3D11BlendState			*_alphaToCoverageBlend;
		ID3D11BlendState			*_alphaBlend;

		ID3D11DepthStencilState		*_depthDisableDepthTest;

		ID3D11DepthStencilState		*_depthLess;
		ID3D11DepthStencilState		*_depthEqualAndDisableDepthWrite;

		ID3D11DepthStencilState		*_depthGreater;
		ID3D11DepthStencilState		*_depthGreaterAndDisableDepthWrite;

		ID3D11SamplerState			*_anisotropicSamplerState;
		ID3D11SamplerState			*_linearSamplerState;
		ID3D11SamplerState			*_pointSamplerState;

		DXGI_SAMPLE_DESC			_msaaDesc;
		Math::Size<uint>			_backBufferSize;

	public:
		DirectX();
		~DirectX(void);

	private:
		bool CreateRenderTargetView();
		bool CreateDeviceAndSwapChain(const Win32* win, const DXGI_SAMPLE_DESC* multiSampler = nullptr);
		bool InitViewport(const Math::Rect<uint>& rect);
		bool CreateSwapChain(const Win32* win);
	
		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);		
		void CreateBlendStates(bool isDeferredRender);

	public:
		bool InitDevice(const Win32* win, const Math::Rect<uint>& renderScreenRect, bool isDeferredRender = false);
		unsigned int CalcFormatSize(DXGI_FORMAT format) const;
		void ClearDeviceContext();
		Math::Size<uint> FetchBackBufferSize();
		void GetViewportMatrix(Math::Matrix& outMat) const;

	public:
		GET_ACCESSOR(Device,								ID3D11Device*,				_device);
		GET_ACCESSOR(SwapChain,								IDXGISwapChain*,			_swapChain);
		GET_ACCESSOR(Context,								ID3D11DeviceContext*,		_immediateContext);

		GET_ACCESSOR(BackBufferRTV,							ID3D11RenderTargetView*,	_renderTargetView);

		GET_ACCESSOR(RasterizerStateDefaultCulling,			ID3D11RasterizerState*,		_defaultCulling);
		GET_ACCESSOR(RasterizerStateDisableCulling,			ID3D11RasterizerState*,		_disableCulling);

		GET_ACCESSOR(BlendStateOpaque,						ID3D11BlendState*,			_opaqueBlend);
		GET_ACCESSOR(BlendStateAlphaToCoverage,				ID3D11BlendState*,			_alphaToCoverageBlend);
		GET_ACCESSOR(BlendStateOpaqueDepthOnly,				ID3D11BlendState*,			_opaqueBlendDepthOnly);
		GET_ACCESSOR(BlendStateAlpha,						ID3D11BlendState*,			_alphaBlend);

		GET_ACCESSOR(DepthStateDisableDepthTest,			ID3D11DepthStencilState*,	_depthDisableDepthTest);
		GET_ACCESSOR(DepthStateLess,						ID3D11DepthStencilState*,	_depthLess);
		GET_ACCESSOR(DepthStateEqualAndDisableDepthWrite,	ID3D11DepthStencilState*,	_depthEqualAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateGreater,						ID3D11DepthStencilState*,	_depthGreater);
		GET_ACCESSOR(DepthStateGreaterAndDisableDepthWrite, ID3D11DepthStencilState*,	_depthGreaterAndDisableDepthWrite);

		GET_ACCESSOR(SamplerStateAnisotropic,				ID3D11SamplerState*,		_anisotropicSamplerState);
		GET_ACCESSOR(SamplerStateLinear,					ID3D11SamplerState*,		_linearSamplerState);
		GET_ACCESSOR(SamplerStatePoint,						ID3D11SamplerState*,		_pointSamplerState);

		GET_ACCESSOR(MSAADesc,								const DXGI_SAMPLE_DESC&,	_msaaDesc);
		Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;
		GET_ACCESSOR(BackBufferSize,						const Math::Size<uint>&,	_backBufferSize);

		GET_ACCESSOR(FeatureLevel,							D3D_FEATURE_LEVEL,			_featureLevel);
		GET_ACCESSOR(DriverType,							D3D_DRIVER_TYPE,			_driverType);
	};
}