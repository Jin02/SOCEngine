//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

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

#endif