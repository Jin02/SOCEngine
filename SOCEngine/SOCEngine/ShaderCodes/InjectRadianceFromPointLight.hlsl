//EMPTY_META_DATA

#include "Injection_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, INJECTION_TILE_RES)]
void CS(uint3 globalIdx	: SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	int dimension		= (int)GetDimension();
	uint cascade		= globalIdx.y / dimension;
	uint3 voxelIndex	= uint3(globalIdx.x, globalIdx.y % dimension, globalIdx.z);

	float3 bbMin		= float3(0.0f, 0.0f, 0.0f);
	{
		float3 bbMax	= float3(0.0f, 0.0f, 0.0f);
		ComputeVoxelizationBound(bbMin, bbMax, cascade, gi_startCenterWorldPos);
	}
	float voxelSize		= ComputeVoxelSize(cascade);
	float3 worldPos		= GetVoxelCenterPos(voxelIndex, bbMin, voxelSize);

	uint lightCount  = GetNumOfPointLight(gi_packedNumOfLights);
	for(uint lightIndex	= 0; lightIndex < lightCount; ++lightIndex)
	{
		float4 lightCenterWithRadius	= PointLightTransformBuffer[lightIndex];
		float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
		float lightRadius				= lightCenterWithRadius.a;
	
		float3 lightDir					= lightCenterWorldPos - worldPos.xyz;
		float distanceOfLightWithVertex	= length(lightDir);
		lightDir = normalize(lightDir);
	
		float3 normal	= GetNormal(VoxelNormalMap, voxelIndex, voxelization_currentCascade);
		float4 albedo	= GetColor(VoxelAlbedoMap, voxelIndex, voxelization_currentCascade);
		float4 emission	= GetColor(VoxelEmissionMap, voxelIndex, voxelization_currentCascade);
	
		float3 radiosity	= float3(0.0f, 0.0f, 0.0f);
		if( distanceOfLightWithVertex < lightRadius )
		{
			float4 lightColorWithLm = PointLightColorBuffer[lightIndex];
	
			float3 lightColor	= lightColorWithLm.rgb;
			float lumen			= lightColorWithLm.a * float(MAXIMUM_LUMEN);
			float attenuation	= lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);
			float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
	
			radiosity	= lambert * attenuation * lightColor * RenderPointLightShadow(lightIndex, worldPos.xyz, lightDir, 0.0f);
		}
		radiosity = saturate(radiosity + emission.rgb);
	
		StoreRadiosityUsingRWTexture3D(OutVoxelColorMap, radiosity, 1.0f, normal, voxelIndex, voxelization_currentCascade);
	}
}

#endif