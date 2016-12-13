//EMPTY_META_DATA

#include "LightCullingCompareAtomicCS.h"
#include "DynamicLighting.h"
#include "GBufferParser.h"

#if (MSAA_SAMPLES_COUNT > 1)

groupshared uint s_edgePixelCounter;
groupshared uint s_edgePackedPixelIdx[LIGHT_CULLING_TILE_RES * LIGHT_CULLING_TILE_RES];

#define EDGE_MIN_DEGREE 60.0f

#endif

// Output
RWTexture2D<float4> OutDiffuseLightBuffer		: register( u0 );
RWTexture2D<float4> OutSpecularLightBuffer		: register( u1 );

#if defined(STORE_PER_LIGHT_INDICES_IN_TILE)
RWBuffer<uint>		OutPerLightIndicesInTile	: register( u2 );
#endif

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
void MSAALighting(
	out float4 outAccumulativeDiffuse, out float4 outAccumulativeSpecular,
	uint2 globalIdx, uint sampleIdx, uint pointLightCountInThisTile)
{
	Surface surface;
	ParseGBufferSurface(surface, globalIdx.xy, sampleIdx, true);

	float3 viewDir = normalize( camera_worldPos - surface.worldPos );

	LightingParams lightParams;
	lightParams.viewDir			= viewDir;
	lightParams.normal			= surface.normal;
	lightParams.roughness		= surface.roughness;
	lightParams.diffuseColor	= surface.albedo;
	lightParams.specularColor	= surface.specular;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 localDiffuse			= float3(0.0f, 0.0f, 0.0f);
	float3 localSpecular		= float3(0.0f, 0.0f, 0.0f);

	uint pointLightIdx = (int)(surface.depth == 0.0f) * pointLightCountInThisTile;
	for(; pointLightIdx<pointLightCountInThisTile; ++pointLightIdx)
	{
		lightParams.lightIndex		= s_lightIdx[pointLightIdx];

		RenderPointLight(localDiffuse, localSpecular, lightParams, surface.worldPos);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint spotLightIdx = lerp(s_lightIndexCounter, pointLightCountInThisTile, surface.depth != 0.0f);
	for(; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
	{
		lightParams.lightIndex = s_lightIdx[spotLightIdx];

		RenderSpotLight(localDiffuse, localSpecular, lightParams, surface.worldPos);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	uint directionalLightCount = GetNumOfDirectionalLight(tbrParam_packedNumOfLights);
	uint directionalLightIdx = (int)(surface.depth == 0.0f) * directionalLightCount;
	for(; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		RenderDirectionalLight(localDiffuse, localSpecular, lightParams, surface.worldPos);

		accumulativeDiffuse			+= localDiffuse;
		accumulativeSpecular		+= localSpecular;
	}

	outAccumulativeDiffuse	= accumulativeDiffuse + surface.emission.rgb;
	outAccumulativeSpecular	= accumulativeSpecular;
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

	Surface surface;
	ParseGBufferSurface(surface, globalIdx.xy, 0, true);

	float3 viewDir				= normalize( camera_worldPos - surface.worldPos );

	LightingParams lightParams;
	lightParams.viewDir			= viewDir;
	lightParams.normal			= surface.normal;
	lightParams.roughness		= surface.roughness;
	lightParams.diffuseColor	= surface.albedo;
	lightParams.specularColor	= surface.specular;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);
	float3 localDiffuse			= float3(0.0f, 0.0f, 0.0f);
	float3 localSpecular		= float3(0.0f, 0.0f, 0.0f);

	// Lighting Pass
	{
		uint pointLightIdx = (int)(surface.depth == 0.0f) * pointLightCountInThisTile;
		for(; pointLightIdx<pointLightCountInThisTile; ++pointLightIdx)
		{
			lightParams.lightIndex		= s_lightIdx[pointLightIdx];
	
			RenderPointLight(localDiffuse, localSpecular, lightParams, surface.worldPos);
	
			accumulativeDiffuse			+= localDiffuse;
			accumulativeSpecular		+= localSpecular;
		}
	
		uint spotLightIdx = lerp(s_lightIndexCounter, pointLightCountInThisTile, surface.depth != 0.0f);
		for(; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
		{
			lightParams.lightIndex = s_lightIdx[spotLightIdx];
	
			RenderSpotLight(localDiffuse, localSpecular, lightParams, surface.worldPos);
	
			accumulativeDiffuse			+= localDiffuse;
			accumulativeSpecular		+= localSpecular;
		}
	
		uint directionalLightCount = GetNumOfDirectionalLight(tbrParam_packedNumOfLights);
		uint directionalLightIdx = (int)(surface.depth == 0.0f) * directionalLightCount;
		for(; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
		{
			lightParams.lightIndex = directionalLightIdx;
	
			RenderDirectionalLight(localDiffuse, localSpecular, lightParams, surface.worldPos);
	
			accumulativeDiffuse			+= localDiffuse;
			accumulativeSpecular		+= localSpecular;
	
#if defined(DEBUG_MODE)
			isRenderDL = true;
#endif
		}
	}
	
	accumulativeDiffuse += surface.emission.rgb;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA

	uint2 scale_2_idx = globalIdx.xy * 2;
	OutDiffuseLightBuffer[scale_2_idx]	= float4(accumulativeDiffuse, 1.0f);
	OutSpecularLightBuffer[scale_2_idx]	= float4(accumulativeSpecular, 1.0f);

	float3 sampleNormal = float3(0.0f, 0.0f, 0.0f);
	for(uint sampleIdx = 1; sampleIdx < MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
		sampleNormal = GBufferNormal_roughness.Load( globalIdx.xy, sampleIdx).rgb;
		sampleNormal *= 2; sampleNormal -= float3(1.0f, 1.0f, 1.0f);

		isDetectedEdge = isDetectedEdge || (dot(sampleNormal, surface.normal) < DEG_2_RAD(EDGE_MIN_DEGREE) );
	}

	if(isDetectedEdge)
	{
		uint targetIdx = 0;
		InterlockedAdd(s_edgePixelCounter, 1, targetIdx);
		
		s_edgePackedPixelIdx[targetIdx] = (globalIdx.y << 16) | globalIdx.x;
	}
	else
	{

#ifndef USE_POST_PROCESSING_BLOOM
	accumulativeDiffuse		= ToGamma(accumulativeDiffuse, tbrParam_gamma);
	accumulativeSpecular	= ToGamma(accumulativeSpecular, tbrParam_gamma);
#endif

		OutDiffuseLightBuffer[scale_2_idx + uint2(1, 0)] = float4(accumulativeDiffuse, 1.0f);
		OutDiffuseLightBuffer[scale_2_idx + uint2(0, 1)] = float4(accumulativeDiffuse, 1.0f);
		OutDiffuseLightBuffer[scale_2_idx + uint2(1, 1)] = float4(accumulativeDiffuse, 1.0f);

		OutSpecularLightBuffer[scale_2_idx + uint2(1, 0)] = float4(accumulativeSpecular, 1.0f);
		OutSpecularLightBuffer[scale_2_idx + uint2(0, 1)] = float4(accumulativeSpecular, 1.0f);
		OutSpecularLightBuffer[scale_2_idx + uint2(1, 1)] = float4(accumulativeSpecular, 1.0f);
	}

	GroupMemoryBarrierWithGroupSync();

	uint edgePixelIdx			= 0;
		 sampleIdx				= 0;
	uint2 scale_sample_coord	= uint2(0, 0);
	uint sample_mul_LightCount = (MSAA_SAMPLES_COUNT - 1) * s_edgePixelCounter;
	for(uint i=idxInTile; i < sample_mul_LightCount; i += THREAD_COUNT)
	{
		edgePixelIdx = i / (MSAA_SAMPLES_COUNT - 1);
		sampleIdx = (i % (MSAA_SAMPLES_COUNT - 1)) + 1; //1ºÎÅÍ ½ÃÀÛ

		uint packedIdxValue = s_edgePackedPixelIdx[edgePixelIdx];

		uint2 edge_globalIdx_inThisTile;
		edge_globalIdx_inThisTile.x = packedIdxValue & 0x0000ffff;
		edge_globalIdx_inThisTile.y = packedIdxValue >> 16;

		scale_sample_coord = edge_globalIdx_inThisTile * 2;
		scale_sample_coord.x += sampleIdx % 2;
		scale_sample_coord.y += sampleIdx > 1;
		
		float4 diffuse	= float4(0.0f, 0.0f, 0.0f, 0.0f);
		float4 specular	= float4(0.0f, 0.0f, 0.0f, 0.0f);
		MSAALighting(edge_globalIdx_inThisTile, sampleIdx, pointLightCountInThisTile);

#ifndef USE_POST_PROCESSING_BLOOM
		diffuse		= ToGamma(diffuse, tbrParam_gamma);
		specular	= ToGamma(specular, tbrParam_gamma);
#endif

		OutDiffuseLightBuffer[scale_sample_coord]	= diffuse;
		OutSpecularLightBuffer[scale_sample_coord]	= specular;
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

	OutDiffuseLightBuffer[globalIdx.xy] = float4(debugTiles, 1.0f);
#else

#if defined(USE_POST_PROCESSING_BLOOM)
	OutDiffuseLightBuffer[globalIdx.xy]		= float4(accumulativeDiffuse,	1.0f);
	OutSpecularLightBuffer[globalIdx.xy]	= float4(accumulativeSpecular,	1.0f);
#else
	OutDiffuseLightBuffer[globalIdx.xy]		= float4( ToGamma(accumulativeDiffuse, tbrParam_gamma),		1.0f);
	OutSpecularLightBuffer[globalIdx.xy]	= float4( ToGamma(accumulativeSpecular, tbrParam_gamma),	1.0f);
#endif
//	OutDiffuseLightBuffer[globalIdx.xy]		= float4(surface.albedo,	1.0f);
//	OutSpecularLightBuffer[globalIdx.xy]	= float4(0,0,0,	1.0f);

#endif

#endif

#if defined(STORE_PER_LIGHT_INDICES_IN_TILE)
	uint globalTileIdx = groupIdx.x + groupIdx.y * GetNumTilesX();
	uint startOffset = tbrParam_maxNumOfPerLightInTile * globalTileIdx + 1;

	if(idxInTile == 0)
	{
		uint spotLightCount		= s_lightIndexCounter - pointLightCountInThisTile;
		uint pointLightCount	= pointLightCountInThisTile & 0x0000ffff;
		OutPerLightIndicesInTile[startOffset - 1] = (spotLightCount << 16) | pointLightCount;
	}

	for(uint i=idxInTile; i<pointLightCountInThisTile; i+=THREAD_COUNT)
		OutPerLightIndicesInTile[startOffset + i] = s_lightIdx[i];

	for(uint j=(idxInTile + pointLightCountInThisTile); j<s_lightIndexCounter; j+=THREAD_COUNT)
		OutPerLightIndicesInTile[startOffset + j] = s_lightIdx[j];
#endif
}
