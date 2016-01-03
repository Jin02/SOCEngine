//EMPTY_META_DATA

#include "LightCullingCompareAtomicCS.h"
#include "DynamicLighting.h"

#if (MSAA_SAMPLES_COUNT > 1)

groupshared uint s_edgePixelCounter;
groupshared uint s_edgePackedPixelIdx[LIGHT_CULLING_TILE_RES * LIGHT_CULLING_TILE_RES];

#endif

// Output
RWTexture2D<float4> g_tOutScreen : register( u0 );

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
float4 MSAALighting(uint2 globalIdx, uint sampleIdx, uint pointLightCountInThisTile)
{
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( globalIdx, sampleIdx );

	float3 normal = normal_roughness.xyz;
	normal *= 2; normal -= float3(1.0f, 1.0f, 1.0f);

	float roughness = normal_roughness.w;

	float depth = g_tDepth.Load( globalIdx, sampleIdx ).x;

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), tbrParam_invViewProjViewportMat );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( tbrParam_cameraWorldPosition.xyz - worldPosition.xyz );

	float4 albedo_emission	= g_tGBufferAlbedo_emission.Load( globalIdx, sampleIdx );
	float3 albedo			= albedo_emission.xyz;

	float4 specular_metallic = g_tGBufferSpecular_metallic.Load( globalIdx, sampleIdx );
	float3 specularColor = specular_metallic.rgb;
	float metallic = specular_metallic.a;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo;
	lightParams.specularColor	= specularColor;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);

	float3 localDiffuse		= float3(0.0f, 0.0f, 0.0f);
	float3 localSpecular	= float3(0.0f, 0.0f, 0.0f);

	uint pointLightIdx = (int)(depth == 0.0f) * pointLightCountInThisTile;
	for(; pointLightIdx<pointLightCountInThisTile; ++pointLightIdx)
	{
		lightParams.lightIndex		= s_lightIdx[pointLightIdx];

		RenderPointLight(localDiffuse, localSpecular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint spotLightIdx = lerp(s_lightIndexCounter, pointLightCountInThisTile, depth != 0.0f);
	for(; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
	{
		lightParams.lightIndex = s_lightIdx[spotLightIdx];

		RenderSpotLight(localDiffuse, localSpecular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint directionalLightCount = GetNumOfDirectionalLight();
	uint directionalLightIdx = (int)(depth == 0.0f) * directionalLightCount;
	for(; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		RenderDirectionalLight(localDiffuse, localSpecular, lightParams);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	float3	result = accumulativeDiffuse + accumulativeSpecular;
	return float4(result, 1.0f);
}
#endif

#if defined(DEBUG_MODE)
groupshared bool isRenderDL;
#endif

[numthreads(LIGHT_CULLING_TILE_RES, LIGHT_CULLING_TILE_RES, 1)]
void TileBasedDeferredShadingCS(uint3 globalIdx : SV_DispatchThreadID, 
								uint3 localIdx	: SV_GroupThreadID,
								uint3 groupIdx	: SV_GroupID)
{
	uint pointLightCountInThisTile = 0;

	uint idxInTile = localIdx.x + localIdx.y * LIGHT_CULLING_TILE_RES;
	if(idxInTile == 0)
	{
#if (MSAA_SAMPLES_COUNT > 1) // MSAA
		s_edgePixelCounter = 0;
#endif
#if defined(DEBUG_MODE)
		isRenderDL = false;
#endif
		s_lightIndexCounter	= 0;
		s_minZ = 0x7f7fffff; //float max as uint
		s_maxZ = 0;
	}

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	bool isDetectedEdge = LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile);
#else
	LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile);
#endif
	GroupMemoryBarrierWithGroupSync();

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( globalIdx.xy, 0 );
#else // non-MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( uint3(globalIdx.xy, 0) );
#endif

	float3 normal = normal_roughness.xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	float roughness = normal_roughness.w;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	float depth = g_tDepth.Load( globalIdx.xy, 0 ).x;
#else
	float depth = g_tDepth.Load( uint3(globalIdx.xy, 0) ).x;
#endif

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), tbrParam_invViewProjViewportMat );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( tbrParam_cameraWorldPosition.xyz - worldPosition.xyz );

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 albedo_emission = g_tGBufferAlbedo_emission.Load( globalIdx.xy, 0 );
#else
	float4 albedo_emission = g_tGBufferAlbedo_emission.Load( uint3(globalIdx.xy, 0) );
#endif
	float3 albedo	= albedo_emission.xyz;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 specular_metallic = g_tGBufferSpecular_metallic.Load( globalIdx.xy, 0 );
#else
	float4 specular_metallic = g_tGBufferSpecular_metallic.Load( uint3(globalIdx.xy, 0) );
#endif
	float3 specularColor = specular_metallic.rgb;
	float metallic = specular_metallic.a;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo;
	lightParams.specularColor	= specularColor;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 localDiffuse			= float3(0.0f, 0.0f, 0.0f);
	float3 localSpecular		= float3(0.0f, 0.0f, 0.0f);

	uint pointLightIdx = (int)(depth == 0.0f) * pointLightCountInThisTile;
	for(; pointLightIdx<pointLightCountInThisTile; ++pointLightIdx)
	{
		lightParams.lightIndex		= s_lightIdx[pointLightIdx];

		RenderPointLight(localDiffuse, localSpecular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint spotLightIdx = lerp(s_lightIndexCounter, pointLightCountInThisTile, depth != 0.0f);
	for(; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
	{
		lightParams.lightIndex = s_lightIdx[spotLightIdx];

		RenderSpotLight(localDiffuse, localSpecular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint directionalLightCount = GetNumOfDirectionalLight(tbrParam_packedNumOfLights);
	uint directionalLightIdx = (int)(depth == 0.0f) * directionalLightCount;
	for(; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		RenderDirectionalLight(localDiffuse, localSpecular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;

#if defined(DEBUG_MODE)
		isRenderDL = true;
#endif
	}

	float3	result = accumulativeDiffuse + accumulativeSpecular;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA

	uint2 scale_2_idx = globalIdx.xy * 2;
	g_tOutScreen[scale_2_idx] = float4(result, 1.0f);

	float3 sampleNormal = float3(0.0f, 0.0f, 0.0f);
	for(uint sampleIdx = 1; sampleIdx < MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
		sampleNormal = g_tGBufferNormal_roughness.Load( globalIdx.xy, sampleIdx).rgb;
		sampleNormal *= 2; sampleNormal -= float3(1.0f, 1.0f, 1.0f);

		isDetectedEdge = isDetectedEdge || (dot(sampleNormal, normal) < DEG_2_RAD(60.0f) );
	}

	if(isDetectedEdge)
	{
		uint targetIdx = 0;
		InterlockedAdd(s_edgePixelCounter, 1, targetIdx);
		
		s_edgePackedPixelIdx[targetIdx] = (globalIdx.y << 16) | globalIdx.x;
	}
	else
	{
		g_tOutScreen[scale_2_idx + uint2(1, 0)] = float4(result, 1.0f);
		g_tOutScreen[scale_2_idx + uint2(0, 1)] = float4(result, 1.0f);
		g_tOutScreen[scale_2_idx + uint2(1, 1)] = float4(result, 1.0f);
	}

	GroupMemoryBarrierWithGroupSync();

	uint edgePixelIdx			= 0;
	uint sampleIdx				= 0;
	uint2 scale_sample_coord	= uint2(0, 0);
	float4 lightResult			= float4(0.0f, 0.0f, 0.0f, 0.0f);

	uint sample_mul_LightCount = (MSAA_SAMPLES_COUNT - 1) * s_edgePixelCounter;
	for(uint i=idxInTile; i < sample_mul_LightCount; i += THREAD_COUNT)
	{
		edgePixelIdx = i / (MSAA_SAMPLES_COUNT - 1);
		sampleIdx = (i % (MSAA_SAMPLES_COUNT - 1)) + 1; //1부터 시작

		packedIdxValue = s_edgePackedPixelIdx[edgePixelIdx];
		edge_globalIdx_inThisTile.x = packedIdxValue & 0x0000ffff;
		edge_globalIdx_inThisTile.y = packedIdxValue >> 16;

		scale_sample_coord = edge_globalIdx_inThisTile * 2;
		scale_sample_coord.x += sampleIdx % 2;
		scale_sample_coord.y += sampleIdx > 1;
		
		lightResult = MSAALighting(edge_globalIdx_inThisTile, sampleIdx, pointLightCountInThisTile);
		g_tOutScreen[scale_sample_coord] = lightResult;
	}

#else // off MSAA

#if defined(DEBUG_MODE)
	float3 debugTiles = float3(0, 0, 0);
	int debugLightCount = s_lightIndexCounter + (int)isRenderDL * directionalLightCount;

	if(debugLightCount > 0)
		debugTiles = float3(1, 0, 0);
	if(debugLightCount > 1)
		debugTiles = float3(0, 1, 0);
	if(debugLightCount > 2)
		debugTiles = float3(0, 0, 1);
	if(debugLightCount > 3)
		debugTiles = float3(0, 1, 1);
	if(debugLightCount > 4)
		debugTiles = float3(1, 1, 0);
	if(debugLightCount > 5)
		debugTiles = float3(1, 1, 1);	

	g_tOutScreen[globalIdx.xy] = float4(debugTiles, 1.0f);
#else
	g_tOutScreen[globalIdx.xy] = float4(result, 1.0f);
#endif

#endif
}