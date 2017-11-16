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
#include "RenderState.h"

#include "RenderTexture.h"
#include "DepthMap.h"

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
}

namespace Device
{
	class DirectX final
	{
	public:
		DirectX(const Rect<float>& backBufferRect);
		~DirectX() = default;

		DISALLOW_ASSIGN(DirectX);
		DISALLOW_COPY_CONSTRUCTOR(DirectX);

	public:
		unsigned int		CalcFormatSize(DXGI_FORMAT format) const;
		void				ClearDeviceContext();
		const Rect<float>	FetchViewportRect();

		void SetViewport(const Rect<float>& rect);
		void RestoreViewportToBackBuffer();

		void SetRenderTargets(const uint numRTs, Rendering::Texture::RenderTexture* const*, ID3D11DepthStencilView* dsv = nullptr);
		void SetRenderTargets(const uint numRTs, Rendering::Texture::RenderTexture* const*, Rendering::Texture::DepthMap&);
		void SetRenderTarget(Rendering::Texture::RenderTexture& target, ID3D11DepthStencilView* dsv = nullptr);
		void SetRenderTarget(Rendering::Texture::RenderTexture& target, Rendering::Texture::DepthMap& targetDepthMap);
		void ReSetRenderTargets(const uint numRTs);
		void SetUAVsWithoutRenderTarget(uint uavStartSlot, const uint numUAVs, Rendering::View::UnorderedAccessView* const*);
		void ReSetUAVsWithoutRenderTarget(const uint uavStartSlot, const uint numUAVs);
		void SetDepthMapWithoutRenderTarget(Rendering::Texture::DepthMap&);
			 
		void SetBlendState(Rendering::RenderState::BlendState state, const float blendFactor[4], uint sampleMask);
		void SetBlendState(Rendering::RenderState::BlendState state);
			 
		void SetDepthStencilState(Rendering::RenderState::DepthState state, uint stencilRef);
		void SetRasterizerState(Rendering::RenderState::RasterizerState state);
		void SetPrimitiveTopology(Rendering::RenderState::PrimitiveTopology);

	public:
		GET_ACCESSOR(Device,				ID3D11Device* const,			_device.GetRaw());
		GET_ACCESSOR(SwapChain,				IDXGISwapChain* const,			_swapChain.GetRaw());
		GET_ACCESSOR(Context,				ID3D11DeviceContext* const,		_immediateContext.GetRaw());
		GET_ACCESSOR(BackBufferRT,			auto&,							_backBufferRenderTexture);
			
		GET_CONST_ACCESSOR(MSAADesc,		const DXGI_SAMPLE_DESC&,		_msaaDesc);

		GET_CONST_ACCESSOR(FeatureLevel,	D3D_FEATURE_LEVEL,				_featureLevel);
		GET_CONST_ACCESSOR(DriverType,		D3D_DRIVER_TYPE,				_driverType);

		SET_ACCESSOR(BackBufferRect,		const Rect<float>&,				_backBufferRect);
		GET_CONST_ACCESSOR(BackBufferRect,	const auto&,					_backBufferRect);

		const Rendering::Shader::ShaderMacro GetMSAAShaderMacro() const;
		const auto& GetSamplerState(Rendering::RenderState::SamplerState state) const { return _samplerStates[static_cast<uint>(state)]; }

	public:
		DXSharedResource<ID3D11ShaderResourceView>	CreateShaderResourceView(ID3D11Resource* const rawResource, const D3D11_SHADER_RESOURCE_VIEW_DESC& desc);
		DXSharedResource<ID3D11UnorderedAccessView>	CreateUnorderedAccessView(ID3D11Resource* const rawResource, const D3D11_UNORDERED_ACCESS_VIEW_DESC& desc);
		DXSharedResource<ID3D11RenderTargetView>	CreateRenderTargetView(ID3D11Resource* const rawResource, const D3D11_RENDER_TARGET_VIEW_DESC& desc);
		DXSharedResource<ID3D11DepthStencilView>	CreateDepthStencilView(ID3D11Resource* const rawResource, const D3D11_DEPTH_STENCIL_VIEW_DESC& desc);

		DXSharedResource<ID3D11Buffer>				CreateBuffer(const D3D11_BUFFER_DESC& desc, const D3D11_SUBRESOURCE_DATA* data);

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
		Rendering::Texture::RenderTexture			_backBufferRenderTexture;

		std::array<DXUniqueResource<ID3D11RasterizerState>,		Rendering::RenderState::MaxCountRasterizerState>	_rasterizerStates;
		std::array<DXUniqueResource<ID3D11BlendState>,			Rendering::RenderState::MaxCountBlendState>			_blendStates;
		std::array<DXUniqueResource<ID3D11DepthStencilState>,	Rendering::RenderState::MaxCountDepthStencilState>	_depthStencilStates;
		std::array<DXUniqueResource<ID3D11SamplerState>,		Rendering::RenderState::MaxCountSamplerState>		_samplerStates;

		DXGI_SAMPLE_DESC							_msaaDesc;
		D3D_FEATURE_LEVEL							_featureLevel;
		D3D_DRIVER_TYPE								_driverType;

		Rect<float>									_backBufferRect;
	};
}