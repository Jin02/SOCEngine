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
using namespace Rendering::GI;
using namespace Rendering::Factory;

void MainRenderer::Initialize(DirectX& dx, ShaderManager& shaderMgr, BufferManager& bufferMgr, const MainCamera& mainCamera, const GIInitParam& giParam)
{
	_blendedDepthLightCulling.Initialize(dx, shaderMgr, mainCamera.GetRenderRect().size);

	const auto& renderSize = mainCamera.GetRenderRect().size;

	// Result Map
	{
		auto size = renderSize;
		{
			if (dx.GetMSAADesc().Count > 1)
			{
				size.w *= 2;
				size.h *= 2;
			}
		}

		_scaledMap.Initialize(dx, size.Cast<uint>(), DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 0);

		float maxLength	= static_cast<float>( std::max(renderSize.w, renderSize.h) );
		uint mipLevel	= static_cast<uint>( log(maxLength) / log(2.0f) ) + 1;
		_resultMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, 0, 1, mipLevel);
		_transparentMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_skyBoxMap.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
	}

	// Gbuffer
	{
		_gbuffer.albedo_occlusion.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.normal_roughness.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);
		_gbuffer.velocity_metallic_specularity.Initialize(dx, renderSize, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_R16G16B16A16_FLOAT, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.emission_materialFlag.Initialize(dx, renderSize, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_UNKNOWN, 0);

		_gbuffer.opaqueDepthBuffer.Initialize(dx, renderSize, true);
		_gbuffer.blendedDepthBuffer.Initialize(dx, renderSize, true);
	}

	_tbrCB.Initialize(dx);

	// Load Shader
	{
		std::vector<Shader::ShaderMacro> macros{	dx.GetMSAAShaderMacro(),
													ShaderMacro("USE_COMPUTE_SHADER")};

		_tbdrShader = *ShaderFactory::LoadComputeShader(dx, shaderMgr,"TBDR", "TileBasedDeferredShadingCS", &macros, "@TBDR");

		Size<uint> size = Light::CullingUtility::ComputeThreadGroupSize(renderSize);
		_tbdrThreadGroup = ComputeShader::ThreadGroup(size.w, size.h, 1);
	}

	_skyBoxRenderer.Initialize(dx, bufferMgr);

	if (_useGI = giParam.GetUseGI())
		_gi.Initialize(dx, shaderMgr, renderSize, giParam);

	_mainSceneMaker.Initialize(dx, shaderMgr, renderSize);
	_envBRDFMap.CreatePreBRDFMap(dx, shaderMgr);
}

void MainRenderer::UpdateCB(DirectX& dx, const MainCamera& mainCamera, const LightManager& lightMgr)
{
	if( (_dirty == false) & (mainCamera.GetDirty() == false) & (lightMgr.GetHasChangedLightCount() == false) & (mainCamera.GetHasChangedCB() == false) )
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

		packed.packedViewportSize		= (renderRect.size.w << 16) | renderRect.size.h;
	}

	_tbrCB.UpdateSubResource(dx, tbrCBData);
	_dirty = false;

	const Matrix& worldMat = mainCamera.GetWorldMatrix();
	_skyBoxRenderer.UpdateCB(dx, Vector3(worldMat._41, worldMat._42, worldMat._43), mainCamera.GetViewProjMatrix(), mainCamera.GetFar());
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


