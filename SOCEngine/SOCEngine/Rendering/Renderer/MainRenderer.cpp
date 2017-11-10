#include "MainRenderer.h"
#include "LightCullingUtility.h"
#include "ShaderFactory.hpp"
#include "MeshUtility.h"

using namespace Core;
using namespace Math;
using namespace Device;
using namespace Rendering;
using namespace Rendering::Camera;
using namespace Rendering::Shader;
using namespace Rendering::Texture;
using namespace Rendering::Manager;
using namespace Rendering::Renderer;
using namespace Rendering::Geometry;
using namespace Rendering::RenderState;

void MainRenderer::Initialize(DirectX& dx, ShaderManager& shaderMgr, const MainCamera& mainCamera)
{
	_blendedDepthLightCulling.Initialize(dx, shaderMgr, mainCamera.GetRenderRect().size);

	auto backBufferSize = dx.GetBackBufferSize().Cast<uint>();

	// Result Map
	{
		auto size = backBufferSize;
		{
			if (dx.GetMSAADesc().Count > 1)
			{
				size.w *= 2;
				size.h *= 2;
			}
		}

		float maxLength	= static_cast<float>( std::max(size.w, size.h) );
		uint mipLevel	= static_cast<uint>( log(maxLength) / log(2.0f) ) + 1;
		_resultMap.Initialize(dx, size.Cast<uint>(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0, mipLevel);
	}

	// Gbuffer
	{
		_gbuffer.albedo_occlusion.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.normal_roughness.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.velocity_metallic_specularity.Initialize(dx, backBufferSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.emission_materialFlag.Initialize(dx, backBufferSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.opaqueDepthBuffer.Initialize(dx, backBufferSize, true);
		_gbuffer.blendedDepthBuffer.Initialize(dx, backBufferSize, true);
	}

	_tbrCB.Initialize(dx);

	// Load Shader
	{
		std::vector<Shader::ShaderMacro> macros{	dx.GetMSAAShaderMacro(),
													ShaderMacro("USE_COMPUTE_SHADER"),
													ShaderMacro("ENABLE_BLEND")		};

		Factory::ShaderFactory factory(&shaderMgr);
		_tbdrShader = *factory.LoadComputeShader(dx, "TBDR", "TileBasedDeferredShadingCS", &macros, "@TBDR");

		Size<uint> size = Light::CullingUtility::ComputeThreadGroupSize(backBufferSize);
		ComputeShader::ThreadGroup threadGroup(size.w, size.h, 1);
		_tbdrShader.SetThreadGroupInfo(threadGroup);
	}
}

void MainRenderer::UpdateCB(DirectX& dx, const MainCamera& mainCamera, const LightManager& lightMgr)
{
	if( (_dirty == false) & (mainCamera.GetDirty() == false) & (lightMgr.GetChangedLightCount() == false) )
		return;

	const auto& renderRect = mainCamera.GetRenderRect();

	TBRCBData tbrCBData;
	tbrCBData.invProjMat			= Matrix::Inverse(mainCamera.GetProjMatrix());
	tbrCBData.invViewProjMat		= Matrix::Inverse(mainCamera.GetViewProjMatrix());

	Matrix invViewportMat			= Matrix::ComputeInvViewportMatrix(mainCamera.GetRenderRect());
	tbrCBData.invViewProjViewport	= invViewportMat * tbrCBData.invViewProjMat;

	tbrCBData.invProjMat			= Matrix::Transpose(tbrCBData.invProjMat);
	tbrCBData.invViewProjMat		= Matrix::Transpose(tbrCBData.invViewProjMat);
	tbrCBData.invViewProjViewport	= Matrix::Transpose(tbrCBData.invViewProjViewport);
	tbrCBData.gamma					= _gamma;

	// packed tbr cb data
	{
		auto& packed					= tbrCBData.packedParam;
		packed.maxNumOfperLightInTile	= Light::CullingUtility::CalcMaxNumLightsInTile(mainCamera.GetRenderRect().size);
		packed.packedNumOfLights		= lightMgr.GetPackedLightCount();

		packed.packedViewportSize		= (renderRect.x << 16) | renderRect.y;
	}

	_tbrCB.UpdateSubResource(dx, tbrCBData);
	_dirty = false;
}

void MainRenderer::Render(DirectX& dx, const MainCamera& mainCamera)
{
	// Basic Setting
	{
		dx.SetViewport(mainCamera.GetRenderRect().Cast<float>());
		dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
	}

	// Clear GBuffer
	{
		_gbuffer.albedo_occlusion.Clear(dx, Color::Clear());
		_gbuffer.normal_roughness.Clear(dx, Color::Clear());
		_gbuffer.emission_materialFlag.Clear(dx, Color::Clear());
		_gbuffer.velocity_metallic_specularity.Clear(dx, Color::Clear());

		_gbuffer.opaqueDepthBuffer.Clear(dx, 0.0f, 0);
		_gbuffer.blendedDepthBuffer.Clear(dx, 0.0f, 0);

		_resultMap.Clear(dx, Color::Cyan());
	}

	// Render GBuffer
	{
		RenderTexture* renderTargets[] =
		{
			&_gbuffer.albedo_occlusion,
			&_gbuffer.normal_roughness,
			&_gbuffer.emission_materialFlag,
			&_gbuffer.velocity_metallic_specularity
		};

		dx.SetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, _gbuffer.opaqueDepthBuffer);
		dx.SetDepthStencilState(DepthState::Greater, 0);

		PixelShader::BindSamplerState(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Anisotropic);

		// Opaque
		{
			dx.SetRasterizerState(RasterizerState::CWDefault);
			dx.SetBlendState(BlendState::Opaque);

			// render
			// TODO: gbuffer 작성에 필요한 텍스쳐들 바인드 시켜야 함
		}

		// AlphaTest
		{
			if(dx.GetMSAADesc().Count > 1)	// on msaa
				dx.SetBlendState(BlendState::AlphaToCoverage);
			// msaa가 아니라면 BlendState::Opaque를 그대로 쓴다.

			dx.SetRasterizerState(RasterizerState::CWDisableCulling);

			//render
		}

		// Transparent
		{			
			dx.ReSetRenderTargets(ARRAYSIZE(renderTargets));
			dx.SetDepthMapWithoutRenderTarget(_gbuffer.blendedDepthBuffer);
			dx.SetBlendState(BlendState::Opaque);
			dx.SetRasterizerState(RasterizerState::CWDefault);

			// render
		}
	}
}
