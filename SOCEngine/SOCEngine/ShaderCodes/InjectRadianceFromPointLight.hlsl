//EMPTY_META_DATA

#include "Injection_Common.h"

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, INJECTION_TILE_RES)]
void CS(uint3 globalIdx	: SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	float3 bbMin		= float3(0.0f, 0.0f, 0.0f);
	{
		float3 bbMax	= float3(0.0f, 0.0f, 0.0f);
		ComputeVoxelizationBound(bbMin, bbMax, gi_startCenterWorldPos);
	}

	float3 worldPos	= GetVoxelCenterPos(globalIdx, bbMin, gi_voxelSize);
	uint lightCount	= GetNumOfPointLight(gi_packedNumfOfLights);
	for(uint lightIndex	= 0; lightIndex < lightCount; ++lightIndex)
	{
		float4 lightCenterWithRadius	= PointLightTransformBuffer[lightIndex];
		float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
		float lightRadius				= lightCenterWithRadius.a;
	
		float3 lightDir					= lightCenterWorldPos - worldPos.xyz;
		float distanceOfLightWithVertex	= length(lightDir);
		lightDir = normalize(lightDir);
	
		float3 normal	= GetNormalInVoxelRawBuf(VoxelNormalRawBuf, globalIdx);
		float4 albedo	= GetColorInVoxelRawBuf(VoxelAlbedoRawBuf, globalIdx);
		float4 emission	= GetColorInVoxelRawBuf(VoxelEmissionRawBuf, globalIdx);
	
		float3 radiosity	= float3(0.0f, 0.0f, 0.0f);
		float4 shadowColor	= float4(1.0f, 1.0f, 1.0f, 1.0f);
		if( distanceOfLightWithVertex < lightRadius )
		{
			float4 lightColorWithLm = PointLightColorBuffer[lightIndex];
	
			float3 lightColor	= lightColorWithLm.rgb;
			float lumen			= lightColorWithLm.a * float(MAXIMUM_LUMEN);
			float attenuation	= lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);
			float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
	
			shadowColor = RenderPointLightShadow(lightIndex, worldPos.xyz, lightDir, 0.0f);
			radiosity = lambert * attenuation * lightColor * shadowColor.rgb;
		}
		radiosity = saturate(radiosity + emission.rgb);

		StoreRadiosity(OutVoxelColorMap, radiosity, shadowColor.a, normal, globalIdx);
	}
}