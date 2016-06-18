//EMPTY_META_DATA

#ifndef __SOC_VOXELIZATION_COMMON_H__
#define __SOC_VOXELIZATION_COMMON_H__

#include "DynamicLighting.h"
#include "PhysicallyBased_Common.h"
#include "GICommon.h"

cbuffer Voxelization_Info_CB : register( b5 )
{
	matrix	voxelization_toVoxelSpace;

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

void ComputeVoxelizationProjPos(
	out float4 position[3], out float3 worldPos[3], out uint axisIndex[3],
	in float3 inputLocalPos[3] )
{
	worldPos[0] = mul(float4(inputLocalPos[0], 1.0f), transform_world).xyz;
	worldPos[1] = mul(float4(inputLocalPos[1], 1.0f), transform_world).xyz;
	worldPos[2] = mul(float4(inputLocalPos[2], 1.0f), transform_world).xyz;

	float3 faceNormal = cross(normalize(worldPos[1] - worldPos[0]), normalize(worldPos[2] - worldPos[0]));

	float3 axis;
	axis.x = abs( dot(float3(1.0f, 0.0f, 0.0f), faceNormal) );
	axis.y = abs( dot(float3(0.0f, 1.0f, 0.0f), faceNormal) );
	axis.z = abs( dot(float3(0.0f, 0.0f, 1.0f), faceNormal) );
	
	float dominant = max(axis.x, max(axis.y, axis.z));

	float4 scale = float4( 2.0f,  2.0f, 1.0f, 1.0f);
	float4 shift = float4(-1.0f, -1.0f, 0.0f, 0.0f);

	float4 reCalcWorldPos;
	
	if(dominant == axis.x)
	{
		for(uint i=0; i<3; ++i)
		{
			axisIndex[i]	= 0u;
			
			reCalcWorldPos	= mul(float4(inputLocalPos[i].yzx, 1.0f), transform_world);
			position[i] 	= mul(reCalcWorldPos, voxelization_toVoxelSpace);
			position[i]	= position[i] * scale + shift;
		}
	}
	else if(dominant == axis.y)
	{
		for(uint i=0; i<3; ++i)
		{
			axisIndex[i]	= 1u;
		
			reCalcWorldPos	= mul(float4(inputLocalPos[i].zxy, 1.0f), transform_world);
			position[i]	= mul(reCalcWorldPos, voxelization_toVoxelSpace);
			position[i	= position[i] * scale + shift;
		}
	}
	else if(dominant == axis.z)
	{
		for(uint i=0; i<3; ++i)
		{
			axisIndex[i]	= 2u;

			reCalcWorldPos	= mul(float4(inputLocalPos[i].xyz, 1.0f), transform_world);
			position[i]	= mul(reCalcWorldPos, voxelization_toVoxelSpace);
			position[i]	= position[i] * scale + shift;
		}
	}
	
	/*
	[unroll]
	for(uint i=0; i<3; ++i)
	{
		float3 reCalcLocalPos;

		if(dominant == axis.x)		{ reCalcLocalPos = inputLocalPos[i].yzx; axisIndex[i] = 0u; }
		else if(dominant == axis.y)	{ reCalcLocalPos = inputLocalPos[i].zxy; axisIndex[i] = 1u; }	
		else if(dominant == axis.z)	{ reCalcLocalPos = inputLocalPos[i].xyz; axisIndex[i] = 2u; }

		float4 reCalcWorldPos = mul(float4(reCalcLocalPos, 1.0f), transform_world);
		position[i] = mul(reCalcWorldPos, voxelization_toVoxelSpace);
		position[i] = position[i] * scale + shift;
	}*/
}

void ComputeAlbedo(out float3 albedo, out float alpha, float2 uv)
{
	float4 diffuseTex	= diffuseMap.Sample(defaultSampler, uv);
	float3 mainColor	= GetMaterialMainColor().rgb;
	albedo				= lerp(mainColor, diffuseTex.rgb * mainColor, HasDiffuseMap());

	//float opacityMap	= 1.0f - opacityMap.Sample(defaultSampler, input.uv).x;
	alpha				= 1.0f;//lerp(1.0f, diffuseTex.a, HasDiffuseMap()) * opacityMap * GetMaterialMainColor().a;
}

void StoreVoxelMap(float4 albedoWithAlpha, float3 normal, int3 voxelIdx)
{
#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
	int dimension = int(GetDimension());
	if(all(0 <= voxelIdx) && all(voxelIdx < dimension))
	{
		int3 index = voxelIdx;
		index.y += voxelization_currentCascade * dimension;

		StoreVoxelMapAtomicColorAvg(OutVoxelAlbedoTexture,	index,	albedoWithAlpha, false);

		//StoreVoxelMapAtomicColorAvg(OutVoxelEmissionTexture,	index,	float4(material_emissionColor.xyz, 1.0f));

		float3 storeNormal = normal * 0.5f + 0.5f;
		StoreVoxelMapAtomicColorAvg(OutVoxelNormalTexture,	index,	float4(storeNormal, 1.0f), false);
	}
#endif
}

void ComputeVoxelIdx(out int3 outVoxelIdx, float3 position, uint axis)
{
	float3 scale	= float3(0.5f, 0.5f, 1.0f);
	float3 shift	= float3(1.0f, 1.0f, 0.0f);

	int dimension	= int( GetDimension() );
	int3 voxelIdx	= dimension * scale * (position + shift);

	if(axis == 0)		voxelIdx = voxelIdx.yzx;	// x
	else if(axis == 1)	voxelIdx = voxelIdx.zxy;	// y
	else if(axis == 2)	{}				// z

	outVoxelIdx	= voxelIdx;
}

void InjectRadianceFromDirectionalLight(int3 voxelIdx, float3 worldPos, float3 albedo, float alpha, float3 normal)
{
#if defined(USE_OUT_ANISOTROPIC_VOXEL_TEXTURES)
	float3 radiosity = float3(0.0f, 0.0f, 0.0f);

	uint dlShadowCount = GetNumOfDirectionalLight(shadowGlobalParam_packedNumOfShadows);
	for(uint dlShadowIdx=0; dlShadowIdx<dlShadowCount; ++dlShadowIdx)
	{
		ParsedShadowParam shadowParam;
		ParseShadowParam(shadowParam, g_inputDirectionalLightShadowParams[dlShadowIdx]);
		uint lightIndex			= shadowParam.lightIndex;

		float4 lightCenterWithDirZ	= g_inputDirectionalLightTransformWithDirZBuffer[lightIndex];
		float2 lightParam		= g_inputDirectionalLightParamBuffer[lightIndex];
		float3 lightDir			= -float3(lightParam.x, lightParam.y, lightCenterWithDirZ.w);

		float3 lightColor		= g_inputDirectionalLightColorBuffer[lightIndex].rgb;
		float3 lambert			= albedo.rgb * saturate(dot(normal, lightDir));
		float intensity			= g_inputDirectionalLightColorBuffer[lightIndex].a * 10.0f;

		radiosity += lambert * lightColor * intensity * RenderDirectionalLightShadow(lightIndex, worldPos);
		radiosity += GetMaterialEmissiveColor().rgb;
	}

	StoreRadiosity(OutInjectionColorMap, radiosity, alpha, normal, voxelIdx, voxelization_currentCascade);
#endif
}

void VoxelizationInPSStage(float3 normal, float2 uv, float3 position, float3 worldPos, uint axis)
{
	if( any(position < -1.0f) || any(position > 1.0f) )
		return;

	float	alpha;
	float3	albedo;
	ComputeAlbedo(albedo, alpha, uv);

	int3 voxelIdx;
	ComputeVoxelIdx(voxelIdx, position, axis);

	StoreVoxelMap(float4(albedo, alpha), normal, voxelIdx);
	InjectRadianceFromDirectionalLight(voxelIdx, worldPos, albedo, alpha, normal);
}

#endif
