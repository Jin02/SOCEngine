//EMPTY_META_DATA

#include "Injection_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, 1)]
void CS(uint3 globalIdx	: SV_DispatchThreadID, 
	uint3 localIdx	: SV_GroupThreadID,
	uint3 groupIdx	: SV_GroupID)
{
	uint capacity		= GetNumOfPointLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	float perShadowMapRes	= (float)(1 << GetNumOfPointLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));

	uint shadowIndex	= globalIdx.x / (uint)perShadowMapRes;
	uint faceIndex		= globalIdx.y / (uint)perShadowMapRes;

	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, g_inputPointLightShadowParams[shadowIndex].xy);
	uint lightIndex		= shadowParam.lightIndex;

	float2 shadowMapPos	= float2(globalIdx.x % uint(perShadowMapRes), globalIdx.y % uint(perShadowMapRes));
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth		= g_inputPointLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);
	float4 worldPos		= mul( float4(shadowMapPos.xy, depth, 1.0f), g_inputPointLightShadowInvVPVMatBuffer[shadowIndex].mat[faceIndex] );
	worldPos /= worldPos.w;

	float voxelizeSize	= GetVoxelizeSize(voxelization_currentCascade);
	float3 voxelSpaceUV	= (worldPos.xyz - voxelization_minPos) / voxelizeSize;
	int dimension		= (int)GetDimension();
	int3 voxelIdx		= int3(voxelSpaceUV * dimension);

	if( any(voxelIdx < 0) || any(dimension <= voxelIdx) )
		return;

	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightIndex];
	float3 lightCenterWorldPos	= lightCenterWithRadius.xyz;
	float lightRadius		= lightCenterWithRadius.a;

	float3 lightDir			= lightCenterWorldPos - worldPos.xyz;
	float distanceOfLightWithVertex	= length(lightDir);
	lightDir			= normalize(lightDir);

	float3 normal			= GetNormal(g_inputVoxelNormalMap, voxelIdx, voxelization_currentCascade);
	float4 albedo			= GetColor(g_inputVoxelAlbedoMap, voxelIdx, voxelization_currentCascade);
	float4 emission			= GetColor(g_inputVoxelEmissionMap, voxelIdx, voxelization_currentCascade);

	float3 radiosity = float3(0.0f, 0.0f, 0.0f);
	if( distanceOfLightWithVertex < lightRadius )
	{
		float4 lightColorWithLm = g_inputPointLightColorBuffer[lightIndex];

		float3 lightColor	= lightColorWithLm.rgb;
		float lumen		= lightColorWithLm.a * float(MAXIMUM_LUMEN);
		float attenuation	= lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);
		float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));

		radiosity = lambert * attenuation * lightColor * RenderPointLightShadow(lightIndex, worldPos.xyz, lightDir, distanceOfLightWithVertex / lightRadius);
	}
	radiosity = saturate(radiosity + emission.rgb);

	StoreRadiosity(OutVoxelColorTexture, radiosity, albedo.a, normal, voxelIdx, voxelization_currentCascade);
}

#endif
