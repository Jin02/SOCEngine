#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <vector>

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
		ID3D11Device*				_device;
		IDXGISwapChain*				_swapChain;
		ID3D11DeviceContext*		_immediateContext;

		ID3D11RenderTargetView*		_renderTargetView;

		D3D_FEATURE_LEVEL			_featureLevel;
		D3D_DRIVER_TYPE				_driverType;

		ID3D11RasterizerState*		_rasterizerClockwiseDisableCulling;
		ID3D11RasterizerState*		_rasterizerClockwiseDefault;
		ID3D11RasterizerState*		_rasterizerCounterClockwiseDisableCulling;
		ID3D11RasterizerState*		_rasterizerClockwiseDisableCullingWithClip;
		ID3D11RasterizerState*		_rasterizerCounterClockwiseDefault;

		ID3D11BlendState*			_opaqueBlend;

		ID3D11BlendState*			_alphaToCoverageBlend;
		ID3D11BlendState*			_alphaBlend;

		ID3D11DepthStencilState*	_depthDisableDepthTest;
		ID3D11DepthStencilState*	_depthDisableDepthWrite;

		ID3D11DepthStencilState*	_depthLess;
		ID3D11DepthStencilState*	_depthLessEqual;
		ID3D11DepthStencilState*	_depthEqualAndDisableDepthWrite;

		ID3D11DepthStencilState*	_depthGreater;
		ID3D11DepthStencilState*	_depthGreaterAndDisableDepthWrite;
		ID3D11DepthStencilState*	_depthGreaterEqualAndDisableDepthWrite;

		ID3D11SamplerState*			_anisotropicSamplerState;
		ID3D11SamplerState*			_linearSamplerState;
		ID3D11SamplerState*			_pointSamplerState;
		ID3D11SamplerState*			_shadowLessEqualCompState;
		ID3D11SamplerState*			_shadowGreaterEqualCompState;
		ID3D11SamplerState*			_shadowLinearSamplerState;
		ID3D11SamplerState*			_coneTracingSamplerState;

		DXGI_SAMPLE_DESC			_msaaDesc;
		Math::Size<uint>			_backBufferSize;

	public:
		DirectX();
		~DirectX(void);

	private:
		bool CreateRenderTargetView();
		bool CreateDeviceAndSwapChain(const Win32* win, bool useMSAA);
		bool InitViewport(const Math::Rect<uint>& rect);
		bool CreateSwapChain(const Win32* win);
	
		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);		
		void CreateBlendStates();

	public:
		bool InitDevice(const Win32* win, const Math::Rect<uint>& renderScreenRect, bool useMSAA);
		unsigned int CalcFormatSize(DXGI_FORMAT format) const;
		void ClearDeviceContext() const;
		Math::Size<uint> FetchBackBufferSize();
		void Destroy();

	public:
		GET_ACCESSOR(Device,										ID3D11Device*,				_device);
		GET_ACCESSOR(SwapChain,										IDXGISwapChain*,			_swapChain);
		GET_ACCESSOR(Context,										ID3D11DeviceContext*,		_immediateContext);

		GET_ACCESSOR(BackBufferRTV,									ID3D11RenderTargetView*,	_renderTargetView);

		GET_ACCESSOR(RasterizerStateCCWDisableCulling,				ID3D11RasterizerState*,		_rasterizerCounterClockwiseDisableCulling);
		GET_ACCESSOR(RasterizerStateCCWDefaultState,				ID3D11RasterizerState*,		_rasterizerCounterClockwiseDefault);
		GET_ACCESSOR(RasterizerStateCWDisableCulling,				ID3D11RasterizerState*,		_rasterizerClockwiseDisableCulling);
		GET_ACCESSOR(RasterizerStateCWDisableCullingWithClip,		ID3D11RasterizerState*,		_rasterizerClockwiseDisableCullingWithClip);
		GET_ACCESSOR(RasterizerStateCWDefaultState,					ID3D11RasterizerState*,		_rasterizerClockwiseDefault);

		GET_ACCESSOR(BlendStateOpaque,								ID3D11BlendState*,			_opaqueBlend);
		GET_ACCESSOR(BlendStateAlphaToCoverage,						ID3D11BlendState*,			_alphaToCoverageBlend);
		GET_ACCESSOR(BlendStateAlpha,								ID3D11BlendState*,			_alphaBlend);

		GET_ACCESSOR(DepthStateDisableDepthWrite,					ID3D11DepthStencilState*,	_depthDisableDepthWrite);
		GET_ACCESSOR(DepthStateDisableDepthTest,					ID3D11DepthStencilState*,	_depthDisableDepthTest);
		GET_ACCESSOR(DepthStateLess,								ID3D11DepthStencilState*,	_depthLess);
		GET_ACCESSOR(DepthStateEqualAndDisableDepthWrite,			ID3D11DepthStencilState*,	_depthEqualAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateGreater,								ID3D11DepthStencilState*,	_depthGreater);
		GET_ACCESSOR(DepthStateGreaterAndDisableDepthWrite,			ID3D11DepthStencilState*,	_depthGreaterAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateGreaterEqualAndDisableDepthWrite,	ID3D11DepthStencilState*,	_depthGreaterEqualAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateLessEqual,							ID3D11DepthStencilState*,	_depthLessEqual);
		GET_ACCESSOR(SamplerStateAnisotropic,						ID3D11SamplerState*,		_anisotropicSamplerState);
		GET_ACCESSOR(SamplerStateLinear,							ID3D11SamplerState*,		_linearSamplerState);
		GET_ACCESSOR(SamplerStatePoint,								ID3D11SamplerState*,		_pointSamplerState);
		GET_ACCESSOR(ShadowLessEqualSamplerComparisonState,			ID3D11SamplerState*,		_shadowLessEqualCompState);
		GET_ACCESSOR(ShadowGreaterEqualSamplerComparisonState,		ID3D11SamplerState*,		_shadowGreaterEqualCompState);
		GET_ACCESSOR(ShadowSamplerState,							ID3D11SamplerState*,		_shadowLinearSamplerState);
		GET_ACCESSOR(ConeTracingSamplerState,						ID3D11SamplerState*,		_coneTracingSamplerState);

		GET_ACCESSOR(MSAADesc,										const DXGI_SAMPLE_DESC&,	_msaaDesc);
		Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;

		GET_ACCESSOR(BackBufferSize,								const Math::Size<uint>&,	_backBufferSize);

		GET_ACCESSOR(FeatureLevel,									D3D_FEATURE_LEVEL,			_featureLevel);
		GET_ACCESSOR(DriverType,									D3D_DRIVER_TYPE,			_driverType);
	};
}