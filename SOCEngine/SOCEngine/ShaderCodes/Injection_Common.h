//NOT_CREATE_META_DATA

#ifndef __SOC_INJECTION_COMMON_H__
#define __SOC_INJECTION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

RWTexture3D<uint>	OutVoxelColorMap	: register( u0 );
RWByteAddressBuffer	VoxelAlbedoRawBuf	: register( u1 );
RWByteAddressBuffer	VoxelNormalRawBuf	: register( u2 );
RWByteAddressBuffer	VoxelEmissionRawBuf	: register( u3 );

float4 GetColorInVoxelRawBuf(RWByteAddressBuffer voxelRawBuf, uint3 voxelIdx)
{
	uint bufferIndex = GetFlattedVoxelIndex(voxelIdx, gi_dimension);
	return RGBA8UintColorToFloat4(voxelRawBuf.Load(bufferIndex * 4));
}

float3 GetNormalInVoxelRawBuf(RWByteAddressBuffer voxelRawBuf, uint3 voxelIdx)
{
	uint bufferIndex = GetFlattedVoxelIndex(voxelIdx, gi_dimension);
	float3 normal = RGBA8UintColorToFloat4(voxelRawBuf.Load(bufferIndex * 4)).rgb;
	normal *= 2.0f; normal -= float3(1.0f, 1.0f, 1.0f);

	return normal;
}

void StoreRadiosity(RWTexture3D<uint> outVoxelMap, float3 radiosity, float alpha, float3 normal, uint3 voxelIdx)
{
#if 0
	float anisotropicNormals[6] = {
		-normal.x,
		 normal.x,
		-normal.y,
		 normal.y,
		-normal.z,
		 normal.z
	};

	for(int faceIndex=0; faceIndex<6; ++faceIndex)
	{
		uint3 index = voxelIdx;
		index.x += (faceIndex * gi_dimension);

		float rate = max(anisotropicNormals[faceIndex], 0.0f);
		float4 storeValue = float4(radiosity * rate, alpha);

		outVoxelMap[index] = Float4ColorToUint(storeValue);
	}
#else
	outVoxelMap[voxelIdx] = Float4ColorToUint(float4(radiosity, alpha));
#endif
}


#endif