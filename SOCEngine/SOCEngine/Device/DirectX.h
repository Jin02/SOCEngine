#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>

#include "Common.h"
#include "WinApp.h"

#include "Color.h"
#include "ShaderMacro.h"

#include "Matrix.h"
#include "Rect.h"

namespace Core
{
	class Launcher;
}

namespace Device
{
	class DirectX final
	{
	public:
		DirectX();
		~DirectX(void);

		DISALLOW_ASSIGN(DirectX);
		DISALLOW_COPY_CONSTRUCTOR(DirectX);

	public:
		unsigned int		CalcFormatSize(DXGI_FORMAT format) const;
		void				ClearDeviceContext() const;
		const Size<uint>	FetchBackBufferSize();

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

		GET_ACCESSOR(BlendStateOpaque,								ID3D11BlendState*,			_blendOpaque);
		GET_ACCESSOR(BlendStateAlphaToCoverage,						ID3D11BlendState*,			_blendAlphaToCoverage);
		GET_ACCESSOR(BlendStateAlpha,								ID3D11BlendState*,			_blendAlpha);

		GET_ACCESSOR(DepthStateDisableDepthWrite,					ID3D11DepthStencilState*,	_depthDisableDepthWrite);
		GET_ACCESSOR(DepthStateDisableDepthTest,					ID3D11DepthStencilState*,	_depthDisableDepthTest);
		GET_ACCESSOR(DepthStateLess,								ID3D11DepthStencilState*,	_depthLess);
		GET_ACCESSOR(DepthStateEqualAndDisableDepthWrite,			ID3D11DepthStencilState*,	_depthEqualAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateGreater,								ID3D11DepthStencilState*,	_depthGreater);
		GET_ACCESSOR(DepthStateGreaterAndDisableDepthWrite,			ID3D11DepthStencilState*,	_depthGreaterAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateGreaterEqualAndDisableDepthWrite,	ID3D11DepthStencilState*,	_depthGreaterEqualAndDisableDepthWrite);
		GET_ACCESSOR(DepthStateLessEqual,							ID3D11DepthStencilState*,	_depthLessEqual);

		GET_ACCESSOR(SamplerStateAnisotropic,						ID3D11SamplerState*,		_samplerAnisotropic);
		GET_ACCESSOR(SamplerStateLinear,							ID3D11SamplerState*,		_samplerLinear);
		GET_ACCESSOR(SamplerStatePoint,								ID3D11SamplerState*,		_samplerPoint);
		GET_ACCESSOR(SamplerStateShadowLessEqual,					ID3D11SamplerState*,		_samplerShadowLessEqualComp);
		GET_ACCESSOR(SamplerStateShadowGreaterEqual,				ID3D11SamplerState*,		_samplerShadowGreaterEqualComp);
		GET_ACCESSOR(SamplerStateShadowLinear,						ID3D11SamplerState*,		_samplerShadowLinear);
		GET_ACCESSOR(SamplerStateConeTracing,						ID3D11SamplerState*,		_samplerConeTracing);

		GET_ACCESSOR(MSAADesc,										const DXGI_SAMPLE_DESC&,	_msaaDesc);
		const Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;

		GET_ACCESSOR(BackBufferSize,								const Size<uint>&,			_backBufferSize);

		GET_ACCESSOR(FeatureLevel,									D3D_FEATURE_LEVEL,			_featureLevel);
		GET_ACCESSOR(DriverType,									D3D_DRIVER_TYPE,			_driverType);

	private:
		friend class Core::Launcher;
		void Initialize(const WinApp& win, const Rect<uint>& viewport, bool useMSAA);
		void InitViewport(const Rect<uint>& rect);
		void CreateRenderTargetView();
		void CreateDeviceAndSwapChain(const WinApp& win, const Size<uint>& viewportSize, bool useMSAA);
		void CreateBlendStates();

		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);

		void Destroy();

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

		ID3D11BlendState*			_blendOpaque;
		ID3D11BlendState*			_blendAlphaToCoverage;
		ID3D11BlendState*			_blendAlpha;

		ID3D11DepthStencilState*	_depthDisableDepthTest;
		ID3D11DepthStencilState*	_depthDisableDepthWrite;
		ID3D11DepthStencilState*	_depthLess;
		ID3D11DepthStencilState*	_depthLessEqual;
		ID3D11DepthStencilState*	_depthEqualAndDisableDepthWrite;
		ID3D11DepthStencilState*	_depthGreater;
		ID3D11DepthStencilState*	_depthGreaterAndDisableDepthWrite;
		ID3D11DepthStencilState*	_depthGreaterEqualAndDisableDepthWrite;

		ID3D11SamplerState*			_samplerAnisotropic;
		ID3D11SamplerState*			_samplerLinear;
		ID3D11SamplerState*			_samplerPoint;
		ID3D11SamplerState*			_samplerShadowLessEqualComp;
		ID3D11SamplerState*			_samplerShadowGreaterEqualComp;
		ID3D11SamplerState*			_samplerShadowLinear;
		ID3D11SamplerState*			_samplerConeTracing;

		DXGI_SAMPLE_DESC			_msaaDesc;
		Size<uint>					_backBufferSize;
	};
}