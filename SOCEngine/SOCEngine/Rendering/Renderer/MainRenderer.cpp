#include "MainRenderer.h"
#include "LightCullingUtility.h"
#include "ShaderFactory.hpp"
#include "MeshUtility.h"
#include "PhysicallyBasedMaterial.h"
#include "PixelShader.h"

#include "AutoBinder.hpp"

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
using namespace Rendering::Material;
using namespace Rendering::Light;
using namespace Rendering::Shadow;

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

template <class ShaderType>
inline void _BindGBufferResourceTextures(DirectX& dx, const MaterialForm::BindTextured2D* material)
{
	if(material)
		ShaderType::BindShaderResourceView(dx, static_cast<TextureBindIndex>(material->bindIndex), material->resource.GetShaderResourceView());
}

inline void BindGBufferResourceTextures(DirectX& dx, const PhysicallyBasedMaterial* material)
{
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetDiffuseMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetNormalMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetEmissionMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetMetallicMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetOpacityMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetRoughnessMap());
	_BindGBufferResourceTextures<PixelShader>(dx,		material->GetOcclusionMap());

	PixelShader::BindConstBuffer(dx, ConstBufferBindIndex::PhysicallyBasedMaterial, material->GetParamCB());

	_BindGBufferResourceTextures<GeometryShader>(dx,	material->GetHeightMap());
}

inline void UnBindGBufferResourceTextures(DirectX& dx)
{
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::DiffuseMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::NormalMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::EmissionMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::MetallicMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::OpacityMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::RoughnessMap);
	PixelShader::UnBindShaderResourceView(dx,		TextureBindIndex::HeightMap);

	PixelShader::UnBindConstBuffer(dx,				ConstBufferBindIndex::PhysicallyBasedMaterial);

	GeometryShader::UnBindShaderResourceView(dx,	TextureBindIndex::OcclusionMap);
}


