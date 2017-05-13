#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>

#include "Common.h"
#include "WinApp.h"

#include "Color.h"
#include "ShaderMacro.hpp"

#include "Matrix.h"
#include "Rect.h"

#include "DXResource.h"

namespace Core
{
	class Launcher;
}

namespace Rendering
{
	namespace Shader
	{
		class BaseShader;
	}

	namespace Buffer
	{
		class BaseBuffer;
	}
}

namespace Device
{
	class DirectX final
	{
	public:
		DirectX();
		~DirectX() = default;

		DISALLOW_ASSIGN(DirectX);
		DISALLOW_COPY_CONSTRUCTOR(DirectX);

	public:
		unsigned int		CalcFormatSize(DXGI_FORMAT format) const;
		void				ClearDeviceContext();
		const Size<uint>	FetchBackBufferSize();

	public:
		GET_CONST_ACCESSOR(Device,										ID3D11Device*,				_device);
		GET_CONST_ACCESSOR(SwapChain,									IDXGISwapChain*,			_swapChain);
		GET_CONST_ACCESSOR(Context,										ID3D11DeviceContext*,		_immediateContext);
		GET_CONST_ACCESSOR(BackBufferRTV,								ID3D11RenderTargetView*,	_renderTargetView);
		GET_CONST_ACCESSOR(RasterizerStateCCWDisableCulling,			ID3D11RasterizerState*,		_rasterizerCounterClockwiseDisableCulling);
		GET_CONST_ACCESSOR(RasterizerStateCCWDefaultState,				ID3D11RasterizerState*,		_rasterizerCounterClockwiseDefault);
		GET_CONST_ACCESSOR(RasterizerStateCWDisableCulling,				ID3D11RasterizerState*,		_rasterizerClockwiseDisableCulling);
		GET_CONST_ACCESSOR(RasterizerStateCWDisableCullingWithClip,		ID3D11RasterizerState*,		_rasterizerClockwiseDisableCullingWithClip);
		GET_CONST_ACCESSOR(RasterizerStateCWDefaultState,				ID3D11RasterizerState*,		_rasterizerClockwiseDefault);
		GET_CONST_ACCESSOR(BlendStateOpaque,							ID3D11BlendState*,			_blendOpaque);
		GET_CONST_ACCESSOR(BlendStateAlphaToCoverage,					ID3D11BlendState*,			_blendAlphaToCoverage);
		GET_CONST_ACCESSOR(BlendStateAlpha,								ID3D11BlendState*,			_blendAlpha);
		GET_CONST_ACCESSOR(DepthStateDisableDepthWrite,					ID3D11DepthStencilState*,	_depthDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateDisableDepthTest,					ID3D11DepthStencilState*,	_depthDisableDepthTest);
		GET_CONST_ACCESSOR(DepthStateLess,								ID3D11DepthStencilState*,	_depthLess);
		GET_CONST_ACCESSOR(DepthStateEqualAndDisableDepthWrite,			ID3D11DepthStencilState*,	_depthEqualAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateGreater,							ID3D11DepthStencilState*,	_depthGreater);
		GET_CONST_ACCESSOR(DepthStateGreaterAndDisableDepthWrite,		ID3D11DepthStencilState*,	_depthGreaterAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateGreaterEqualAndDisableDepthWrite,	ID3D11DepthStencilState*,	_depthGreaterEqualAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateLessEqual,							ID3D11DepthStencilState*,	_depthLessEqual);
		GET_CONST_ACCESSOR(SamplerStateAnisotropic,						ID3D11SamplerState*,		_samplerAnisotropic);
		GET_CONST_ACCESSOR(SamplerStateLinear,							ID3D11SamplerState*,		_samplerLinear);
		GET_CONST_ACCESSOR(SamplerStatePoint,							ID3D11SamplerState*,		_samplerPoint);
		GET_CONST_ACCESSOR(SamplerStateShadowLessEqual,					ID3D11SamplerState*,		_samplerShadowLessEqualComp);
		GET_CONST_ACCESSOR(SamplerStateShadowGreaterEqual,				ID3D11SamplerState*,		_samplerShadowGreaterEqualComp);
		GET_CONST_ACCESSOR(SamplerStateShadowLinear,					ID3D11SamplerState*,		_samplerShadowLinear);
		GET_CONST_ACCESSOR(SamplerStateConeTracing,						ID3D11SamplerState*,		_samplerConeTracing);

		GET_CONST_ACCESSOR(MSAADesc,									const DXGI_SAMPLE_DESC&,	_msaaDesc);
		const Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;

		GET_CONST_ACCESSOR(BackBufferSize,								const Size<uint>&,			_backBufferSize);

		GET_CONST_ACCESSOR(FeatureLevel,								D3D_FEATURE_LEVEL,			_featureLevel);
		GET_CONST_ACCESSOR(DriverType,									D3D_DRIVER_TYPE,			_driverType);

	public:
		DXResource<ID3D11ShaderResourceView>	CreateShaderResourceView(ID3D11Resource* rawResource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);
		DXResource<ID3D11UnorderedAccessView>	CreateUnorderedAccessView(ID3D11Resource* rawResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc);
		DXResource<ID3D11RenderTargetView>		CreateRenderTargetView(ID3D11Resource* rawResource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);
		DXResource<ID3D11DepthStencilView>		CreateDepthStencilView(ID3D11Resource* rawResource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

		DXResource<ID3D11Buffer>				CreateBuffer(const D3D11_BUFFER_DESC& desc, const void* data);

		DXResource<ID3D11GeometryShader>		CreateGeometryShader(Rendering::Shader::BaseShader& baseShader);
		DXResource<ID3D11PixelShader>			CreatePixelShader(Rendering::Shader::BaseShader& baseShader);
		DXResource<ID3D11VertexShader>			CreateVertexShader(Rendering::Shader::BaseShader& baseShader);
		DXResource<ID3D11ComputeShader>			CreateComputeShader(Rendering::Shader::BaseShader& baseShader);

		DXResource<ID3D11InputLayout>			CreateInputLayout(Rendering::Shader::BaseShader& baseShader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);
		DXResource<ID3D11Texture2D>				CreateTexture2D(const D3D11_TEXTURE2D_DESC& desc);
		DXResource<ID3D11Texture3D>				CreateTexture3D(const D3D11_TEXTURE3D_DESC& desc);

	private:
		friend class Core::Launcher;
		void Initialize(const WinApp& win, const Rect<uint>& viewport, bool useMSAA);
		void InitViewport(const Rect<uint>& rect);
		void CreateRenderTargetView();
		void CreateDeviceAndSwapChain(const WinApp& win, const Size<uint>& viewportSize, bool useMSAA);
		void CreateBlendStates();

		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);

	private:
		ID3D11Device*				_device;
		IDXGISwapChain*				_swapChain;
		ID3D11DeviceContext*		_immediateContext;
		ID3D11RenderTargetView*		_renderTargetView;
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
		D3D_FEATURE_LEVEL			_featureLevel;
		D3D_DRIVER_TYPE				_driverType;
	};
}