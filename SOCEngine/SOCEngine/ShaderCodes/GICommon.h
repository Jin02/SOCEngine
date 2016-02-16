#ifndef __SOC_GI_COMMON_H__
#define __SOC_GI_COMMON_H__

#include "ShaderCommon.h"

#ifndef VOXEL_CONE_TRACING

Texture3D<float4>	g_inputVoxelAlbedoTexture	: register( t29 );
Texture3D<float4>	g_inputVoxelEmissionTexture	: register( t31 );
Texture3D<float4>	g_inputVoxelNormalTexture	: register( t30 );

cbuffer GIInfoCB : register( b6 )
#else
cbuffer GIInfoCB : register( b1 )
#endif
{
	// High 16 bit is cascade
	// Low bits is voxel dimension
	uint	gi_maxCascadeWithVoxelDimensionPowOf2;

	float	gi_initVoxelSize;
	float	gi_initWorldSize;
	float	gi_maxMipLevel;
}

uint GetMaximumCascade()
{
	return (gi_maxCascadeWithVoxelDimensionPowOf2 >> 16);
}

uint ComputeCascade(float3 worldPos, float3 cameraWorldPos)
{
	//x, y, z 축 중에 가장 큰걸 고름
	float	dist = max(	abs(worldPos.x - cameraWorldPos.x),
						abs(worldPos.y - cameraWorldPos.y));
			dist = max(	dist,
						abs(worldPos.z - cameraWorldPos.z) );

	return (uint)sqrt(dist / (gi_initWorldSize * 0.5f));
}

float GetVoxelizeSize(uint cascade)
{
	float cascadeScale = float(cascade + 1);
	return gi_initWorldSize * (cascadeScale * cascadeScale);
}

void ComputeVoxelizationBound(out float3 outBBMin, out float3 outBBMax, uint cascade, float3 cameraWorldPos)
{
	float cascadeScale = float(cascade + 1);

	float worldSize		= GetVoxelizeSize(cascade);
	float halfWorldSize	= worldSize * 0.5f;

	outBBMin = cameraWorldPos - halfWorldSize.xxx;
	outBBMax = outBBMin + worldSize.xxx;
}

float GetDimension()
{
	return float(1 << (gi_maxCascadeWithVoxelDimensionPowOf2 & 0x0000ffff));
}

float ComputeVoxelSize(uint cascade)
{
	float worldSize = GetVoxelizeSize(cascade);
	float dimension = GetDimension();

	return worldSize / dimension;
}

float3 GetVoxelCenterPos(uint3 voxelIdx, float3 bbMin, float voxelSize)
{
	float3 voxelCenter;

	voxelCenter.x = (voxelIdx.x * voxelSize) + (voxelSize * 0.5f) + bbMin.x;
	voxelCenter.y = (voxelIdx.y * voxelSize) + (voxelSize * 0.5f) + bbMin.y;
	voxelCenter.z = (voxelIdx.z * voxelSize) + (voxelSize * 0.5f) + bbMin.z;

	return voxelCenter;
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

void StoreVoxelMapAtomicColorAvgNibble(RWTexture3D<uint> voxelMap, int3 idx, float4 value, uniform bool useLimit)
{
	uint newValue			= Float4ColorToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint count = 0;
	do//while( (!useLimit) || (count++ < 8) )
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

	}while(count++ < 16);
}

void StoreVoxelMapAtomicColorAvg(RWTexture3D<uint> voxelMap, int3 idx, float4 value, uniform bool useLimit)
{
	value *= 255.0f;

	uint newValue			= ToUint(value);
	uint prevStoredValue	= 0;
	uint currentStoredValue	= 0;

	uint count = 0;

	[allow_uav_condition]do//[allow_uav_condition]while(true)
	{
		InterlockedCompareExchange(voxelMap[idx], prevStoredValue, newValue, currentStoredValue);

		if(prevStoredValue == currentStoredValue)
			break;

		prevStoredValue = currentStoredValue;

		float4 curFlt4 = ToFloat4(currentStoredValue);
		curFlt4.xyz = (curFlt4.xyz * curFlt4.w);

		float4 reCompute = curFlt4 + value;
		reCompute.xyz /= reCompute.w;

		newValue = ToUint(reCompute);
	}while(true);
}

void StoreRadiosity(RWTexture3D<uint> outVoxelColorTexture, float3 radiosity, float alpha, float3 normal, uint3 voxelIdx, uint curCascade)
{
	float anisotropicNormals[6] = {
		-normal.x,
		 normal.x,
		-normal.y,
		 normal.y,
		-normal.z,
		 normal.z
	};

	uint dimension = (uint)GetDimension();
	voxelIdx.y += curCascade * dimension;

	if(any(radiosity > 0.0f))
	{
		for(int faceIndex=0; faceIndex<6; ++faceIndex)
		{
			uint3 index = voxelIdx;
			index.x += (faceIndex * dimension);

			float rate = max(anisotropicNormals[faceIndex], 0.0f);
			float4 storeValue = float4(radiosity * rate, alpha);

//			StoreVoxelMapAtomicColorAvg(outVoxelColorTexture, index, storeValue, true);
			outVoxelColorTexture[index] = Float4ColorToUint(storeValue);
		}
	}
}


#endif