//EMPTY_META_DATA

#include "Voxelization_Common.h"

RWTexture3D<uint> g_inputVoxelMap		: register(u0);
RWTexture3D<uint> g_outputMipMap		: register(u1);

float4 AlphaBlending(float4 front, float4 back)
{
// Pre-Multiplied Alpha Belnding?
	float3 blending = front.rgb + back.rgb * (1.0f - front.a);

//	°Á Æò±Õ¸¸ ±¸ÇÔ
	float alpha = (front.a + back.a) / 2.0f;
//	float t = 1.0f;

//	return float4(blending * alpha, alpha * t);
	return float4(blending * alpha, alpha);
}

uint AlphaBledningAnisotropicVoxelMap
	(	float4 front0,	float4 front1,	float4 front2,	float4 front3,
		float4 back0,	float4 back1,	float4 back2,	float4 back3	)
{
	float4 color0 = AlphaBlending(front0, back0);
	float4 color1 = AlphaBlending(front1, back1);
	float4 color2 = AlphaBlending(front2, back2);
	float4 color3 = AlphaBlending(front3, back3);

	return Float4ToUint( (color0 + color1 + color2 + color3) * 0.25f );
}

float4 GetColorFromVoxelMap(uint3 voxelIdx, uint faceIndex)
{
	voxelIdx.x += faceIndex;
	return UintToFloat4(g_inputVoxelMap[voxelIdx]);
}

[numthreads(MIPMAPPING_TILE_RES_HALF, MIPMAPPING_TILE_RES_HALF, MIPMAPPING_TILE_RES_HALF)]
void MipmapAnisotropicVoxelMapCS(uint3 globalIdx : SV_DispatchThreadID, 
								 uint3 localIdx	 : SV_GroupThreadID,
								 uint3 groupIdx	 : SV_GroupID)
{
	uint3 scaledGlobaIdx = globalIdx * 2;
	scaledGlobaIdx.x *= 6; //faceIdx

	uint3 sampleIdx[8] =
	{
		( scaledGlobaIdx + uint3(0, 0, 0) ),
		( scaledGlobaIdx + uint3(0, 0, 1) ),
		( scaledGlobaIdx + uint3(0, 1, 0) ),
		( scaledGlobaIdx + uint3(0, 1, 1) ),
		( scaledGlobaIdx + uint3(6, 0, 0) ),
		( scaledGlobaIdx + uint3(6, 0, 1) ),
		( scaledGlobaIdx + uint3(6, 1, 0) ),
		( scaledGlobaIdx + uint3(6, 1, 1) )
	};

	uint posx = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 0), GetColorFromVoxelMap(sampleIdx[1], 0), GetColorFromVoxelMap(sampleIdx[2], 0), GetColorFromVoxelMap(sampleIdx[3], 0),	//Fronts
		GetColorFromVoxelMap(sampleIdx[4], 0), GetColorFromVoxelMap(sampleIdx[5], 0), GetColorFromVoxelMap(sampleIdx[6], 0), GetColorFromVoxelMap(sampleIdx[7], 0)	//Backs
	);

	uint negx = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[4], 1), GetColorFromVoxelMap(sampleIdx[5], 1), GetColorFromVoxelMap(sampleIdx[6], 1), GetColorFromVoxelMap(sampleIdx[7], 1),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 1), GetColorFromVoxelMap(sampleIdx[1], 1), GetColorFromVoxelMap(sampleIdx[2], 1), GetColorFromVoxelMap(sampleIdx[3], 1)	//Backs
	);

	uint posy = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 2), GetColorFromVoxelMap(sampleIdx[1], 2), GetColorFromVoxelMap(sampleIdx[4], 2), GetColorFromVoxelMap(sampleIdx[5], 2),	//Fronts
		GetColorFromVoxelMap(sampleIdx[2], 2), GetColorFromVoxelMap(sampleIdx[3], 2), GetColorFromVoxelMap(sampleIdx[6], 2), GetColorFromVoxelMap(sampleIdx[7], 2)	//Backs
	);
	
	uint negy = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[2], 3), GetColorFromVoxelMap(sampleIdx[3], 3), GetColorFromVoxelMap(sampleIdx[6], 3), GetColorFromVoxelMap(sampleIdx[7], 3),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 3), GetColorFromVoxelMap(sampleIdx[1], 3), GetColorFromVoxelMap(sampleIdx[4], 3), GetColorFromVoxelMap(sampleIdx[5], 3)	//Backs
	);

	uint posz = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[0], 4), GetColorFromVoxelMap(sampleIdx[2], 4), GetColorFromVoxelMap(sampleIdx[4], 4), GetColorFromVoxelMap(sampleIdx[6], 4),	//Fronts
		GetColorFromVoxelMap(sampleIdx[1], 4), GetColorFromVoxelMap(sampleIdx[3], 4), GetColorFromVoxelMap(sampleIdx[5], 4), GetColorFromVoxelMap(sampleIdx[7], 4)	//Backs
	);
	
	uint negz = AlphaBledningAnisotropicVoxelMap(
		GetColorFromVoxelMap(sampleIdx[1], 5), GetColorFromVoxelMap(sampleIdx[3], 5), GetColorFromVoxelMap(sampleIdx[5], 5), GetColorFromVoxelMap(sampleIdx[7], 5),	//Fronts
		GetColorFromVoxelMap(sampleIdx[0], 5), GetColorFromVoxelMap(sampleIdx[2], 5), GetColorFromVoxelMap(sampleIdx[4], 5), GetColorFromVoxelMap(sampleIdx[6], 5)	//Backs
	);

	g_outputMipMap[uint3(globalIdx.x++, globalIdx.yz)]	= posx;
	g_outputMipMap[uint3(globalIdx.x++, globalIdx.yz)]	= negx;
	g_outputMipMap[uint3(globalIdx.x++, globalIdx.yz)]	= posy;
	g_outputMipMap[uint3(globalIdx.x++, globalIdx.yz)]	= negy;
	g_outputMipMap[uint3(globalIdx.x++, globalIdx.yz)]	= posz;
	g_outputMipMap[uint3(globalIdx.x, globalIdx.yz)]	= negz;
}