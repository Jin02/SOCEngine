#pragma once

namespace Rendering
{
	enum class TextureBindIndex : unsigned int
	{
		PointLightRadiusWithCenter				= 0,
		PointLightColor							= 1,
		SpotLightRadiusWithCenter				= 2,
		SpotLightColor							= 3,
		SpotLightParam							= 4,
		DirectionalLightCenterWithDirZ			= 5,
		DirectionalLightColor					= 6,
		DirectionalLightParam					= 7,

		GBuffer_Albedo_Occlusion				= 8,
		GBuffer_MotionXY_Metallic_Specularity	= 9,
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
		LightIndexBuffer						= 13,

		PointLightShadowParam					= 14,
		SpotLightShadowParam					= 15,
		DirectionalLightShadowParam				= 16,

		PointLightShadowMapAtlas				= 17,
		SpotLightShadowMapAtlas					= 18,
		DirectionalLightShadowMapAtlas			= 19,

		PointLightShadowIndex					= 20,
		SpotLightShadowIndex					= 21,
		DirectionalLightShadowIndex				= 22,

		PointLightShadowViewProjMatrix			= 23,
		SpotLightShadowViewProjMatrix			= 24,
		DirectionalLightShadowViewProjMatrix	= 25,

		PointLightMomentShadowMapAtlas			= 26,
		SpotLightMomentShadowMapAtlas			= 27,
		DirectionalLightMomentShadowMapAtlas	= 28,

		AnisotropicVoxelAlbedoTexture			= 29,
		AnisotropicVoxelNormalTexture			= 30,
		AnisotropicVoxelEmissionTexture			= 31,

		DirectionalLightShadowInvVPVMat			= 32,
		PointLightShadowInvVPVMat				= 33,
		SpotLightShadowInvVPVMat				= 34,

		VCT_InputVoxelMap						= 29,
		VCT_InputDiffuseLightMap				= 30,
		VCT_InputSpecularLightMap				= 31,	// 31�� GBuffer_Emission_MaterialFlag�� ��ħ

//		GBuffer_Emission_MaterialFlag			31, ���� ����
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
		World							= 1,
		Camera							= 2,
		PhysicallyBasedMaterial			= 3,
		ShadowGlobalParam				= 4,

		ReflectionProbe_Info			= 5,	

		Voxelization_InfoCB				= 5,
		GlobalIIllumination_InfoCB		= 6,

		SkyMapInfoParam					= 7,

		Mipmap_InfoCB					= 0,
		VCT_GlobalInfoCB				= 1,

		SkyBoxWVP						= 0,
	};

	enum class SamplerStateBindIndex : unsigned int
	{
		DefaultSamplerState				= 0,
		UISamplerState					= 1,
		ShadowComprisonSamplerState		= 2,
		VSMShadowSamplerState			= 3,
		AmbientCubeMapSamplerState		= 4,
	};

	enum class UAVBindIndex : unsigned int
	{
		TBDR_OutDiffuseLightBuffer		= 0,
		TBDR_OutSpecularLightBuffer		= 1,
		
		Lightculling_LightIndexBuffer	= 0,

		OutAnisotropicVoxelColorTexture	= 0,

		VoxelMap_Albedo					= 0,
		VoxelMap_Normal					= 1,
		VoxelMap_Emission				= 2,

		VoxelMipmap_InputVoxelMap		= 0,
		VoxelMipmap_OutputVoxelMap		= 1,

		VCT_OutIndirectMap				= 0,
	};
}