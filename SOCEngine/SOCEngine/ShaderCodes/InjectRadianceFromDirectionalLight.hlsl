//EMPTY_META_DATA

#include "Injection_Common.h"

[numthreads(8, 8, 8)]
void InjectRadianceDirectionalLightsCS(	uint3 globalIdx	: SV_DispatchThreadID, 
										uint3 localIdx	: SV_GroupThreadID,
										uint3 groupIdx	: SV_GroupID)
{
	uint capacity			= GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadowAtlasCapacity);
	float perShadowMapRes	= (float)(1 << GetNumOfDirectionalLight(shadowGlobalParam_packedPowerOfTwoShadowAtlasSize));
	uint2 shadowMapSize		= uint2(perShadowMapRes * capacity, perShadowMapRes * capacity);

	uint shadowIndex	= globalIdx.x / (uint)perShadowMapRes;
	uint lightIndex		= g_inputDirectionalLightShadowIndexToLightIndex[shadowIndex];

	if( (globalIdx.x >= shadowMapSize.x || globalIdx.y >= shadowMapSize.y) ||
		(((uint)g_inputDirectionalLightShadowParams[lightIndex].index - 1) != shadowIndex) )
		return;

	float2 shadowMapPos	= float2(globalIdx.x % perShadowMapRes, globalIdx.y % perShadowMapRes);
	float2 shadowMapUV	= shadowMapPos.xy / perShadowMapRes;

	float depth = g_inputDirectionalLightShadowMapAtlas.Load(uint3(globalIdx.xy, 0), 0);

	float4 screenSpaceCoord = float4( shadowMapUV.x * 2.0f - 1.0f,
									-(shadowMapUV.y * 2.0f - 1.0f),
									depth, 1.0f );

	float4 voxelSpaceCoord = mul(screenSpaceCoord, injection_volumeProj);
	voxelSpaceCoord /= voxelSpaceCoord.w;
	voxelSpaceCoord.xyz = voxelSpaceCoord.xyz * 0.5f + 0.5f;
	uint3 voxelIdx = uint3(voxelSpaceCoord.xyz * voxelization_dimension);

	float4 lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
	float2 lightParam			= g_inputDirectionalLightParamBuffer[lightIndex];
	float3 lightDir				= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

	float4 albedo	= GetColor(AnistropicVoxelAlbedoTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float3 normal	= GetNormal(AnistropicVoxelNormalTexture, voxelIdx, lightDir, voxelization_currentCascade);
	float4 emission	= GetColor(AnistropicVoxelEmissionTexture, voxelIdx, lightDir, voxelization_currentCascade);

	float3 lightColor	= g_inputDirectionalLightColorBuffer[lightIndex].rgb;
	float3 lambert		= albedo.rgb * saturate(dot(normal, lightDir));
	float intensity		= g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;
	float4 outputColor	= float4(lambert * lightColor * intensity, albedo.a);

//	outputColor.rgb *= RenderDirectionalLightShadow(lightIndex, voxelCenterPos);

	float anisotropicNormals[6] = {
		 normal.x,
		-normal.x,
		 normal.y,
		-normal.y,
		 normal.z,
		-normal.z
	};

	voxelIdx.y += (float)voxelization_currentCascade * voxelization_dimension;

	for(int faceIndex=0; faceIndex<6; ++faceIndex)
	{
		voxelIdx.x += (float)faceIndex * voxelization_dimension;
		StoreVoxelMapAtomicColorMax(OutAnistropicVoxelColorTexture, voxelIdx, float4(outputColor.xyz * max(anisotropicNormals[faceIndex], 0.0f), outputColor.a));
	}
}