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
		float4 lightCenterWithRadius	= SpotLightTransformBuffer[lightIndex];
		float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
		float radiusWithMinusZDirBit	= lightCenterWithRadius.a;
	
		float4 spotParam	= SpotLightParamBuffer[lightIndex];
		float3 lightDir		= float3(spotParam.x, spotParam.y, 0.0f);
		lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
		lightDir.z = (radiusWithMinusZDirBit >= 0.0f) ? lightDir.z : -lightDir.z;
	
		float radius = abs(radiusWithMinusZDirBit);
	
		float outerCosineConeAngle	= spotParam.z;
		float innerCosineConeAngle	= spotParam.w;
	
		float3 vtxToLight				= lightCenterWorldPos - worldPos.xyz;
		float3 vtxToLightDir			= normalize(vtxToLight);
		float distanceOfLightWithVertex = length(vtxToLight);
		float currentCosineConeAngle	= dot(-vtxToLightDir, lightDir);
	
		float3 normal	= GetNormalInVoxelRawBuf(VoxelNormalRawBuf, globalIdx);
		float4 albedo	= GetColorInVoxelRawBuf(VoxelAlbedoRawBuf, globalIdx);
		float4 emission	= GetColorInVoxelRawBuf(VoxelEmissionRawBuf, globalIdx);
	
		float3 radiosity	= float3(0.0f, 0.0f, 0.0f);
//		float4 shadowColor	= float4(1.0f, 1.0f, 1.0f, 1.0f);
		if( (distanceOfLightWithVertex < (radius * 1.5f)) &&
			(outerCosineConeAngle < currentCosineConeAngle) )
		{
			float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
			innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
			innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
			innerOuterAttenuation = (innerCosineConeAngle < currentCosineConeAngle) ? 1 : innerOuterAttenuation;
	
			float4 lightColorWithLm = PointLightColorBuffer[lightIndex];
			float lumen = lightColorWithLm.w * float(MAXIMUM_LUMEN); //maximum lumen is float(MAXIMUM_LUMEN)
	
			float plAttenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex);
			float totalAttenTerm = lumen * plAttenuation * innerOuterAttenuation;
	
			float3 lightColor = lightColorWithLm.rgb;
			float3 lambert = (albedo.rgb / PI) * saturate(dot(normal, lightDir));
	
//			shadowColor = RenderSpotLightShadow(lightIndex, worldPos.xyz, 0.0f);
			radiosity = lambert * totalAttenTerm * lightColor;// * shadowColor.rgb;
		}
		radiosity += emission.rgb;
	
		StoreRadiosity(OutVoxelColorMap, radiosity, albedo.a, normal, globalIdx, true);
	}
}