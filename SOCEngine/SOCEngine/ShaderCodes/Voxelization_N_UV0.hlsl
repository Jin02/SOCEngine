#define USE_OUT_ANISOTROPIC_VOXEL_TEXTURES

#include "Voxelization_Common.h"

#ifndef USE_SHADOW_INVERTED_DEPTH
#include "Injection_Common.h"
#endif

struct VS_INPUT
{
	float3 position 			: POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

struct VS_OUTPUT
{
	float3 localPos				: LOCAL_POSITION;
	float3 normal 				: NORMAL;

	float2 uv					: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT output;

	output.localPos		= input.position;
	output.uv			= input.uv;
	output.normal 		= mul(input.normal, (float3x3)transform_worldInvTranspose);
 
    return output;
}

struct GS_OUTPUT
{
	float4 position				: SV_POSITION;
//	float3 voxelPos				: VOXEL_POSITION;
	float3 worldPos				: WORLD_POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

[maxvertexcount(3)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> outputStream)
{
	float4 worldPos[3] = 
	{
		mul(float4(input[0].localPos, 1.0f), transform_world),
		mul(float4(input[1].localPos, 1.0f), transform_world),
		mul(float4(input[2].localPos, 1.0f), transform_world),
	};

	float3 faceNormal = cross(normalize(worldPos[1].xyz - worldPos[0].xyz), normalize(worldPos[2].xyz - worldPos[0].xyz));

	float3 axis;
	axis.x = abs( dot(float3(1, 0, 0), faceNormal) );
	axis.y = abs( dot(float3(0, 1, 0), faceNormal) );
	axis.z = abs( dot(float3(0, 0, 1), faceNormal) );

	matrix viewProjMat;
	if(		axis.x > max(axis.y, axis.z))
		viewProjMat = voxelization_vp_axisX;
	else if(axis.y > max(axis.x, axis.z))
		viewProjMat = voxelization_vp_axisY;
	else if(axis.z > max(axis.x, axis.y))
		viewProjMat = voxelization_vp_axisZ;

	float4 position[3] =
	{
		mul(worldPos[0], viewProjMat),
		mul(worldPos[1], viewProjMat),
		mul(worldPos[2], viewProjMat),
	};

	[unroll]
	for(uint i=0; i<3; ++i)
	{
		GS_OUTPUT output;
		output.position	= position[i];
//		output.voxelPos	= mul(voxelization_worldToVoxel, worldPos[i]).xyz;
		output.uv		= input[i].uv;
		output.normal	= input[i].normal;
		output.worldPos	= worldPos[i].xyz;

		outputStream.Append(output);
	}

	outputStream.RestartStrip();
}

void PS( GS_OUTPUT input )
{
	float4 diffuseTex	= diffuseTexture.Sample(defaultSampler, input.uv);
	float3 mainColor	= abs(material_mainColor);
	float3 albedo		= lerp(mainColor, diffuseTex.rgb * mainColor, HasDiffuseTexture());

	//float opacityMap	= 1.0f - opacityTexture.Sample(defaultSampler, input.uv).x;
	float alpha			= 1.0f;//lerp(1.0f, diffuseTex.a, HasDiffuseTexture()) * opacityMap * ParseMaterialAlpha();

	float3 normal		= normalize(input.normal);
	int dimension		= 128;//int(GetDimension());

	float voxelSize		= ComputeVoxelSize(voxelization_currentCascade);
	int3 voxelIdx		= int3( (input.worldPos - voxelization_minPos) / voxelSize );

//#if 1
#if defined(USE_ANISOTROPIC_VOXELIZATION)
	float anisotropicNormals[6] = {
		 normal.x,
		-normal.x,
		 normal.y,
		-normal.y,
		 normal.z,
		-normal.z
	};
#endif

	if(all(0 <= voxelIdx) && all(voxelIdx < dimension))
	{
		voxelIdx.y += voxelization_currentCascade * dimension;

#if defined(USE_ANISOTROPIC_VOXELIZATION)
		for(int faceIndex=0; faceIndex<6; ++faceIndex)
		{
			int3 index = voxelIdx;
			index.x += (faceIndex * dimension);
			
			float storeRatio = max(anisotropicNormals[faceIndex], 0.0f);

			float3 outAlbedo = albedo.xyz * storeRatio;
			StoreVoxelMapAtomicColorAvg(OutVoxelAlbedoTexture,				index, float4(outAlbedo, alpha));

			float3 outEmission = material_emissionColor.xyz * storeRatio;
			StoreVoxelMapAtomicColorAvg(OutVoxelEmissionTexture,			index, float4(outEmission, 1.0f));
		}

#else
		StoreVoxelMapAtomicColorAvg(OutVoxelAlbedoTexture,		voxelIdx,	float4(albedo.xyz, alpha));
//		OutVoxelAlbedoTexture[voxelIdx] = Float4ColorToUint( float4(albedo.xyz, alpha) );

		StoreVoxelMapAtomicColorAvg(OutVoxelEmissionTexture,	voxelIdx,	float4(material_emissionColor.xyz, 1.0f));
#endif
		normal = normal * 0.5f + 0.5f;
		StoreVoxelMapAtomicColorAvg(OutVoxelNormalTexture,		voxelIdx,	float4(normal, 1.0f));
	}
//#else
//	float3 radiosity = float3(0.0f, 0.0f, 0.0f);
//
//	uint dlShadowCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadows);
//	for(uint dlShadowIdx=0; dlShadowIdx<dlShadowCount; ++dlShadowIdx)
//	{
//		uint lightIndex = g_inputDirectionalLightShadowIndexToLightIndex[dlShadowIdx];
//
//		float4 lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
//		float2 lightParam			= g_inputDirectionalLightParamBuffer[lightIndex];
//		float3 lightDir				= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);
//
//		float3 lightColor	= g_inputDirectionalLightColorBuffer[lightIndex].rgb;
//		float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
//		float intensity		= g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;
//
//		radiosity += lambert * lightColor * intensity * RenderDirectionalLightShadow(lightIndex, input.worldPos);
//		radiosity += material_emissionColor.rgb;
//	}
//
//	uint plShadowCount = GetNumOfPointLight(shadowGlobalParam_packedNumOfShadows);
//	for(uint plShadowIdx=0; plShadowIdx<plShadowCount; ++plShadowIdx)
//	{
//		uint lightIndex = g_inputPointLightShadowIndexToLightIndex[plShadowIdx];
//
//		float4 lightCenterWithRadius	= g_inputPointLightTransformBuffer[lightIndex];
//		float3 lightCenterWorldPos		= lightCenterWithRadius.xyz;
//		float lightRadius				= lightCenterWithRadius.a;
//		float3 lightDir					= lightCenterWorldPos - input.worldPos;
//		float distanceOfLightWithVertex	= length(lightDir);
//
//		if(distanceOfLightWithVertex < lightRadius)
//		{
//			float4 lightColorWithLm = g_inputPointLightColorBuffer[lightIndex];
//
//			float3 lightColor	= lightColorWithLm.rgb;
//			float lumen			= lightColorWithLm.a * 12750.0f; //maximum lumen is 12,750f
//			float attenuation	= lumen / (distanceOfLightWithVertex * distanceOfLightWithVertex);
//			float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
//
//			radiosity += lambert * attenuation * lightColor * RenderPointLightShadow(lightIndex, input.worldPos, lightDir, distanceOfLightWithVertex / lightRadius);
//			radiosity += material_emissionColor.rgb;
//		}
//	}
//
//	uint slShadowCount = GetNumOfSpotLight(shadowGlobalParam_packedNumOfShadows);
//	for(uint slShadowIdx=0; slShadowIdx<slShadowCount; ++slShadowIdx)
//	{
//		uint lightIndex = g_inputSpotLightShadowIndexToLightIndex[slShadowIdx];
//
//		float4 lightCenterWithRadius = g_inputSpotLightTransformBuffer[lightIndex];
//		float3 lightCenterWorldPos = lightCenterWithRadius.xyz;
//		float radiusWithMinusZDirBit = lightCenterWithRadius.a;
//		float radius = abs(radiusWithMinusZDirBit);
//		
//		float4 spotParam = g_inputSpotLightParamBuffer[lightIndex];
//
//		float3 lightDir = float3(spotParam.x, spotParam.y, 0.0f);
//		lightDir.z = sqrt(1.0f - lightDir.x*lightDir.x - lightDir.y*lightDir.y);
//		lightDir.z = lerp(-lightDir.z, lightDir.z, radiusWithMinusZDirBit >= 0.0f);
//
//		float outerCosineConeAngle	= spotParam.z;
//		float innerCosineConeAngle	= spotParam.w;
//		
//		float3 vtxToLight = lightCenterWorldPos - input.worldPos;
//		float distanceOfLightWithVertex = length(vtxToLight);
//		float3 vtxToLightDir = normalize(vtxToLight);
//		float currentCosineConeAngle = dot(-vtxToLightDir, lightDir);
//
//		if( (distanceOfLightWithVertex < (radius * 1.5f)) && (outerCosineConeAngle < currentCosineConeAngle) )
//		{
//			float innerOuterAttenuation = saturate( (currentCosineConeAngle - outerCosineConeAngle) / (innerCosineConeAngle - outerCosineConeAngle));
//			innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
//			innerOuterAttenuation = innerOuterAttenuation * innerOuterAttenuation;
//			innerOuterAttenuation = lerp(innerOuterAttenuation, 1, innerCosineConeAngle < currentCosineConeAngle);
//
//			float4 lightColorWithLm = g_inputPointLightColorBuffer[lightIndex];
//			float lumen = lightColorWithLm.w * 12750.0f; //maximum lumen is 12750.0f
//
//			float plAttenuation = 1.0f / (distanceOfLightWithVertex * distanceOfLightWithVertex);
//			float totalAttenTerm = lumen * plAttenuation * innerOuterAttenuation;
//
//			float3 lightColor = lightColorWithLm.rgb;
//			float3 lambert = albedo.rgb * saturate(dot(normal, lightDir));
//
//			radiosity += lambert * totalAttenTerm * lightColor * RenderSpotLightShadow(lightIndex, input.worldPos, distanceOfLightWithVertex / radius);
//			radiosity += material_emissionColor.rgb;
//		}
//	}
//
//	StoreRadiosity(radiosity, alpha, normal, voxelIdx);
//#endif
}
