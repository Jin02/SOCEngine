#pragma once

namespace Rendering
{
	namespace TBDR
	{
		enum class InputSRBufferBindSlotIndex : unsigned int
		{
			PointLightRadiusWithCenter		= 0,
			PointLightColor					= 1,
			SpotLightRadiusWithCenter		= 2,
			SpotLightColor					= 3,
			SpotLightParam					= 4,
			DirectionalLightCenterWithDirZ	= 5,
			DirectionalLightColor			= 6,
			DirectionalLightParam			= 7,
			GBuffer_Albedo_Emission			= 8,
			GBuffer_Specular_Metallic		= 9,
			GBuffer_Normal_Roughness		= 10,
			GBuffer_Depth					= 11,
			GBuffer_BlendedDepth			= 12,
			LightIndexBuffer				= 13,
			PointLightShadowParam			= 14,
			SpotLightShadowParam			= 15,
			DirectionalLightShadowParam		= 16,
			PointLightShadowMapAtlas		= 17,
			SpotLightShadowMapAtlas			= 18,
			DirectionalLightShadowMapAtlas	= 19,
			PointLightShadowColor			= 20,
			SpotLightShadowColor			= 21,
			DirectionalLightShadowColor		= 22,
		};

		enum class InputConstBufferBindSlotIndex : unsigned int
		{
			TBRParam						= 0,
		//	World							= 1, -> PhysicallyBasedMaterial
		//	Camera							= 2, -> PhysicallyBasedMaterial
		//	PhysicallyBasedMaterial			= 3, -> PhysicallyBasedMaterial
			ShadowGlobalParam				= 4
		};

		enum class InputSamplerStateBindSlotIndex : unsigned int
		{
			DefaultSamplerState				= 0,
			UISamplerState					= 1,
			ShadowComprisonSamplerState		= 2,
		};
	}
}