//EMPTY_META_DATA

#include "LightCullingCompareAtomicCS.h"
#include "BRDF.h"

#if (MSAA_SAMPLES_COUNT > 1)

groupshared uint s_edgePixelCounter;
groupshared uint s_edgePackedPixelIdx[TILE_RES * TILE_RES];

#endif

// Output
RWTexture2D<float4> g_tOutScreen : register( u0 );

void RenderDirectionalLight(out float3 resultDiffuseColor, out float3 resultSpecularColor,
							in LightingParams lightingParams)
{
	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].w;

		float2 lightParam = g_inputDirectionalLightParamBuffer[lightingParams.lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);
	}
}

void RenderPointLight(out float3 resultDiffuseColor, out float3 resultSpecularColor,
					  in LightingParams lightingParams, float3 vertexWorldPosition)
{
	float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightingParams.lightIndex];
	float3 lightCenterWorldPosition	= lightCenterWithRadius.xyz;

	float3 lightDir					= lightCenterWorldPosition - vertexWorldPosition;
	float distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float lightRadius				= lightCenterWithRadius.w;

	if( distanceOfLightAndVertex < lightRadius )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputPointLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputPointLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		float x = distanceOfLightAndVertex / lightRadius;
		float k = 100.0f - commonParams.lightIntensity; //testing
		float falloff = -(1.0f / k) * (1.0f - (k + 1) / (1.0f + k * x * x) );

		resultDiffuseColor	*= falloff;
		resultSpecularColor	*= falloff;
	}
}

void RenderSpotLight(out float3 resultDiffuseColor, out float3 resultSpecularColor,
					 in LightingParams lightingParams, float3 vertexWorldPosition)
{
	float4 spotLightParam	= g_inputSpotLightParamBuffer[lightingParams.lightIndex];
	float3 spotLightDir;
	{
		spotLightDir.xy		= spotLightParam.xy;
		spotLightDir.z		= sqrt(1.0f - spotLightDir.x*spotLightDir.x - spotLightDir.y*spotLightDir.y);	

		bool isDirZMinus	= spotLightParam.w < 0;
		spotLightDir.z		= spotLightDir.z * (1 - (2 * (uint)isDirZMinus));
	}

	float4	lightCenterWithRadius		= g_inputSpotLightTransformBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition	= lightCenterWithRadius.xyz;
	float	lightRadius					= lightCenterWithRadius.w;

	float3	lightRealWorldPosition		= lightCenterWorldPosition - (spotLightDir * lightRadius);
	float3	lightDir					= lightRealWorldPosition - vertexWorldPosition;
	float	distanceOfLightAndVertex	= length(lightDir);
	lightDir = normalize(lightDir);

	float	lightCosineConeAngle		= spotLightParam.z;
	float	currentCosineConeAngle		= dot(-lightDir, spotLightDir);


	if( (distanceOfLightAndVertex < lightRadius) && 
		(lightCosineConeAngle < currentCosineConeAngle) )
	{
		LightingCommonParams commonParams;
		commonParams.lightColor		= g_inputSpotLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputSpotLightColorBuffer[lightingParams.lightIndex].w;
		commonParams.lightDir		= lightDir;

		BRDFLighting(resultDiffuseColor, resultSpecularColor, lightingParams, commonParams);

		float falloffCoef = abs(spotLightParam.w);
		float x = distanceOfLightAndVertex / lightRadius;
		float falloff = -(1.0f / falloffCoef) * (1.0f - (falloffCoef + 1) / (1.0f + falloffCoef * x * x) );

		resultDiffuseColor	*= falloff;
		resultSpecularColor	*= falloff;
	}
}