void MainRenderer::Render(DirectX& dx, Param&& param)
{
	const auto& mainCamera = param.mainCamera;
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

	// 1 Pass - Render GBuffer
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

		AutoBinderSampler<PixelShader> defaultSampler(dx, SamplerStateBindIndex::DefaultSamplerState, SamplerState::Anisotropic);
		AutoBinderCB<VertexShader> cameraCB(dx, ConstBufferBindIndex::Camera, mainCamera.GetCameraCB());

		const auto& materialMgr = param.materialMgr;
		const auto& renderParam	= param.renderParam;
		// Opaque
		{
			dx.SetRasterizerState(RasterizerState::CWDefault);
			dx.SetBlendState(BlendState::Opaque);

			MeshRenderer::RenderOpaqueMeshes(dx, renderParam, DefaultRenderType::GBuffer_Opaque, mainCamera.GetOpaqueMeshRenderQ(),
				[&dx, &materialMgr](const Mesh* mesh) // Pre Render
				{
					const PhysicallyBasedMaterial* material = materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID());
					BindGBufferResourceTextures(dx, material);
				},
				[&dx](/*Post Render*/)
				{
					UnBindGBufferResourceTextures(dx);
				}
			);		
		}

		// AlphaTest
		{
			if(dx.GetMSAADesc().Count > 1)	// on msaa
				dx.SetBlendState(BlendState::AlphaToCoverage);
			// msaa가 아니라면 BlendState::Opaque를 그대로 쓴다.

			dx.SetRasterizerState(RasterizerState::CWDisableCulling);

			MeshRenderer::RenderAlphaTestMeshes(dx, renderParam, DefaultRenderType::GBuffer_AlphaTest, mainCamera.GetAlphaTestMeshRenderQ(),
				[&dx, &materialMgr](const Mesh* mesh)	// Pre-Render
				{
					const PhysicallyBasedMaterial* material = materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID());					
					BindGBufferResourceTextures(dx, material);
				},
				[&dx](/*Post Render*/) { UnBindGBufferResourceTextures(dx); }
			);	
		}

		// Transparent
		{			
			dx.ReSetRenderTargets(ARRAYSIZE(renderTargets));
			dx.SetDepthMapWithoutRenderTarget(_gbuffer.blendedDepthBuffer);
			dx.SetBlendState(BlendState::Opaque);
			dx.SetRasterizerState(RasterizerState::CWDefault);

			MeshRenderer::RenderTransparentMeshes(	dx, renderParam, DefaultRenderType::Forward_OnlyDepth,
													mainCamera.GetTransparentMeshRenderQ(), [](const Mesh*){}, [](){}	);	
		}

		VertexShader::UnBindShaderToContext(dx);
		PixelShader::UnBindShaderToContext(dx);
		GeometryShader::UnBindShaderToContext(dx);
		dx.ReSetRenderTargets(ARRAYSIZE(renderTargets));
	}

	const auto& lightMgr		= param.lightMgr;
	const auto& shadowMgr		= param.shadowMgr;
	const auto& shadowRenderer	= param.shadowRenderer;

	// 2 Pass - Light Culling and Deferred Shading
	{
		AutoBinderSampler<ComputeShader> shadowCompSampler(dx,	SamplerStateBindIndex::ShadowComprisonSamplerState,	SamplerState::ShadowGreaterEqualComp);
		AutoBinderSampler<ComputeShader> pointSampler(dx,		SamplerStateBindIndex::ShadowPointSamplerState,		SamplerState::Point);

		AutoBinderCB<ComputeShader> tbrCB(dx,				ConstBufferBindIndex::TBRParam,				_tbrCB);
		AutoBinderCB<ComputeShader> cameraCB(dx,			ConstBufferBindIndex::Camera,				mainCamera.GetCameraCB());
		AutoBinderCB<ComputeShader> shadowGlobalParam(dx,	ConstBufferBindIndex::ShadowGlobalParam,	shadowMgr.GetGlobalParamCB());

		AutoBinderSRV<ComputeShader> dlDirXY(dx,			TextureBindIndex::DirectionalLightDirXY,				lightMgr.GetBuffer<DirectionalLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlColor(dx,			TextureBindIndex::DirectionalLightColor,				lightMgr.GetBuffer<DirectionalLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlOptionalParam(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr.GetBuffer<DirectionalLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowParam(dx,		TextureBindIndex::DirectionalLightShadowParam,			shadowMgr.GetBuffer<DirectionalLightShadow>().GetBuffer().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowVPMat(dx,		TextureBindIndex::DirectionalLightShadowViewProjMatrix,	shadowMgr.GetBuffer<DirectionalLightShadow>().GetBuffer().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> plTF(dx,				TextureBindIndex::PointLightRadiusWithCenter,			lightMgr.GetBuffer<PointLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plColor(dx,			TextureBindIndex::PointLightColor,						lightMgr.GetBuffer<PointLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plOptionalParam(dx,	TextureBindIndex::PointLightOptionalParamIndex,			lightMgr.GetBuffer<PointLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plShadowParam(dx,		TextureBindIndex::PointLightShadowParam,				shadowMgr.GetBuffer<PointLightShadow>().GetBuffer().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plShadowVPMat(dx,		TextureBindIndex::PointLightShadowViewProjMatrix,		shadowMgr.GetBuffer<PointLightShadow>().GetBuffer().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> slTF(dx,				TextureBindIndex::SpotLightRadiusWithCenter,			lightMgr.GetBuffer<SpotLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slParam(dx,			TextureBindIndex::SpotLightParam,						lightMgr.GetBuffer<SpotLight>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slColor(dx,			TextureBindIndex::SpotLightColor,						lightMgr.GetBuffer<SpotLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slOptionalParam(dx,	TextureBindIndex::SpotLightOptionalParamIndex,			lightMgr.GetBuffer<SpotLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowParam(dx,		TextureBindIndex::SpotLightShadowParam,					shadowMgr.GetBuffer<SpotLightShadow>().GetBuffer().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowVPMat(dx,		TextureBindIndex::SpotLightShadowViewProjMatrix,		shadowMgr.GetBuffer<SpotLightShadow>().GetBuffer().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> gbufferDepth(dx,		TextureBindIndex::GBuffer_Depth,						_gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferBlendDepth(dx,	TextureBindIndex::GBuffer_BlendedDepth,					_gbuffer.blendedDepthBuffer.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferAlbedo(dx,		TextureBindIndex::GBuffer_Albedo_Occlusion,				_gbuffer.albedo_occlusion.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferVelocity(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,_gbuffer.velocity_metallic_specularity.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferNormal(dx,		TextureBindIndex::GBuffer_Normal_Roughness,				_gbuffer.normal_roughness.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferEmission(dx,	TextureBindIndex::GBuffer_Emission_MaterialFlag,		_gbuffer.emission_materialFlag.GetTexture2D().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> plShadowMap(dx,		TextureBindIndex::PointLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<PointLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowMap(dx,		TextureBindIndex::SpotLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<SpotLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowMap(dx,		TextureBindIndex::DirectionalLightShadowMapAtlas,		shadowRenderer.GetShadowAtlasMap<DirectionalLightShadow>().GetTexture2D().GetShaderResourceView());

		AutoBinderUAV outLightBuffer(dx, UAVBindIndex::TBDR_OutLightBuffer, _resultMap.GetTexture2D().GetUnorderedAccessView());
//		AutoBinderUAV outLightIndices(dx, UAVBindIndex::TBDR_OutPerLightIndicesInTile, ?)

		Size<uint> groupSize = CullingUtility::CalcThreadGroupSize(mainCamera.GetRenderRect().size);
		_tbdrShader.SetThreadGroupInfo({groupSize.w, groupSize.h, 1});
		_tbdrShader.Dispatch(dx);

		_blendedDepthLightCulling.Dispatch(dx, {groupSize.w, groupSize.h, 1});
	}

	// 3 - Pass GI
	{

	}

	// 4 Pass Sky
	{

	}

	// 5 Pass Render Transparent Mesh
	{

	}
}
