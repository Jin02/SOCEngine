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
	ParseShadowParam(shadowParam, DirectionalLightShadowParams[shadowIndex]);
	uint lightIndex			= shadowParam.lightIndex;

	float2 shadowMapPos	= float2(globalIdx.x % uint(perShadowMapRes), globalIdx.y);
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth = DirectionalLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);

	float4 worldPos = mul( float4(shadowMapPos.xy, depth, 1.0f), DirectionalLightShadowInvVPVMatBuffer[shadowIndex].mat );
	worldPos /= worldPos.w;

	float voxelizeSize	= GetVoxelizeSize(voxelization_currentCascade);
	float3 voxelSpaceUV = (worldPos.xyz - voxelization_minPos) / voxelizeSize;
	int dimension		= (int)GetDimension();
	int3 voxelIdx		= int3(voxelSpaceUV * dimension);

	if( any(voxelIdx < 0) || any(dimension <= voxelIdx) )
		return;

	float4 lightCenterWithDirZ	= DirectionalLightTransformWithDirZBuffer[lightIndex];
	float2 lightParam			= DirectionalLightParamBuffer[lightIndex];
	float3 lightDir				= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

	float3 normal	= GetNormal(VoxelNormalMap, voxelIdx, voxelization_currentCascade);
	float4 albedo	= GetColor(VoxelAlbedoMap, voxelIdx, voxelization_currentCascade);
	float4 emission	= GetColor(VoxelEmissionMap, voxelIdx, voxelization_currentCascade);

	float3 lightColor	= DirectionalLightColorBuffer[lightIndex].rgb;
	float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
	float intensity		= DirectionalLightColorBuffer[lightIndex].a * 10.0f;
	float3 radiosity	= lambert * lightColor * intensity;

//	radiosity *= RenderDirectionalLightShadow(lightIndex, worldPos.xyz);
	radiosity += emission.rgb;

	StoreRadiosity(OutVoxelColorMap, radiosity, albedo.a, normal, voxelIdx, voxelization_currentCascade);
}

#endif