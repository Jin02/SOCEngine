//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
//#include "TBDRInput.h"
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
RWTexture3D<uint> OutVoxelAlbedoTexture		: register( u0 );
RWTexture3D<uint> OutVoxelNormalTexture		: register( u1 );
RWTexture3D<uint> OutVoxelEmissionTexture	: register( u2 );
RWTexture3D<uint> OutInjectionColorMap		: register( u3 );
#endif

void StoreVoxelMapAtomicColorMax(RWTexture3D<uint> voxelMap, int3 idx, float4 value)
{
	uint newValue = Float4ColorToUint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while이나 for는 그래픽 드라이버에 따라 에러가 난다고 함 -ㅠ-;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;
		
		float4 curFlt4 = RGBA8UintColorToFloat4(currentStoredValue);
		float4 reCompute = curFlt4 + value; //걍 섞음. 뭐, 이래도 Max이긴 매한가지
		newValue = Float4ColorToUint(reCompute);

	}while(true);
}

uint encUnsignedNibble(uint m, uint n)
{
  return	(m & 0xFEFEFEFE)		|
			(n & 0x00000001)		|
			(n & 0x00000002) << 7U	|
			(n & 0x00000004) << 14U	|
			(n & 0x00000008) << 21U;
}

uint decUnsignedNibble(uint m)
{
  return	(m & 0x00000001)		|
			(m & 0x00000100) >> 7U	|
			(m & 0x00010000) >> 14U	|
			(m & 0x01000000) >> 21U;
}

void StoreVoxelMapAtomicColorAvg(RWTexture3D<uint> voxelMap, int3 idx, float4 value)
{
	uint newValue			= Float4ColorToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint counter = 0;
	[allow_uav_condition]do
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;
		prevStoredValue = currentStoredValue;

		float4 rval = ToFloat4(currentStoredValue & 0xFEFEFEFE);
		uint n = decUnsignedNibble(currentStoredValue);

		rval = rval * n + value;
		rval /= ++n;
		rval = round(rval / 2) * 2;
		newValue = encUnsignedNibble(ToUint(rval), n);

	}while(counter++ < 16);
}

//void StoreVoxelMapAtomicColorAvg(RWTexture3D<uint> voxelMap, int3 idx, float4 value)
//{
//	uint newValue = Float4ColorToUint(value);
//	uint prevStoredValue = 0;
//	uint currentStoredValue = 0;
//
//	// while이나 for는 그래픽 드라이버에 따라 에러가 난다고 함 -ㅠ-;
//	[allow_uav_condition]do
//	{
//		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);
//
//		if(prevStoredValue == currentStoredValue)
//			break;
//
//		prevStoredValue = currentStoredValue;
//
//		float4 curFlt4 = ToFloat4(currentStoredValue);
//		curFlt4.xyz = curFlt4.xyz * curFlt4.w;
//
//		float4 reCompute = curFlt4 + value;
//		reCompute.xyz /= (reCompute.w);
//
//		newValue = ToUint(reCompute);
//
//	}while(true);
//}

void StoreVoxelMapAtomicAddNormalOneValue(RWTexture3D<uint> voxelMap, int3 idx, float value)
{
	uint newValue = asuint(value);
	uint prevStoredValue = 0;
	uint currentStoredValue = 0;

	// while이나 for는 그래픽 드라이버에 따라 에러가 난다고 함 -ㅠ-;
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