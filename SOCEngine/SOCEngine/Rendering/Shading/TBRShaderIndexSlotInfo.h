#pragma once

namespace Rendering
{
	namespace TBDR
	{
		enum class InputBufferShaderIndex : unsigned int
		{
			PointLightRadiusWithCenter		= 0,
			PointLightColor					= 1,
			SpotLightRadiusWithCenter		= 2,
			SpotLightColor					= 3,
			SpotLightParam					= 4,
			DirectionalLightCenterWithDirZ	= 5,
			DirectionalLightColor			= 6,
			DirectionalLightParam			= 7
		};

		enum class InputTextureShaderIndex : unsigned int
		{
			GBuffer_Albedo_Metallic			= 8,
			GBuffer_Specular_Fresnel0		= 9,
			GBuffer_Normal_Roughness		= 10,
			GBuffer_Depth					= 11,
			GBuffer_BlendedDepth			= 12
		};

		enum class InputConstBufferShaderIndex : unsigned int
		{
			Camera							= 0,
		//	Transform						= 1,
		//	PhysicallyBasedMaterial			= 2,
			TBRParam						= 3
		};
	}
}