//EMPTY_META_DATA

#ifndef __SOC_INJECTION_COMMON_H__
#define __SOC_INJECTION_COMMON_H__

#include "DynamicLightingCommon.h"
#include "PhysicallyBased_Common.h"
#include "Voxelization_Common.h"

#ifdef USE_SHADOW_INVERTED_DEPTH

cbuffer Injection_Info_CB : register(b7)
{
	matrix injection_volumeProj;
};

Texture3D<float4>	g_inputAnistropicVoxelAlbedoTexture		: register( t26 );
Texture3D<float>	g_inputAnistropicVoxelNormalTexture		: register( t27 );
Texture3D<float4>	g_inputAnistropicVoxelEmissionTexture	: register( t28 );

struct DSLightInvVPVMat
{
	matrix invMat;
};
struct PLightInvVPVMat
{
	matrix invMat[6];
};

StructuredBuffer<DSLightInvVPVMat>	g_inputDirectionalLightShadowInvVPVMatBuffer	: register( t29 );
StructuredBuffer<PLightInvVPVMat>	g_inputPointLightShadowInvVPVMatBuffer			: register( t30 );
StructuredBuffer<DSLightInvVPVMat>	g_inputSpotLightShadowInvVPVMatBuffer			: register( t31 );

RWTexture3D<uint> OutAnistropicVoxelColorTexture	: register( u0 );

float4 GetColor(Texture3D<float4> anisotropicVoxelTexture, uint3 voxelIdx, float3 dir, uint cascade)
{
	uint3 idx = voxelIdx;
	idx.y += cascade * voxelization_dimension;

	float4 colorAxisX = (dir.x > 0.0f) ? 
		anisotropicVoxelTexture.Load(int4(idx, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + voxelization_dimension, idx.yz, 0));

	float4 colorAxisY = (dir.y > 0.0f) ?
		anisotropicVoxelTexture.Load(int4(idx.x + (voxelization_dimension * 2), idx.yz, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + (voxelization_dimension * 3), idx.yz, 0));
	
	float4 colorAxisZ = (dir.z > 0.0f) ?
		anisotropicVoxelTexture.Load(int4(idx.x + (voxelization_dimension * 4), idx.yz, 0)) :
		anisotropicVoxelTexture.Load(int4(idx.x + (voxelization_dimension * 5), idx.yz, 0));

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
}

float3 GetNormal(Texture3D<float> anisotropicVoxelNormalMap, uint3 voxelIdx, float3 dir, uint cascade)
{
	uint3 idx = voxelIdx;
	idx.y += cascade * voxelization_dimension;

	float normalAxisX = (dir.x > 0.0f) ? 
		anisotropicVoxelNormalMap.Load(int4(idx, 0)) :
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + voxelization_dimension, idx.yz, 0));

	float normalAxisY = (dir.y > 0.0f) ?
		anisotropicVoxelNormalMap.Load(int4(idx.x + (voxelization_dimension * 2), idx.yz, 0))	:
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + (voxelization_dimension * 3), idx.yz, 0));
	
	float normalAxisZ = (dir.z > 0.0f) ?
		anisotropicVoxelNormalMap.Load(int4(idx.x + (voxelization_dimension * 4), idx.yz, 0))	:
	   -anisotropicVoxelNormalMap.Load(int4(idx.x + (voxelization_dimension * 5), idx.yz, 0));

	return normalize( float3(normalAxisX, normalAxisY, normalAxisZ) );
}

float3 GetVoxelCenterPos(uint3 voxelIdx)
{
	float3 voxelCenter;

	voxelCenter.x = voxelIdx.x * voxelization_voxelSize + voxelization_voxelSize * 0.5f + voxelization_minPos.x;
	voxelCenter.y = voxelIdx.y * voxelization_voxelSize + voxelization_voxelSize * 0.5f + voxelization_minPos.y;
	voxelCenter.z = voxelIdx.z * voxelization_voxelSize + voxelization_voxelSize * 0.5f + voxelization_minPos.z;
	
	return voxelCenter;
}

#endif

#endif