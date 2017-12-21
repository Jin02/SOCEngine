#pragma once

#ifdef SOCENGINE
#include "../GlobalDefine.h"
#else
#include "GlobalDefine.h"
#endif

namespace Rendering
{
	enum class TextureBindIndex : unsigned int
	{
		SkyBoxCubeMap							= 0,

		PointLightRadiusWithCenter				= 0,
		PointLightColor							= 1,
		SpotLightRadiusWithCenter				= 2,
		SpotLightColor							= 3,
		SpotLightParam							= 4,
		DirectionalLightDirXY					= 5,
		DirectionalLightColor					= 6,

		GBuffer_Albedo_Occlusion				= 8,
		GBuffer_Velocity_Metallic_Specularity	= 9,
		GBuffer_Normal_Roughness				= 10,
		GBuffer_Depth							= 11,
		GBuffer_Emission_MaterialFlag			= 31,

		DiffuseMap								= 8,
		NormalMap								= 9,
		OpacityMap								= 10,
		HeightMap								= 11,
		MetallicMap								= 12,
		OcclusionMap							= 13,
		RoughnessMap							= 33,
		EmissionMap								= 34,

		GBuffer_BlendedDepth					= 12,
		LightIndexBuffer						= 27,

		PointLightShadowParam					= 14,
		SpotLightShadowParam					= 15,
		DirectionalLightShadowParam				= 16,

		PointLightShadowMapAtlas				= 17,
		SpotLightShadowMapAtlas					= 18,
		DirectionalLightShadowMapAtlas			= 19,

		PointLightOptionalParamIndex			= 20,
		SpotLightOptionalParamIndex				= 21,
		DirectionalLightOptionalParamIndex		= 22,

		PointLightShadowViewProjMatrix			= 23,
		SpotLightShadowViewProjMatrix			= 24,
		DirectionalLightShadowViewProjMatrix	= 25,

		VoxelAlbedoRawBuffer					= 29,
		VoxelNormalRawBuffer					= 30,
		VoxelEmissionRawBuffer					= 31,

		VCTInjectionSourceColorMap				= 29,
		VCTMipmappedInjectionColorMap			= 30,

		IBLPass_PreIntegrateEnvBRDFMap			= 29,
		IBLPass_IlluminationMap					= 30,
		ReflectionProbe_PreIntegrateEnvBRDFMap	= 29,
		AmbientCubeMap							= 32,

		OffScreen_DirectDiffuseLightBuffer		= 0,
		OffScreen_DirectSpecularLightBuffer		= 1,
		OffScreen_InDirectLightBuffer			= 2,
	};

	enum class ConstBufferBindIndex : unsigned int
	{
		TBRParam						= 0,
		Transform						= 1,
		Camera							= 2,
		PhysicallyBasedMaterial			= 3,
		ShadowGlobalParam				= 4,
		OnlyPassViewProjMat				= 4,

		ReflectionProbe_Info			= 5,	

		VoxelizationInfoCB				= 5,
		VXGIStaticInfoCB				= 6,
		VXGIDynamicInfoCB				= 7,

		SkyMapInfoParam					= 7,

		Mipmap_InfoCB					= 0,
		VCT_GlobalInfoCB				= 1,
		HDRGlobalParamCB				= 1,

		SkyBoxWVP						= 1,
	};

	enum class SamplerStateBindIndex : unsigned int
	{
		DefaultSamplerState				= 0,
		UISamplerState					= 1,
		ShadowComprisonSamplerState		= 2,
		VSMShadowSamplerState			= 3,
		ShadowPointSamplerState			= 4,
		AmbientCubeMapSamplerState		= 5,
	};

	enum class UAVBindIndex : unsigned int
	{
		TBDR_OutLightBuffer		= 0,
		TBDR_OutPerLightIndicesInTile	= 1,
		
		Lightculling_LightIndexBuffer	= 0,

		OutVoxelColorMap				= 0,

		VoxelMap_Albedo					= 0,
		VoxelMap_Normal					= 1,
		VoxelMap_Emission				= 2,

		VoxelMipmap_InputVoxelMap		= 0,
		VoxelMipmap_OutputVoxelMap		= 1,

		VCTOutIndirectMap				= 0,
	};
}