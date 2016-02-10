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
	uint3 dirIdx;
	dirIdx.x = (dir.x < 0.0f) ? 0 : 1;
	dirIdx.y = (dir.y < 0.0f) ? 2 : 3;
	dirIdx.z = (dir.z < 0.0f) ? 4 : 5;

	float4 colorAxisX = voxelTexture.Load(int4(idx.x + (dirIdx.x * dimension), idx.yz, 0));
	float4 colorAxisY = voxelTexture.Load(int4(idx.x + (dirIdx.y * dimension), idx.yz, 0));
	float4 colorAxisZ = voxelTexture.Load(int4(idx.x + (dirIdx.z * dimension), idx.yz, 0));

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
#else
	return voxelTexture.Load(int4(idx, 0));
#endif
}

float3 GetNormal(Texture3D<float4> voxelNormalMap, uint3 voxelIdx, uint cascade)
{
	voxelIdx.y += cascade * int(GetDimension());

	float3 normal = voxelNormalMap.Load(int4(voxelIdx, 0)).xyz;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}

#endif

#endif