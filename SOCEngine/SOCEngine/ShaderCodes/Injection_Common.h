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

RWTexture3D<uint> OutVoxelColorTexture									: register( u0 );

float4 GetColor(Texture3D<float4> voxelTexture, uint3 voxelIdx, float3 dir, uint cascade)
{
	uint dimension = (uint)GetDimension();

	uint3 idx = voxelIdx;
	idx.y += cascade * dimension;

#if defined(USE_ANISOTROPIC_VOXELIZATION)
	float4 colorAxisX = (dir.x > 0.0f) ? 
		voxelTexture.Load(int4(idx.x + (0 * dimension), idx.yz, 0)) :
		voxelTexture.Load(int4(idx.x + (1 * dimension), idx.yz, 0));

	float4 colorAxisY = (dir.y > 0.0f) ?
		voxelTexture.Load(int4(idx.x + (2 * dimension), idx.yz, 0)) :
		voxelTexture.Load(int4(idx.x + (3 * dimension), idx.yz, 0));
	
	float4 colorAxisZ = (dir.z > 0.0f) ?
		voxelTexture.Load(int4(idx.x + (4 * dimension), idx.yz, 0)) :
		voxelTexture.Load(int4(idx.x + (5 * dimension), idx.yz, 0));

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
#else
	return voxelTexture.Load(int4(idx, 0));
#endif
}

#if defined(USE_ANISOTROPIC_VOXELIZATION)
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
#else
float3 GetNormal(Texture3D<float4> voxelNormalMap, uint3 voxelIdx, float3 dir, uint cascade)
{
	float3 normal = GetColor(voxelNormalMap, voxelIdx, dir, cascade).xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}
#endif

#endif

void StoreRadiosity(float3 radiosity, float alpha, float3 normal, uint3 voxelIdx)
{
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

	uint dimension = (uint)GetDimension();
	voxelIdx.y += voxelization_currentCascade * dimension;

	if(any(radiosity > 0.0f))
	{
#if defined(USE_ANISOTROPIC_VOXELIZATION)
		for(int faceIndex=0; faceIndex<6; ++faceIndex)
		{
			voxelIdx.x += (faceIndex * dimension);
			StoreVoxelMapAtomicColorMax(OutVoxelColorTexture, voxelIdx, float4(radiosity * max(anisotropicNormals[faceIndex], 0.0f), alpha));
		}
#else
		StoreVoxelMapAtomicColorMax(OutVoxelColorTexture, voxelIdx, float4(radiosity, alpha));
#endif
	}
}

#endif