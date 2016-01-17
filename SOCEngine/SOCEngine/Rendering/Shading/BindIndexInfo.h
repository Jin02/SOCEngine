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

		GBuffer_Albedo_Emission					= 8,
		GBuffer_Specular_Metallic				= 9,
		GBuffer_Normal_Roughness				= 10,
		GBuffer_Depth							= 11,

		DiffuseTex								= 8,
		NormalTex								= 9,
		SpecularTex								= 10,
		OpacityTex								= 11,

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

		AnisotropicVoxelAlbedoTexture			= 0,
		AnisotropicVoxelNormalTexture			= 1,
		AnisotropicVoxelEmissionTexture			= 2,

		DirectionalLightShadowInvVPVMat			= 3,
		PointLightShadowInvVPVMat				= 4,
		SpotLightShadowInvVPVMat				= 5,

		VCT_InputVoxelColorMap					= 29,
		VCT_InputDirectColorMap					= 30,
	};

	enum class ConstBufferBindIndex : unsigned int
	{
		TBRParam						= 0,
		World							= 1,
		Camera							= 2,
		PhysicallyBasedMaterial			= 3,
		ShadowGlobalParam				= 4,

		GlobalIIllumination_InfoCB		= 0,
		Voxelization_InfoCB				= 1,
		Mipmap_InfoCB					= 2,
		VCT_GlobalInfoCB				= 1,
	};

	enum class SamplerStateBindIndex : unsigned int
	{
		DefaultSamplerState				= 0,
		UISamplerState					= 1,
		ShadowComprisonSamplerState		= 2,
		VSMShadowSamplerState			= 3,
	};

	enum class UAVBindIndex : unsigned int
	{
		TBDR_OutScreen					= 0,
		
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