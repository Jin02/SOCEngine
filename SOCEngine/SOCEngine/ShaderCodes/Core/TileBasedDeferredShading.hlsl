//EMPTY_META_DATA

#include "LightCulling.h"
#include "BRDF.h"

#if (MSAA_SAMPLES_COUNT > 1)

groupshared uint s_edgePixelCounter;

#endif


void BRDFLighting(out float3 resultDiffuseColor, out float3 resultSpecularColor,
				  in LightingParams lightingParams, in LightingCommonParams commonParamas)
{
	float3 halfVector	= normalize(lightingParams.viewDir + commonParamas.lightDir);

	float NdotL			= saturate(dot(lightingParams.normal,	commonParamas.lightDir));
	float NdotH			= saturate(dot(lightingParams.normal,	halfVector));
	float NdotV			= saturate(dot(lightingParams.normal,	lightingParams.viewDir));
	float VdotH			= saturate(dot(lightingParams.viewDir,	halfVector));
	float LdotH			= saturate(dot(lightingParams.lightDir,	halfVector));
	float VdotL			= saturate(dot(lightingParams.viewDir,	commonParamas.lightDir));

	float	Fr = Fresnel(lightingParams.fresnel0, LdotH) * 
		Geometry(lightingParams.roughness, NdotH, NdotV, NdotL, VdotH) * 
		Distribution(lightingParams.roughness, NdotH) 
		/ (4.0f * NdotL * NdotV);

	float diffuseEnergyConservation = DiffuseEnergyConservation(lightingParams.fresnel0, NdotH);
	resultDiffuseColor	= Diffuse(lightingParams.diffuseColor, lightingParams.roughness, NdotV, NdotL, VdotH, VdotL) * commonParamas.lightColor * diffuseEnergyConservation.xxx * commonParamas.lightIntensity.xxx;

	resultSpecularColor	= Fr * NdotL * commonParamas.lightColor * commonParamas.lightIntensity.xxx;
}

void RenderDirectionalLight(out float3 resultDiffuseColor, out float3 resultSpecularColor,
							in LightingParams lightingParams)
{
	float4	lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightingParams.lightIndex];
	float3	lightCenterWorldPosition = lightCenterWithDirZ.xyz;

	LightingCommonParams commonParams;
	{
		commonParams.lightColor		= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].xyz;
		commonParams.lightIntensity	= g_inputDirectionalLightColorBuffer[lightingParams.lightIndex].w;

		float4	lightParam = g_inputDirectionalLightParamBuffer[lightingParams.lightIndex];
		commonParams.lightDir		= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		BRDFLighting(resultDiffuseColor, resultSpecularColor, commonParams, lightingParams);
	}
}

void RenderPointLight(out float3 resultDiffuseColor, out float3 resultSpecularColor,
					  in LightingParams lightingParams, float3 vertexWorldPosition)
{
	float3 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightingParams.lightIndex];
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

		BRDFLighting(resultDiffuseColor, resultSpecularColor, commonParams, lightingParams);

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

	float3	lightCenterWithRadius		= g_inputSpotLightTransformBuffer[lightingParams.lightIndex];
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

		BRDFLighting(resultDiffuseColor, resultSpecularColor, commonParams, lightingParams);

		float falloffCoef = abs(spotLightParam.w);
		float x = distanceOfLightAndVertex / lightRadius;
		float falloff = -(1.0f / falloffCoef) * (1.0f - (falloffCoef + 1) / (1.0f + falloffCoef * x * x) );

		resultDiffuseColor	*= falloff;
		resultSpecularColor	*= falloff;
	}
}


