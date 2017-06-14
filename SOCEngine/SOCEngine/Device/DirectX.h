#pragma once

#include <d3d11_1.h>
#include <d3dcompiler.h>
#include <vector>
#include <memory>
#include <functional>

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
		const Rect<float>	FetchViewportRect();
		void				SetViewport(const Rect<float>& rect);

	public:
		GET_ACCESSOR(Device,									ID3D11Device*,				_device.GetRaw());
		GET_ACCESSOR(SwapChain,									IDXGISwapChain*,			_swapChain.GetRaw());
		GET_ACCESSOR(Context,									ID3D11DeviceContext*,		_immediateContext.GetRaw());
		GET_ACCESSOR(BackBufferRTV,								ID3D11RenderTargetView*,	_renderTargetView.GetRaw());
		GET_ACCESSOR(RasterizerStateCCWDisableCulling,			ID3D11RasterizerState*,		_rasterizerCounterClockwiseDisableCulling.GetRaw());
		GET_ACCESSOR(RasterizerStateCCWDefaultState,			ID3D11RasterizerState*,		_rasterizerCounterClockwiseDefault.GetRaw());
		GET_ACCESSOR(RasterizerStateCWDisableCulling,			ID3D11RasterizerState*,		_rasterizerClockwiseDisableCulling.GetRaw());
		GET_ACCESSOR(RasterizerStateCWDisableCullingWithClip,	ID3D11RasterizerState*,		_rasterizerClockwiseDisableCullingWithClip.GetRaw());
		GET_ACCESSOR(RasterizerStateCWDefaultState,				ID3D11RasterizerState*,		_rasterizerClockwiseDefault.GetRaw());
		GET_ACCESSOR(BlendStateOpaque,							ID3D11BlendState*,			_blendOpaque.GetRaw());
		GET_ACCESSOR(BlendStateAlphaToCoverage,					ID3D11BlendState*,			_blendAlphaToCoverage.GetRaw());
		GET_ACCESSOR(BlendStateAlpha,							ID3D11BlendState*,			_blendAlpha.GetRaw());
		GET_ACCESSOR(DepthStateDisableDepthWrite,				ID3D11DepthStencilState*,	_depthDisableDepthWrite.GetRaw());
		GET_ACCESSOR(DepthStateDisableDepthTest,				ID3D11DepthStencilState*,	_depthDisableDepthTest.GetRaw());
		GET_ACCESSOR(DepthStateLess,							ID3D11DepthStencilState*,	_depthLess.GetRaw());
		GET_ACCESSOR(DepthStateEqualAndDisableDepthWrite,		ID3D11DepthStencilState*,	_depthEqualAndDisableDepthWrite.GetRaw());
		GET_ACCESSOR(DepthStateGreater,							ID3D11DepthStencilState*,	_depthGreater.GetRaw());
		GET_ACCESSOR(DepthStateGreaterAndDisableDepthWrite,		ID3D11DepthStencilState*,	_depthGreaterAndDisableDepthWrite.GetRaw());
		GET_ACCESSOR(DepthStateGreaterEqualAndDisableDepthWrite,ID3D11DepthStencilState*,	_depthGreaterEqualAndDisableDepthWrite.GetRaw());
		GET_ACCESSOR(DepthStateLessEqual,						ID3D11DepthStencilState*,	_depthLessEqual.GetRaw());
		GET_ACCESSOR(SamplerStateAnisotropic,					ID3D11SamplerState*,		_samplerAnisotropic.GetRaw());
		GET_ACCESSOR(SamplerStateLinear,						ID3D11SamplerState*,		_samplerLinear.GetRaw());
		GET_ACCESSOR(SamplerStatePoint,							ID3D11SamplerState*,		_samplerPoint.GetRaw());
		GET_ACCESSOR(SamplerStateShadowLessEqual,				ID3D11SamplerState*,		_samplerShadowLessEqualComp.GetRaw());
		GET_ACCESSOR(SamplerStateShadowGreaterEqual,			ID3D11SamplerState*,		_samplerShadowGreaterEqualComp.GetRaw());
		GET_ACCESSOR(SamplerStateShadowLinear,					ID3D11SamplerState*,		_samplerShadowLinear.GetRaw());
		GET_ACCESSOR(SamplerStateConeTracing,					ID3D11SamplerState*,		_samplerConeTracing.GetRaw());

		GET_CONST_ACCESSOR(MSAADesc,							const DXGI_SAMPLE_DESC&,	_msaaDesc);
		const Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;

		GET_CONST_ACCESSOR(BackBufferSize,						const Size<float>&,			_backBufferSize);

		GET_CONST_ACCESSOR(FeatureLevel,						D3D_FEATURE_LEVEL,			_featureLevel);
		GET_CONST_ACCESSOR(DriverType,							D3D_DRIVER_TYPE,			_driverType);

	public:
		DXSharedResource<ID3D11ShaderResourceView>	CreateShaderResourceView(ID3D11Resource* rawResource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);
		DXSharedResource<ID3D11UnorderedAccessView>	CreateUnorderedAccessView(ID3D11Resource* rawResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc);
		DXSharedResource<ID3D11RenderTargetView>	CreateRenderTargetView(ID3D11Resource* rawResource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);
		DXSharedResource<ID3D11DepthStencilView>	CreateDepthStencilView(ID3D11Resource* rawResource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

		DXSharedResource<ID3D11Buffer>				CreateBuffer(const D3D11_BUFFER_DESC& desc, const void* data);

		DXSharedResource<ID3D11GeometryShader>		CreateGeometryShader(Rendering::Shader::BaseShader& baseShader);
		DXSharedResource<ID3D11PixelShader>			CreatePixelShader(Rendering::Shader::BaseShader& baseShader);
		DXSharedResource<ID3D11VertexShader>		CreateVertexShader(Rendering::Shader::BaseShader& baseShader);
		DXSharedResource<ID3D11ComputeShader>		CreateComputeShader(Rendering::Shader::BaseShader& baseShader);

		DXSharedResource<ID3D11InputLayout>			CreateInputLayout(Rendering::Shader::BaseShader& baseShader, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDeclations);
		DXSharedResource<ID3D11Texture2D>			CreateTexture2D(const D3D11_TEXTURE2D_DESC& desc);
		DXSharedResource<ID3D11Texture3D>			CreateTexture3D(const D3D11_TEXTURE3D_DESC& desc);

	private:
		friend class Core::Launcher;
		void Initialize(const WinApp& win, const Rect<uint>& viewport, bool useMSAA);
		void CreateRenderTargetView();
		void CreateDeviceAndSwapChain(const WinApp& win, const Size<uint>& viewportSize, bool useMSAA);
		void CreateBlendStates();

		void CheckAbleMultiSampler(std::vector<DXGI_SAMPLE_DESC>& outDescs, DXGI_FORMAT format);

	private:
		DXUniqueResource<ID3D11Device>				_device;
		DXUniqueResource<IDXGISwapChain>			_swapChain;
		DXUniqueResource<ID3D11DeviceContext>		_immediateContext;
		DXUniqueResource<ID3D11RenderTargetView>	_renderTargetView;
		DXUniqueResource<ID3D11RasterizerState>		_rasterizerClockwiseDisableCulling;
		DXUniqueResource<ID3D11RasterizerState>		_rasterizerClockwiseDefault;
		DXUniqueResource<ID3D11RasterizerState>		_rasterizerCounterClockwiseDisableCulling;
		DXUniqueResource<ID3D11RasterizerState>		_rasterizerClockwiseDisableCullingWithClip;
		DXUniqueResource<ID3D11RasterizerState>		_rasterizerCounterClockwiseDefault;
		DXUniqueResource<ID3D11BlendState>			_blendOpaque;
		DXUniqueResource<ID3D11BlendState>			_blendAlphaToCoverage;
		DXUniqueResource<ID3D11BlendState>			_blendAlpha;
		DXUniqueResource<ID3D11DepthStencilState>	_depthDisableDepthTest;
		DXUniqueResource<ID3D11DepthStencilState>	_depthDisableDepthWrite;
		DXUniqueResource<ID3D11DepthStencilState>	_depthLess;
		DXUniqueResource<ID3D11DepthStencilState>	_depthLessEqual;
		DXUniqueResource<ID3D11DepthStencilState>	_depthEqualAndDisableDepthWrite;
		DXUniqueResource<ID3D11DepthStencilState>	_depthGreater;
		DXUniqueResource<ID3D11DepthStencilState>	_depthGreaterAndDisableDepthWrite;
		DXUniqueResource<ID3D11DepthStencilState>	_depthGreaterEqualAndDisableDepthWrite;
		DXUniqueResource<ID3D11SamplerState>		_samplerAnisotropic;
		DXUniqueResource<ID3D11SamplerState>		_samplerLinear;
		DXUniqueResource<ID3D11SamplerState>		_samplerPoint;
		DXUniqueResource<ID3D11SamplerState>		_samplerShadowLessEqualComp;
		DXUniqueResource<ID3D11SamplerState>		_samplerShadowGreaterEqualComp;
		DXUniqueResource<ID3D11SamplerState>		_samplerShadowLinear;
		DXUniqueResource<ID3D11SamplerState>		_samplerConeTracing;

		DXGI_SAMPLE_DESC			_msaaDesc;
		Size<float>					_backBufferSize;
		D3D_FEATURE_LEVEL			_featureLevel;
		D3D_DRIVER_TYPE				_driverType;
	};
}