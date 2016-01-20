//NOT_CREATE_META_DATA

#ifndef __SOC_INJECTION_COMMON_H__
#define __SOC_INJECTION_COMMON_H__

#define NEVER_USE_VSM

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "Voxelization_Common.h"
#include "GICommon.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

StructuredBuffer<DSLightVPMat>	g_inputDirectionalLightShadowInvVPVMatBuffer		: register( t32 );
StructuredBuffer<PLightVPMat>	g_inputPointLightShadowInvVPVMatBuffer				: register( t33 );
StructuredBuffer<DSLightVPMat>	g_inputSpotLightShadowInvVPVMatBuffer				: register( t34 );

RWTexture3D<uint> OutAnistropicVoxelColorTexture									: register( u0 );

float4 GetColor(Texture3D<float4> anisotropicVoxelTexture, uint3 voxelIdx, float3 dir, uint cascade)
{
	uint dimension = (uint)GetDimension();

	uint3 idx = voxelIdx;
	idx.y += cascade * dimension;

	float4 colorAxisX = (dir.x > 0.0f) ? 
		anisotropicVoxelTexture.Load(int4(idx.x + (0 * dimension), idx.yz, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + (1 * dimension), idx.yz, 0));

	float4 colorAxisY = (dir.y > 0.0f) ?
		anisotropicVoxelTexture.Load(int4(idx.x + (2 * dimension), idx.yz, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + (3 * dimension), idx.yz, 0));
	
	float4 colorAxisZ = (dir.z > 0.0f) ?
		anisotropicVoxelTexture.Load(int4(idx.x + (4 * dimension), idx.yz, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + (5 * dimension), idx.yz, 0));

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
}

float3 GetNormal(Texture3D<float> anisotropicVoxelNormalMap, uint3 voxelIdx, float3 dir, uint cascade)
{
	uint dimension = (uint)GetDimension();

	uint3 idx = voxelIdx;
	idx.y += cascade * dimension;

	float normalAxisX = (dir.x > 0.0f) ? 
		anisotropicVoxelNormalMap.Load(int4(idx.x + (0 * dimension), idx.yz, 0)) :
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + (1 * dimension), idx.yz, 0));

	float normalAxisY = (dir.y > 0.0f) ?
		anisotropicVoxelNormalMap.Load(int4(idx.x + (2 * dimension), idx.yz, 0))	:
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + (3 * dimension), idx.yz, 0));
	
	float normalAxisZ = (dir.z > 0.0f) ?
		anisotropicVoxelNormalMap.Load(int4(idx.x + (4 * dimension), idx.yz, 0))	:
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + (5 * dimension), idx.yz, 0));

	return normalize( float3(normalAxisX, normalAxisY, normalAxisZ) );
}

#endif

void StoreRadiosity(float3 radiosity, float alpha, float3 normal, uint3 voxelIdx)
{
	float anisotropicNormals[6] = {
		 normal.x,
		-normal.x,
		 normal.y,
		-normal.y,
		 normal.z,
		-normal.z
	};

	uint dimension = (uint)GetDimension();
	voxelIdx.y += voxelization_currentCascade * dimension;

	if(any(radiosity > 0.0f))
	{
		for(int faceIndex=0; faceIndex<6; ++faceIndex)
		{
			voxelIdx.x += (faceIndex * dimension);
			StoreVoxelMapAtomicColorMax(OutAnistropicVoxelColorTexture, voxelIdx, float4(radiosity * max(anisotropicNormals[faceIndex], 0.0f), alpha));
		}
	}
}

#endif