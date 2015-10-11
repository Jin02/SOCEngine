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

		enum class InputShaderResourceBufferIndex : unsigned int
		{
			GBuffer_Albedo_Emission			= 8,
			GBuffer_Specular_Metallic		= 9,
			GBuffer_Normal_Roughness		= 10,
			GBuffer_Depth					= 11,
			GBuffer_BlendedDepth			= 12,
			LightIndexBuffer				= 13,
		};

		enum class InputConstBufferShaderIndex : unsigned int
		{
			TBRParam						= 0
		//	Transform						= 1,
		//	PhysicallyBasedMaterial			= 2,
		};
	}
}