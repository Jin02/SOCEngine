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

		DISALLOW_ASSIGN(DirectX);
		DISALLOW_COPY_CONSTRUCTOR(DirectX);

	public:
		unsigned int		CalcFormatSize(DXGI_FORMAT format) const;
		void				ClearDeviceContext();
		const Size<uint>	FetchBackBufferSize();

	public:
		GET_CONST_ACCESSOR(Device,										DXResource<ID3D11Device>,				_device);
		GET_CONST_ACCESSOR(SwapChain,									DXResource<IDXGISwapChain>,				_swapChain);
		GET_CONST_ACCESSOR(Context,										DXResource<ID3D11DeviceContext>,		_immediateContext);
		GET_CONST_ACCESSOR(BackBufferRTV,								DXResource<ID3D11RenderTargetView>,		_renderTargetView);
		GET_CONST_ACCESSOR(RasterizerStateCCWDisableCulling,			DXResource<ID3D11RasterizerState>,		_rasterizerCounterClockwiseDisableCulling);
		GET_CONST_ACCESSOR(RasterizerStateCCWDefaultState,				DXResource<ID3D11RasterizerState>,		_rasterizerCounterClockwiseDefault);
		GET_CONST_ACCESSOR(RasterizerStateCWDisableCulling,				DXResource<ID3D11RasterizerState>,		_rasterizerClockwiseDisableCulling);
		GET_CONST_ACCESSOR(RasterizerStateCWDisableCullingWithClip,		DXResource<ID3D11RasterizerState>,		_rasterizerClockwiseDisableCullingWithClip);
		GET_CONST_ACCESSOR(RasterizerStateCWDefaultState,				DXResource<ID3D11RasterizerState>,		_rasterizerClockwiseDefault);
		GET_CONST_ACCESSOR(BlendStateOpaque,							DXResource<ID3D11BlendState>,			_blendOpaque);
		GET_CONST_ACCESSOR(BlendStateAlphaToCoverage,					DXResource<ID3D11BlendState>,			_blendAlphaToCoverage);
		GET_CONST_ACCESSOR(BlendStateAlpha,								DXResource<ID3D11BlendState>,			_blendAlpha);
		GET_CONST_ACCESSOR(DepthStateDisableDepthWrite,					DXResource<ID3D11DepthStencilState>,	_depthDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateDisableDepthTest,					DXResource<ID3D11DepthStencilState>,	_depthDisableDepthTest);
		GET_CONST_ACCESSOR(DepthStateLess,								DXResource<ID3D11DepthStencilState>,	_depthLess);
		GET_CONST_ACCESSOR(DepthStateEqualAndDisableDepthWrite,			DXResource<ID3D11DepthStencilState>,	_depthEqualAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateGreater,							DXResource<ID3D11DepthStencilState>,	_depthGreater);
		GET_CONST_ACCESSOR(DepthStateGreaterAndDisableDepthWrite,		DXResource<ID3D11DepthStencilState>,	_depthGreaterAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateGreaterEqualAndDisableDepthWrite,	DXResource<ID3D11DepthStencilState>,	_depthGreaterEqualAndDisableDepthWrite);
		GET_CONST_ACCESSOR(DepthStateLessEqual,							DXResource<ID3D11DepthStencilState>,	_depthLessEqual);
		GET_CONST_ACCESSOR(SamplerStateAnisotropic,						DXResource<ID3D11SamplerState>,			_samplerAnisotropic);
		GET_CONST_ACCESSOR(SamplerStateLinear,							DXResource<ID3D11SamplerState>,			_samplerLinear);
		GET_CONST_ACCESSOR(SamplerStatePoint,							DXResource<ID3D11SamplerState>,			_samplerPoint);
		GET_CONST_ACCESSOR(SamplerStateShadowLessEqual,					DXResource<ID3D11SamplerState>,			_samplerShadowLessEqualComp);
		GET_CONST_ACCESSOR(SamplerStateShadowGreaterEqual,				DXResource<ID3D11SamplerState>,			_samplerShadowGreaterEqualComp);
		GET_CONST_ACCESSOR(SamplerStateShadowLinear,					DXResource<ID3D11SamplerState>,			_samplerShadowLinear);
		GET_CONST_ACCESSOR(SamplerStateConeTracing,						DXResource<ID3D11SamplerState>,			_samplerConeTracing);

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
		void InitViewport(const Rect<float>& rect);
		void CreateRenderTargetView();
		void CreateDeviceAndSwapChain(const WinApp& win, const Size<uint>& viewportSize, bool useMSAA);
		void CreateBlendStates();

		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);

	private:
		DXResource<ID3D11Device>				_device;
		DXResource<IDXGISwapChain>				_swapChain;
		DXResource<ID3D11DeviceContext>			_immediateContext;
		DXResource<ID3D11RenderTargetView>		_renderTargetView;
		DXResource<ID3D11RasterizerState>		_rasterizerClockwiseDisableCulling;
		DXResource<ID3D11RasterizerState>		_rasterizerClockwiseDefault;
		DXResource<ID3D11RasterizerState>		_rasterizerCounterClockwiseDisableCulling;
		DXResource<ID3D11RasterizerState>		_rasterizerClockwiseDisableCullingWithClip;
		DXResource<ID3D11RasterizerState>		_rasterizerCounterClockwiseDefault;
		DXResource<ID3D11BlendState>			_blendOpaque;
		DXResource<ID3D11BlendState>			_blendAlphaToCoverage;
		DXResource<ID3D11BlendState>			_blendAlpha;
		DXResource<ID3D11DepthStencilState>		_depthDisableDepthTest;
		DXResource<ID3D11DepthStencilState>		_depthDisableDepthWrite;
		DXResource<ID3D11DepthStencilState>		_depthLess;
		DXResource<ID3D11DepthStencilState>		_depthLessEqual;
		DXResource<ID3D11DepthStencilState>		_depthEqualAndDisableDepthWrite;
		DXResource<ID3D11DepthStencilState>		_depthGreater;
		DXResource<ID3D11DepthStencilState>		_depthGreaterAndDisableDepthWrite;
		DXResource<ID3D11DepthStencilState>		_depthGreaterEqualAndDisableDepthWrite;
		DXResource<ID3D11SamplerState>			_samplerAnisotropic;
		DXResource<ID3D11SamplerState>			_samplerLinear;
		DXResource<ID3D11SamplerState>			_samplerPoint;
		DXResource<ID3D11SamplerState>			_samplerShadowLessEqualComp;
		DXResource<ID3D11SamplerState>			_samplerShadowGreaterEqualComp;
		DXResource<ID3D11SamplerState>			_samplerShadowLinear;
		DXResource<ID3D11SamplerState>			_samplerConeTracing;

		DXGI_SAMPLE_DESC			_msaaDesc;
		Size<uint>					_backBufferSize;
		D3D_FEATURE_LEVEL			_featureLevel;
		D3D_DRIVER_TYPE				_driverType;
	};
}