#if (MSAA_SAMPLES_COUNT > 1) //MSAA
float4 MSAALighting(uint2 globalIdx, uint sampleIdx, uint pointLightCountInThisTile)
{
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( globalIdx, sampleIdx );

	float3 normal = normal_roughness.xyz;
	normal *= 2; normal -= float3(1, 1, 1);

	float roughness = normal_roughness.w;

	float depth = g_tDepth.Load( globalIdx, sampleIdx ).x;

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), g_invViewProjViewport );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( camera_pos.xyz - worldPosition.xyz );

	float4 albedo_metallic = g_tGBufferAlbedo_metallic.Load( globalIdx, sampleIdx );

	float3 albedo	= albedo_metallic.xyz;
	float metallic	= albedo_metallic.w;;

	float4 specular_fresnel0 = g_tGBufferSpecular_fresnel0.Load( globalIdx, sampleIdx );
	float3 specularColor = specular_fresnel0.rgb;
	float fresnel0 = specular_fresnel0.a;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.fresnel0		= fresnel0;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo;
	lightParams.specularColor	= specularColor;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);

	uint startIdx = 0;
	for(uint pointLightIdx=startIdx; pointLightIdx<pointLightCountInThisTile; pointLightIdx++)
	{
		lightParams.lightIndex		= s_lightIdx[pointLightIdx];

		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint spotLightIdx=pointLightCountInThisTile; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
	{
		lightParams.lightIndex = s_lightIdx[spotLightIdx];

		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	uint directionalLightCount = GetNumOfDirectionalLight();
	for(uint directionalLightIdx=0; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		float3 diffuse, specular;
		RenderDirectionalLight(diffuse, specular, lightParams);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	//float3 diffuseColor = albedo - albedo * metallic;
	//float3 specularColor = lerp(0.08f * fresnel0.xxx, albedo, metallic.xxx);

	//float3 result = (accumulativeDiffuse * diffuseColor) + (specularColor * specularColor);
	float3	result = accumulativeDiffuse + accumulativeSpecular;
	return float4(result, 1.0f);
}
#endif

[numthreads(TILE_RES, TILE_RES, 1)]
void TileBasedDeferredShadingCS(uint3 globalIdx : SV_DispatchThreadID, 
								uint3 localIdx	: SV_GroupThreadID,
								uint3 groupIdx	: SV_GroupID)
{
	float minZ, maxZ;
	uint pointLightCountInThisTile = 0;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	bool isDetectedEdge = LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile, minZ, maxZ);
#else
	LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile, minZ, maxZ);
#endif
	GroupMemoryBarrierWithGroupSync();

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( uint2(globalIdx.x, globalIdx.y),	0 );
#else // non-MSAA
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( uint3(globalIdx.x, globalIdx.y, 0) );
#endif

	float3 normal = normal_roughness.xyz;
	normal *= 2; normal -= float3(1, 1, 1);

	float roughness = normal_roughness.w;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	float depth = g_tDepth.Load( uint2(globalIdx.x,	globalIdx.y), 0 ).x;
#else
	float depth = g_tDepth.Load( uint3(globalIdx.x,	globalIdx.y, 0) ).x;
#endif

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), g_invViewProjViewport );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( camera_pos.xyz - worldPosition.xyz );

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 albedo_metallic = g_tGBufferAlbedo_metallic.Load( uint2(globalIdx.x, globalIdx.y), 0 );
#else
	float4 albedo_metallic = g_tGBufferAlbedo_metallic.Load( uint3(globalIdx.x, globalIdx.y, 0) );
#endif

	float3 albedo	= albedo_metallic.xyz;
	float metallic	= albedo_metallic.w;;

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 specular_fresnel0 = g_tGBufferSpecular_fresnel0.Load( uint2(globalIdx.x, globalIdx.y), 0 );
#else
	float4 specular_fresnel0 = g_tGBufferSpecular_fresnel0.Load( uint3(globalIdx.x, globalIdx.y, 0) );
#endif

	float3 specularColor = specular_fresnel0.rgb;
	float fresnel0 = specular_fresnel0.a;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.fresnel0		= fresnel0;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= albedo;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);

	uint startIdx = 0;
	for(uint pointLightIdx=startIdx; pointLightIdx<pointLightCountInThisTile; pointLightIdx++)
	{
		lightParams.lightIndex		= s_lightIdx[pointLightIdx];

		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint spotLightIdx=pointLightCountInThisTile; spotLightIdx<s_lightIndexCounter; ++spotLightIdx)
	{
		lightParams.lightIndex = s_lightIdx[spotLightIdx];

		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, lightParams, worldPosition.xyz);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	uint directionalLightCount = GetNumOfDirectionalLight();
	for(uint directionalLightIdx=0; directionalLightIdx<directionalLightCount; ++directionalLightIdx)
	{
		lightParams.lightIndex = directionalLightIdx;

		float3 diffuse, specular;
		RenderDirectionalLight(diffuse, specular, lightParams);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	//float3 diffuseColor = albedo - albedo * metallic;
	//float3 specularColor = lerp(0.08f * fresnel0.xxx, albedo, metallic.xxx);

	//float3 result = (accumulativeDiffuse * diffuseColor) + (specularColor * specularColor);
	float3	result = accumulativeDiffuse + accumulativeSpecular;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA

	uint2 scale_2_idx = globalIdx.xy * uint2(2, 2);
	g_tOutScreen[scale_2_idx] = float4(result, 1.0f);

	for(uint sampleIdx = 1; sampleIdx < MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
		float3 sampleNormal = g_tGBufferNormal_roughness.Load( uint2(globalIdx.x, globalIdx.y), sampleIdx).rgb;
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

	uint sample_mul_LightCount = (MSAA_SAMPLES_COUNT - 1) * s_edgePixelCounter;
	uint idxInTile = localIdx.x + localIdx.y * TILE_RES;
	for(uint i=idxInTile; i < sample_mul_LightCount; i += SHADING_THREAD_COUNT)
	{
		uint edgePixelIdx = i / (MSAA_SAMPLES_COUNT - 1);
		uint sampleIdx = (i % (MSAA_SAMPLES_COUNT - 1)) + 1; //1부터 시작

		uint packedIdxValue = s_edgePackedPixelIdx[edgePixelIdx];
		uint2 edge_globalIdx_inThisTile = uint2(packedIdxValue & 0x0000ffff, packedIdxValue >> 16);

		uint2 scale_sample_coord = edge_globalIdx_inThisTile * uint2(2, 2);
		scale_sample_coord.x += sampleIdx % 2;
		scale_sample_coord.y += sampleIdx > 1;
		
		float4 lightResult = MSAALighting(edge_globalIdx_inThisTile, sampleIdx, pointLightCountInThisTile);
		g_tOutScreen[scale_sample_coord] = lightResult;
	}

#else // off MSAA
	g_tOutScreen[globalIdx.xy] = float4(result, 1.0f);
#endif
}