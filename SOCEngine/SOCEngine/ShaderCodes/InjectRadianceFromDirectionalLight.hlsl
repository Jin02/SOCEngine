//EMPTY_META_DATA

#include "Injection_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

[numthreads(INJECTION_TILE_RES, INJECTION_TILE_RES, 1)]
void CS(uint3 globalIdx	: SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	uint capacity			= GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	float perShadowMapRes	= (float)(1 << GetNumOfDirectionalLight(shadowGlobalParam_packedPowerOfTwoShadowResolution));

	uint shadowIndex		= globalIdx.x / (uint)perShadowMapRes;
	ParsedShadowParam shadowParam;
	ParseShadowParam(shadowParam, g_inputDirectionalLightShadowParams[shadowIndex]);
	uint lightIndex			= shadowParam.lightIndex;

	float2 shadowMapPos	= float2(globalIdx.x % uint(perShadowMapRes), globalIdx.y);
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth = g_inputDirectionalLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);

	float4 worldPos = mul( float4(shadowMapPos.xy, depth, 1.0f), g_inputDirectionalLightShadowInvVPVMatBuffer[shadowIndex].mat );
	worldPos /= worldPos.w;

	float voxelizeSize	= GetVoxelizeSize(voxelization_currentCascade);
	float3 voxelSpaceUV = (worldPos.xyz - voxelization_minPos) / voxelizeSize;
	int dimension		= (int)GetDimension();
	int3 voxelIdx		= int3(voxelSpaceUV * dimension);

	if( any(voxelIdx < 0) || any(dimension <= voxelIdx) )
		return;

	float4 lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
	float2 lightParam			= g_inputDirectionalLightParamBuffer[lightIndex];
	float3 lightDir				= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

	float4 albedo	= GetColor(g_inputVoxelAlbedoTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float3 normal	= GetNormal(g_inputVoxelNormalTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float4 emission	= GetColor(g_inputVoxelEmissionTexture, voxelIdx, lightDir, voxelization_currentCascade);

	float3 lightColor	= g_inputDirectionalLightColorBuffer[lightIndex].rgb;
	float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
	float intensity		= g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;
	float3 radiosity	= lambert * lightColor * intensity;

	radiosity *= RenderDirectionalLightShadow(lightIndex, worldPos.xyz);
	radiosity += emission.rgb;

	StoreRadiosity(radiosity, albedo.a, normal, voxelIdx);
}

#endif