#if (MSAA_SAMPLES_COUNT > 1) //MSAA
void MSAALighting(uint2 globalIdx, uint sampleIdx)
{
	float4 normal_roughness = g_tGBufferNormal_roughness.Load( globalIdx, sampleIdx );

	float3 normal = normal_roughness.xyz;
	normal *= 2; normal -= float3(1, 1, 1);

	float roughness = normal_roughness.w;

	float depth = g_tDepth.Load( globalIdx, sampleIdx ).x;

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), g_invViewProjViewport );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( g_cameraWorldPosition - worldPosition.xyz );

	float4 albedo_metallic = g_tDepth.Load( globalIdx, sampleIdx ).x;

	float3 albedo	= albedo_metallic.xyz;
	float metallic	= albedo_metallic.w;;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.fresnel0		= fresnel;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= diffsueColor;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);

	uint startIdx = 0;
	for(uint i=startIdx; i<pointLightCountInThisTile; i++)
	{
		lightParams.lightIndex		= s_lightIdx[i];

		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, lightParams, worldPosition);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint i=pointLightCountInThisTile; i<s_lightIndexCounter; ++i)
	{
		lightParams.lightIndex = s_lightIdx[i];

		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, lightParams, worldPosition);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint i=0; i<g_directionalLightCount; ++i)
	{
		lightParams.lightIndex = i;

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

// Output
RWTexture2D<float4> g_tOutScreen : register( u0 );

[numthreads(TILE_RES, TILE_RES, 1)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
		uint3 localIdx	: SV_GroupThreadID,
		uint3 groupIdx	: SV_GroupID)
{
	float minZ, maxZ;
	uint pointLightCountInThisTile = 0;

	if( (localIdx.x < TILE_RES_HALF) && (localIdx.y < TILE_RES_HALF) )
		LightCulling(globalIdx, localIdx, groupIdx, pointLightCountInThisTile, minZ, maxZ);

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
	float dpeth = g_tDepth.Load( uint3(globalIdx.x,	globalIdx.y, 0) ).x;
#endif

	float4 worldPosition = mul( float4((float)globalIdx.x, (float)globalIdx.y, depth, 1.0), g_invViewProjViewport );
	worldPosition /= worldPosition.w;

	float3 viewDir = normalize( g_cameraWorldPosition - worldPosition.xyz );

#if (MSAA_SAMPLES_COUNT > 1) // MSAA
	float4 albedo_metallic = g_tDepth.Load( uint2(globalIdx.x, globalIdx.y), 0 ).x;
#else
	float4 albedo_metallic = g_tDepth.Load( uint3(globalIdx.x, globalIdx.y,	0) ).x;
#endif

	float3 albedo	= albedo_metallic.xyz;
	float metallic	= albedo_metallic.w;;

	LightingParams lightParams;

	lightParams.viewDir			= viewDir;
	lightParams.normal			= normal;
	lightParams.fresnel0		= fresnel;
	lightParams.roughness		= roughness;
	lightParams.diffuseColor	= diffsueColor;

	float3 accumulativeDiffuse	= float3(0.0f, 0.0f, 0.0f);
	float3 accumulativeSpecular	= float3(0.0f, 0.0f, 0.0f);

	uint startIdx = 0;
	for(uint i=startIdx; i<pointLightCountInThisTile; i++)
	{
		lightParams.lightIndex		= s_lightIdx[i];

		float3 diffuse, specular;
		RenderPointLight(diffuse, specular, lightParams, worldPosition);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint i=pointLightCountInThisTile; i<s_lightIndexCounter; ++i)
	{
		lightParams.lightIndex = s_lightIdx[i];

		float3 diffuse, specular;
		RenderSpotLight(diffuse, specular, lightParams, worldPosition);

		accumulativeDiffuse			+= diffuse;
		accumulativeSpecular		+= specular;
	}

	for(uint i=0; i<g_directionalLightCount; ++i)
	{
		lightParams.lightIndex = i;

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
	bool	isDetectedEdge = s_isDetectedEdge[localIdx.x + localIdx.y * TILE_RES];

	uint2 scale_2_idx = globalIdx.xy * uint(2, 2);
	g_tOutScreen[scale_2_idx] = float4(result, 1.0f);

	for(uint sampleIdx = 1; sampleIdx < MSAA_SAMPLES_COUNT; ++sampleIdx)
	{
		float3 sampleNormal = g_tGBufferNormal_roughness.Load( uint2(globalIdx.x, globalIdx.y), sampleIdx);
		sampleNormal *= 2; sampleNormal -= float3(1.0f, 1.0f, 1.0f);
		isDetectedEdge = isDetectedEdge || (dot(sampleNormal, normal) < 1.04719755f); //1.04719755 is 60 degree
	}

	if(isDetectedEdge)
	{
		uint targetIdx = 0;
		InterlockedAdd(s_edgePixelCounter, 1, targetIdx);
		
		s_edgePixelIdx[targetIdx] = (globalIdx.y << 16) | globalIdx.x;
	}
	else
	{
		g_tOutScreen[scale_2_idx + uint(1, 0)] = float4(result, 1.0f);
		g_tOutScreen[scale_2_idx + uint(0, 1)] = float4(result, 1.0f);
		g_tOutScreen[scale_2_idx + uint(1, 1)] = float4(result, 1.0f);
	}

	GroupMemoryBarrierWithGroupSync();

	uint sample_mul_LightCount = (NUM_MSAA_SAMPLES - 1) * s_edgePixelCounter;
	uint idxInTile = localIdx.x + localIdx.y * TILE_RES;
	for(uint i=idxInTile; i < sample_mul_LightCount; i += SHADING_THREAD_COUNT)
	{
		uint edgePixelIdx = i / (NUM_MSAA_SAMPLES - 1);
		uint sampleIdx = (i % (NUM_MSAA_SAMPLES - 1)) + 1; //1부터 시작

		uint packedIdxValue = s_edgePixelIdx[edgePixelIdx];
		uint2 edge_globalIdx_inThisTile = uint2(packedIdxValue & 0x0000ffff, packedIdxValue >> 16);

		uint2 sacle_sample_coord = edge_globalIdx_inThisTile * uint(2, 2);
		scale_sample_coord.x += sampleIdx % 2;
		scale_sample_coord.y += sampleIdx > 1;
		
		float4 lightResult = MSAALighting(edge_globalIdx_inThisTile, sampleIdx);
		g_tOutScreen[scale_sample_coord] = lightResult;
	}

#else // off MSAA
	g_tOutScreen[globalidx.xy] = float4(result, 1.0f);
#endif
}