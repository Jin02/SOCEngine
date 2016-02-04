//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

//#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

//#define USE_VOXELIZATION_BLOATING_POS
#define VOXELIZATION_BLOATING_RATIO 5.0f

cbuffer Voxelization_Info_CB : register( b5 )
{
	matrix	voxelization_vp_axisX;
	matrix	voxelization_vp_axisY;
	matrix	voxelization_vp_axisZ;
//	matrix	voxelization_worldToVoxel;

	float3	voxelization_minPos;
	uint	voxelization_currentCascade;
};

SamplerState defaultSampler			: register( s0 );

#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
RWTexture3D<uint> OutVoxelAlbedoTexture	: register( u0 );
RWTexture3D<uint> OutVoxelNormalTexture	: register( u1 );
RWTexture3D<uint> OutVoxelEmissionTexture	: register( u2 );
#endif

void StoreVoxelMapAtomicColorMax(RWTexture3D<uint> voxelMap, int3 idx, float4 value)
{
	uint newValue = Float4ColorToUint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while�̳� for�� �׷��� ����̹��� ���� ������ ���ٰ� �� -��-;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;
		
		float4 curFlt4 = RGBA8UintColorToFloat4(currentStoredValue);
		float4 reCompute = curFlt4 + value; //�� ����. ��, �̷��� Max�̱� ���Ѱ���
		newValue = Float4ColorToUint(reCompute);

	}while(true);
}

void StoreVoxelMapAtomicColorAvg(RWTexture3D<uint> voxelMap, int3 idx, float4 value)
{
	uint newValue = Float4ColorToUint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while�̳� for�� �׷��� ����̹��� ���� ������ ���ٰ� �� -��-;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;

		float4 curFlt4 = ToFloat4(currentStoredValue);
		curFlt4.xyz = curFlt4.xyz * curFlt4.w;

		float4 reCompute = curFlt4 + value;
		reCompute.xyz /= (reCompute.w);

		newValue = ToUint(reCompute);

	}while(true);
}

void StoreVoxelMapAtomicAddNormalOneValue(RWTexture3D<uint> voxelMap, int3 idx, float value)
{
	uint newValue = asuint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while�̳� for�� �׷��� ����̹��� ���� ������ ���ٰ� �� -��-;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);
		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;
		newValue = asuint(asfloat(currentStoredValue) + value);

	}while(true);
}

#endif