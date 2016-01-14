//EMPTY_META_DATA

#include "Injection_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, 1)]
void InjectRadiancePointLightsCS(uint3 globalIdx	: SV_DispatchThreadID, 
								 uint3 localIdx		: SV_GroupThreadID,
								 uint3 groupIdx		: SV_GroupID)
{
	uint capacity			= GetNumOfPointLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	float perShadowMapRes	= (float)(1 << GetNumOfPointLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));

	uint shadowIndex	= globalIdx.x / (uint)perShadowMapRes;
	uint faceIndex		= globalIdx.y / (uint)perShadowMapRes;
	uint lightIndex		= g_inputPointLightShadowIndexToLightIndex[shadowIndex];

	uint bufferShadowIdx = (g_inputPointLightShadowParams[lightIndex].indexWithBias >> 16);
	if( (bufferShadowIdx - 1) != shadowIndex )
		return;

	float2 shadowMapPos	= float2(globalIdx.x % uint(perShadowMapRes), globalIdx.y % uint(perShadowMapRes));
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth = g_inputPointLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);

	float4 worldPos = mul( float4(shadowMapPos.xy, depth, 1.0f), g_inputPointLightShadowInvVPVMatBuffer[shadowIndex].invMat[faceIndex] );
	worldPos /= worldPos.w;

	float3 voxelSpaceUV = (worldPos.xyz - voxelization_minPos) / voxelization_voxelizeSize;
	int dimension		= (int)GetDimension();
	int3 voxelIdx		= int3(voxelSpaceUV * dimension);

	if( any(voxelIdx < 0) || any(dimension <= voxelIdx) )
		return;

	//float4 screenSpaceCoord = float4( shadowMapUV.x * 2.0f - 1.0f,
	//								-(shadowMapUV.y * 2.0f - 1.0f),
	//								depth, 1.0f );

	//float4 voxelSpaceUV = mul(screenSpaceCoord, injection_volumeProj);
	//voxelSpaceUV /= voxelSpaceUV.w;
	//voxelSpaceUV.xyz = voxelSpaceUV.xyz * 0.5f + 0.5f;
	//uint3 voxelIdx = uint3(voxelSpaceUV.xyz * dimension);

	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightIndex];
	float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
	float lightRadius				= lightCenterWithRadius.a;

	float3 lightDir					= lightCenterWorldPos - worldPos.xyz;
	float distanceOfLightWithVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float4 albedo	= GetColor(g_inputAnistropicVoxelAlbedoTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float3 normal	= GetNormal(g_inputAnistropicVoxelNormalTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float4 emission	= GetColor(g_inputAnistropicVoxelEmissionTexture, voxelIdx, lightDir, voxelization_currentCascade);

	float3 radiosity = float3(0.0f, 0.0f, 0.0f);
	if( distanceOfLightWithVertex < lightRadius )
	{
		float4 lightColorWithLm = g_inputPointLightColorBuffer[lightIndex];

		float3 lightColor	= lightColorWithLm.rgb;
		float lumen			= lightColorWithLm.a * 12750.0f; //maximum lumen is 12,750f
		float attenuation	= lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);
		float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));

		radiosity = lambert * attenuation * lightColor * RenderPointLightShadow(lightIndex, worldPos.xyz, lightDir, distanceOfLightWithVertex / lightRadius);
	}
	radiosity += emission.rgb;

	StoreRadiosity(radiosity, albedo.a, normal, voxelIdx);
}

#endif