void MainRenderer::Render(DirectX& dx, const Param&& param)
{
	const auto& mainCamera = param.mainCamera;
	const auto& materialMgr = param.materialMgr;
	const auto& renderParam	= param.renderParam;

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

		_resultMap.Clear(dx, mainCamera.GetClearColor());
		_scaledMap.Clear(dx, mainCamera.GetClearColor());
	}

	// 1 Pass - Render GBuffer
	{
		RenderTexture* renderTargets[] =
		{
			&_gbuffer.albedo_occlusion,
			&_gbuffer.velocity_metallic_specularity,
			&_gbuffer.normal_roughness,
			&_gbuffer.emission_materialFlag
		};

		dx.SetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, _gbuffer.opaqueDepthBuffer);
		dx.SetDepthStencilState(DepthState::Greater, 0);

		AutoBinderSampler<PixelShader>	sampler(dx,		SamplerStateBindIndex::DefaultSamplerState,	SamplerState::Anisotropic);
		AutoBinderCB<VertexShader>		cameraCB(dx,	ConstBufferBindIndex::Camera,				mainCamera.GetCameraCB());

		{
			AutoBinderCB<PixelShader> tbrParamCB(dx,	ConstBufferBindIndex::TBRParam,				_tbrCB);
			// Opaque
			{
				dx.SetRasterizerState(RasterizerState::CWDefault);
				dx.SetBlendState(BlendState::Opaque);
	
				MeshRenderer::RenderOpaqueMeshes(dx, renderParam, DefaultRenderType::GBuffer_Opaque, mainCamera.GetOpaqueMeshRenderQ(),
					[&dx, &materialMgr](const Mesh* mesh) // Pre Render
					{
						const PhysicallyBasedMaterial* material = materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID());
						if(material)
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
				// GBuffer는 그대로 Output으로 사용한다.
				// dx.SetRenderTargets(ARRAYSIZE(renderTargets), renderTargets, _gbuffer.opaqueDepthBuffer);
	
				if(dx.GetMSAADesc().Count > 1)	// on msaa
					dx.SetBlendState(BlendState::AlphaToCoverage);
				// msaa가 아니라면 BlendState::Opaque를 그대로 쓴다.
	
				dx.SetRasterizerState(RasterizerState::CWDisableCulling);
	
				MeshRenderer::RenderAlphaTestMeshes(dx, renderParam, DefaultRenderType::GBuffer_AlphaTest, mainCamera.GetAlphaTestMeshRenderQ(),
					[&dx, &materialMgr](const Mesh* mesh)	// Pre-Render
					{
						const PhysicallyBasedMaterial* material = materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID());					
						if(material)
							BindGBufferResourceTextures(dx, material);
					},
					[&dx](/*Post Render*/) { UnBindGBufferResourceTextures(dx); }
				);	
			}
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
	const auto& shadowParam		= param.shadowParam;
	const auto& shadowMgr		= shadowParam.manager;
	const auto& shadowRenderer	= shadowParam.renderer;

	// 2 Pass - Light Culling and Deferred Shading
	{
		AutoBinderSampler<ComputeShader> shadowCompGreaterS(dx,	SamplerStateBindIndex::ShadowComprisonGreaterSamplerState,	SamplerState::ShadowGreaterEqualComp);
		AutoBinderSampler<ComputeShader> pointS(dx,				SamplerStateBindIndex::ShadowPointSamplerState,				SamplerState::Point);
		AutoBinderSampler<ComputeShader> ambientCubeS(dx,		SamplerStateBindIndex::AmbientCubeMapSamplerState,			SamplerState::Linear);
		AutoBinderSampler<ComputeShader> shadowCompLessS(dx,	SamplerStateBindIndex::ShadowComprisonLessSamplerState,		SamplerState::ShadowLessEqualComp);

		AutoBinderCB<ComputeShader> tbrCB(dx,				ConstBufferBindIndex::TBRParam,				_tbrCB);
		AutoBinderCB<ComputeShader> cameraCB(dx,			ConstBufferBindIndex::Camera,				mainCamera.GetCameraCB());
		AutoBinderCB<ComputeShader> shadowGlobalParam(dx,	ConstBufferBindIndex::ShadowGlobalParam,	shadowParam.manager.GetGlobalCB());

		AutoBinderSRV<ComputeShader> dlDirXY(dx,			TextureBindIndex::DirectionalLightDirXY,				lightMgr.GetBuffer<DirectionalLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlColor(dx,			TextureBindIndex::DirectionalLightColor,				lightMgr.GetBuffer<DirectionalLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlOptionalParam(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr.GetBuffer<DirectionalLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowParam(dx,		TextureBindIndex::DirectionalLightShadowParam,			shadowMgr.GetBuffer<DirectionalLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowVPMat(dx,		TextureBindIndex::DirectionalLightShadowViewProjMatrix,	shadowMgr.GetBuffer<DirectionalLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());
		
		AutoBinderSRV<ComputeShader> plTF(dx,				TextureBindIndex::PointLightRadiusWithCenter,			lightMgr.GetBuffer<PointLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plColor(dx,			TextureBindIndex::PointLightColor,						lightMgr.GetBuffer<PointLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plOptionalParam(dx,	TextureBindIndex::PointLightOptionalParamIndex,			lightMgr.GetBuffer<PointLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plShadowParam(dx,		TextureBindIndex::PointLightShadowParam,				shadowMgr.GetBuffer<PointLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> plShadowVPMat(dx,		TextureBindIndex::PointLightShadowViewProjMatrix,		shadowMgr.GetBuffer<PointLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> slTF(dx,				TextureBindIndex::SpotLightRadiusWithCenter,			lightMgr.GetBuffer<SpotLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slParam(dx,			TextureBindIndex::SpotLightParam,						lightMgr.GetBuffer<SpotLight>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slColor(dx,			TextureBindIndex::SpotLightColor,						lightMgr.GetBuffer<SpotLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slOptionalParam(dx,	TextureBindIndex::SpotLightOptionalParamIndex,			lightMgr.GetBuffer<SpotLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowParam(dx,		TextureBindIndex::SpotLightShadowParam,					shadowMgr.GetBuffer<SpotLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowVPMat(dx,		TextureBindIndex::SpotLightShadowViewProjMatrix,		shadowMgr.GetBuffer<SpotLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<ComputeShader> gbufferDepth(dx,		TextureBindIndex::GBuffer_Depth,						_gbuffer.opaqueDepthBuffer.GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferAlbedo(dx,		TextureBindIndex::GBuffer_Albedo_Occlusion,				_gbuffer.albedo_occlusion.GetTexture2D()->GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferVelocity(dx,	TextureBindIndex::GBuffer_Velocity_Metallic_Specularity,_gbuffer.velocity_metallic_specularity.GetTexture2D()->GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferNormal(dx,		TextureBindIndex::GBuffer_Normal_Roughness,				_gbuffer.normal_roughness.GetTexture2D()->GetShaderResourceView());
		AutoBinderSRV<ComputeShader> gbufferEmission(dx,	TextureBindIndex::GBuffer_Emission_MaterialFlag,		_gbuffer.emission_materialFlag.GetTexture2D()->GetShaderResourceView());

		AutoBinderSRV<ComputeShader> plShadowMap(dx,		TextureBindIndex::PointLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<PointLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> slShadowMap(dx,		TextureBindIndex::SpotLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<SpotLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowMap(dx,		TextureBindIndex::DirectionalLightShadowMapAtlas,		shadowRenderer.GetShadowAtlasMap<DirectionalLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<ComputeShader> dlShadowViewDepth(dx,	TextureBindIndex::DirectionalLightShadowViewDepthMap,	shadowRenderer.GetShadowAtlasMap<DirectionalLightShadow>().GetViewDepthMap()->GetTexture2D()->GetShaderResourceView());

		AutoBinderSRV<ComputeShader> envLUT(dx,				TextureBindIndex::IBLPass_PreIntegrateEnvBRDFMap,		_envBRDFMap.GetTexture2D().GetShaderResourceView());

		if(param.skyBoxMaterial)
			ComputeShader::BindShaderResourceView(dx,			TextureBindIndex::AmbientCubeMap,						param.skyBoxMaterial->GetCubeMap().GetShaderResourceView());

		AutoBinderUAV outLightBuffer(dx, UAVBindIndex::TBDR_OutLightBuffer, _scaledMap.GetTexture2D()->GetUnorderedAccessView());
		_tbdrShader.Dispatch(dx, _tbdrThreadGroup);

		ComputeShader::UnBindShaderResourceView(dx,			TextureBindIndex::AmbientCubeMap);

		AutoBinderSRV<ComputeShader> gbufferBlendDepth(dx,	TextureBindIndex::GBuffer_BlendedDepth,					_gbuffer.blendedDepthBuffer.GetTexture2D().GetShaderResourceView());
		_blendedDepthLightCulling.Dispatch(dx, _tbdrThreadGroup);
	}

	// 2 - Sky
	if(param.skyBoxMaterial)
	{
		_skyBoxMap.Clear(dx, Color::Clear());
		_skyBoxRenderer.Render(dx, _skyBoxMap, _gbuffer.opaqueDepthBuffer, *param.skyBoxMaterial, _tbrCB);
	}

	// 3 - Render Transparent Mesh
	{
		_transparentMap.Clear(dx, Color::Clear());

		dx.SetRenderTarget(_transparentMap, _gbuffer.opaqueDepthBuffer);
		dx.SetDepthStencilState(DepthState::GreaterAndDisableDepthWrite, 0);
		dx.SetViewport(mainCamera.GetRenderRect().Cast<float>());
		dx.SetRasterizerState(RasterizerState::CWDisableCulling);
		dx.SetBlendState(BlendState::Alpha);
		dx.SetPrimitiveTopology(PrimitiveTopology::TriangleList);
		
		AutoBinderCB<VertexShader> vsCamCB(dx,				ConstBufferBindIndex::Camera,				mainCamera.GetCameraCB());

		AutoBinderCB<PixelShader> psCamCB(dx,				ConstBufferBindIndex::Camera,				mainCamera.GetCameraCB());
		AutoBinderCB<PixelShader> psTBRCB(dx,				ConstBufferBindIndex::TBRParam,				_tbrCB);
		AutoBinderCB<PixelShader> psShadowGlobalParam(dx,	ConstBufferBindIndex::ShadowGlobalParam,	shadowMgr.GetGlobalCB());

		AutoBinderSampler<PixelShader> defaultS(dx,			SamplerStateBindIndex::DefaultSamplerState,					SamplerState::Anisotropic);
		AutoBinderSampler<PixelShader> shadowGreaterS(dx,	SamplerStateBindIndex::ShadowComprisonGreaterSamplerState,	SamplerState::ShadowGreaterEqualComp);
		AutoBinderSampler<PixelShader> pointS(dx,			SamplerStateBindIndex::ShadowPointSamplerState,				SamplerState::Point);
		AutoBinderSampler<PixelShader> shadowLessrS(dx,		SamplerStateBindIndex::ShadowComprisonLessSamplerState,		SamplerState::ShadowLessEqualComp);

		AutoBinderSRV<PixelShader> dlDirXY(dx,			TextureBindIndex::DirectionalLightDirXY,				lightMgr.GetBuffer<DirectionalLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> dlColor(dx,			TextureBindIndex::DirectionalLightColor,				lightMgr.GetBuffer<DirectionalLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> dlOptionalParam(dx,	TextureBindIndex::DirectionalLightOptionalParamIndex,	lightMgr.GetBuffer<DirectionalLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> dlShadowParam(dx,	TextureBindIndex::DirectionalLightShadowParam,			shadowMgr.GetBuffer<DirectionalLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> dlShadowVPMat(dx,	TextureBindIndex::DirectionalLightShadowViewProjMatrix,	shadowMgr.GetBuffer<DirectionalLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<PixelShader> plTF(dx,				TextureBindIndex::PointLightRadiusWithCenter,			lightMgr.GetBuffer<PointLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> plColor(dx,			TextureBindIndex::PointLightColor,						lightMgr.GetBuffer<PointLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> plOptionalParam(dx,	TextureBindIndex::PointLightOptionalParamIndex,			lightMgr.GetBuffer<PointLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> plShadowParam(dx,	TextureBindIndex::PointLightShadowParam,				shadowMgr.GetBuffer<PointLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> plShadowVPMat(dx,	TextureBindIndex::PointLightShadowViewProjMatrix,		shadowMgr.GetBuffer<PointLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<PixelShader> slTF(dx,				TextureBindIndex::SpotLightRadiusWithCenter,			lightMgr.GetBuffer<SpotLight>().GetTransformSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slParam(dx,			TextureBindIndex::SpotLightParam,						lightMgr.GetBuffer<SpotLight>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slColor(dx,			TextureBindIndex::SpotLightColor,						lightMgr.GetBuffer<SpotLight>().GetColorSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slOptionalParam(dx,	TextureBindIndex::SpotLightOptionalParamIndex,			lightMgr.GetBuffer<SpotLight>().GetOptionalParamIndexSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slShadowParam(dx,	TextureBindIndex::SpotLightShadowParam,					shadowMgr.GetBuffer<SpotLightShadow>().GetParamSRBuffer().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slShadowVPMat(dx,	TextureBindIndex::SpotLightShadowViewProjMatrix,		shadowMgr.GetBuffer<SpotLightShadow>().GetViewProjMatSRBuffer().GetShaderResourceView());

		AutoBinderSRV<PixelShader> plShadowMap(dx, 		TextureBindIndex::PointLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<PointLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<PixelShader> slShadowMap(dx, 		TextureBindIndex::SpotLightShadowMapAtlas,				shadowRenderer.GetShadowAtlasMap<SpotLightShadow>().GetTexture2D().GetShaderResourceView());
		AutoBinderSRV<PixelShader> dlShadowMap(dx, 		TextureBindIndex::DirectionalLightShadowMapAtlas,		shadowRenderer.GetShadowAtlasMap<DirectionalLightShadow>().GetTexture2D().GetShaderResourceView());

		AutoBinderSRV<PixelShader> lightIndexBuf(dx, 	TextureBindIndex::LightIndexBuffer,						_blendedDepthLightCulling.GetLightIndexSRBuffer().GetShaderResourceView());

		MeshRenderer::RenderTransparentMeshes(dx, param.renderParam, DefaultRenderType::Forward_Transparency,
			mainCamera.GetTransparentMeshRenderQ(),
			[&dx, &materialMgr](const Mesh* mesh)
			{
				const PhysicallyBasedMaterial* material = materialMgr.Find<PhysicallyBasedMaterial>(mesh->GetPBRMaterialID());
				BindGBufferResourceTextures(dx, material);
			},
			[&dx]()
			{
				UnBindGBufferResourceTextures(dx);
			}
		);

		dx.ReSetRenderTargets(1);
	}

	// 4 - GI
	if (_useGI)
	{
		_gi.Run(dx, VXGI::Param{MainRenderingSystemParam{*this, mainCamera}, lightMgr, param.shadowParam, std::move(param.cullingParam), param.renderParam, param.materialMgr});
	}

	// 5 - Build Main RT
	{
		_mainSceneMaker.Render(dx, _resultMap,	{	
													*_scaledMap.GetTexture2D(),
													_gi.GetVXGIResultMap(),
													*_transparentMap.GetTexture2D(),
													*_skyBoxMap.GetTexture2D()
												}	);
	}
}
