//EMPTY_META_DATA

//#define USE_VOXEL_CONE_TRACING_TEST
//

#define VOXEL_CONE_TRACING

#include "GBufferParser.h"
#include "GICommon.h"
#include "TBDRInput.h"

Texture3D<float4>	VoxelMap							: register( t29 );

RWTexture2D<float4>	OutIndirectColorMap					: register( u0 );
SamplerState		linearSampler						: register( s0 );

#define MAXIMUM_CONE_COUNT				6
#define SAMPLE_START_OFFSET_RATE		1.2f

#define AMBIENT_OCCLUSION_K				8.0f

#define SPECULAR_OCCLUSION				0.95f
#define DIFFUSE_OCCLUSION				0.95f

#define DIFFUSE_SAMPLING_COUNT			32
#define SPECULAR_SAMPLING_COUNT			64

// ���� ������ ������ ������ ���� �Ʒ� �� ��������.
// http://simonstechblog.blogspot.kr/2013/01/implementing-voxel-cone-tracing.html
static const float3 ConeDirLS[MAXIMUM_CONE_COUNT] = //Cone Direction In Local Space
{
	float3(0.0f, 1.0f, 0.0f),
	float3(0.0f, 0.5f, 0.866025f),
	float3(0.823639f, 0.5f, 0.267617f),
	float3(0.509037f, 0.5f, -0.7006629f),
	float3(-0.50937f, 0.5f, -0.7006629f),
	float3(-0.823639f, 0.5f, 0.267617f)
};

// �̰͵� ���� ��ũ ����
static const float ConeWeights[MAXIMUM_CONE_COUNT] =
{
	PI / 4.0f,		//45 degree
	3 * PI / 20.0f,	//27 degree
	3 * PI / 20.0f,	//27 degree
	3 * PI / 20.0f,	//27 degree
	3 * PI / 20.0f,	//27 degree
	3 * PI / 20.0f,	//27 degree
};


float3 GetAnisotropicVoxelUV(float3 worldPos, uniform uint faceIdx, float3 bbMin)
{
	float3 uv = (worldPos - bbMin) / GetVoxelizationSize();
	uv.x = (uv.x + (float)faceIdx) * rcp(6.0f);

	return uv;
}

float3 GetVoxelUV(float3 worldPos, float3 bbMin)
{
	float3 uv = (worldPos - bbMin) / GetVoxelizationSize();
	return uv;
}

float4 SampleAnisotropicVoxelTex
	(float3 samplePos, float3 dir, float lod)
{
	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, gi_startCenterWorldPos);

#ifdef USE_ANISOTROPIC_INJECTION_MAP
	uint3 dirIdx;
	dirIdx.x = (dir.x < 0.0f) ? 0 : 1;
	dirIdx.y = (dir.y < 0.0f) ? 2 : 3;
	dirIdx.z = (dir.z < 0.0f) ? 4 : 5;

	float4 colorAxisX = VoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.x, bbMin), lod);
	float4 colorAxisY = VoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.y, bbMin), lod);
	float4 colorAxisZ = VoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.z, bbMin), lod);

	dir = abs(dir);
	float4 result = ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));

	return result;
#else
	return VoxelMap.SampleLevel(linearSampler, GetVoxelUV(samplePos, bbMin), lod);
#endif
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngleRadian)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngleRadian));
	return max(mip, 0);
}

float4 TraceCone(float3 worldPos, float3 worldNormal, float3 dir, float halfConeAngleRad, uniform float occlusionBias, uniform uint sampleCount)
{
	float currLength		= 0.0f;
	float3 samplePos		= worldPos + worldNormal * gi_voxelSize * SAMPLE_START_OFFSET_RATE;
	float3 sampleStartPos	= samplePos;// + worldNormal * gi_initVoxelSize * 2.0f;

	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, gi_startCenterWorldPos);

	float4 colorAccumInCone	= float4(0.0f, 0.0f, 0.0f, 0.0f); // w is occulusion
	float aoAccumInCone		= 0.0f;

	for(uint i=0; i<sampleCount; ++i)
	{
		float voxelSize		= gi_voxelSize;
		float mipLevel		= ComputeDistanceLOD(voxelSize, currLength, halfConeAngleRad);
		
		float4 sampleColor	= SampleAnisotropicVoxelTex(samplePos, dir, mipLevel);

		colorAccumInCone	+= (1.0f - colorAccumInCone.a) * sampleColor;
		aoAccumInCone		+= sampleColor.a * (1.0f / (1.0f + AMBIENT_OCCLUSION_K * currLength));

		currLength			= max(currLength / (1.0f - halfConeAngleRad), currLength + voxelSize);
		samplePos			= sampleStartPos + dir * currLength;

		if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
			samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
			samplePos.z < bbMin.z || samplePos.x >= bbMax.x ||
			colorAccumInCone.a >= occlusionBias )
			break;
	}

	return float4(colorAccumInCone.rgb, aoAccumInCone);
}

float3 SpecularVCT(float3 worldPos, float3 worldNormal, float halfConeAngleRad)
{
	float3 viewDir			= normalize(gi_startCenterWorldPos - worldPos);
	float3 reflectDir		= reflect(-viewDir, worldNormal);

	float4 colorAccum = TraceCone(worldPos, worldNormal, reflectDir, halfConeAngleRad, SPECULAR_OCCLUSION, SPECULAR_SAMPLING_COUNT);
	return colorAccum.rgb;
}

float3 DiffuseVCT(float3 worldPos, float3 worldNormal)
{
	float3 up		= (worldNormal.y * worldNormal.y) > 0.95f ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right	= cross(worldNormal, up);
	up				= cross(worldNormal, right);

	const float halfConeAngleRad	= DEG_2_RAD(60.0f) * 0.5f;
	float4	colorAccum				= float4(0.0f, 0.0f, 0.0f, 0.0f); // w is ao

	[unroll]
	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		float3 dir = normalize(worldNormal + ConeDirLS[coneIdx].x * right + ConeDirLS[coneIdx].z * up);

		float4 colorAccumInCone = TraceCone(worldPos, worldNormal, dir, halfConeAngleRad, DIFFUSE_OCCLUSION, DIFFUSE_SAMPLING_COUNT);
		colorAccum	+= colorAccumInCone * ConeWeights[coneIdx];
	}

	return colorAccum.rgb;
}

[numthreads(VOXEL_CONE_TRACING_TILE_RES, VOXEL_CONE_TRACING_TILE_RES, 1)]
void VoxelConeTracingCS(uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	Surface surface;
	ParseGBufferSurface(surface, globalIdx.xy, 0);

	float3 diffuseVCT	= DiffuseVCT(surface.worldPos, surface.normal);

	float halfConeAngle =	(sin(1.7f * sqrt( pow(surface.roughness, 1.5f) )) +			// �׳�.. roughness�� ������ ������ �������ش�.
							0.2f * sin(surface.roughness * surface.roughness)) * 0.5f;	// ���߿� �ذ� ����� ã���� ���ľ��Ѵ�.

	float3 specularVCT	= SpecularVCT(surface.worldPos, surface.normal, halfConeAngle);

	float3 indirectDiffuse	= diffuseVCT	* surface.albedo;
	float3 indirectSpecular	= specularVCT	* surface.specular;

	float3 indirectColor	= max(indirectDiffuse + indirectSpecular, 0.0f);


#if (MSAA_SAMPLES_COUNT > 1) // MSAA only 4x
	uint2 scaledGlobalIdx = globalIdx.xy * 2;

	uint2 texIndex[4] =
	{
		scaledGlobalIdx + uint2(0, 0),
		scaledGlobalIdx + uint2(0, 1),
		scaledGlobalIdx + uint2(1, 0),
		scaledGlobalIdx + uint2(1, 1)
	};

	OutIndirectColorMap[ texIndex[0] ] = indirectColor;

	[unroll] for(uint i=1; i<4; ++i)
	{
		Surface sampledSurface;
		ParseGBufferSurface(sampledSurface, globalIdx.xy, i);
		
		float3 sampledIndirectDiffuse	= diffuseVCT	* sampledSurface.albedo;
		float3 sampledIndirectSpecular	= specularVCT	* sampledSurface.specular;
		float3 sampledIndirectColor		= max(sampledIndirectDiffuse + sampledIndirectSpecular, 0.0f);

		OutIndirectColorMap[ texIndex[i] ] = float4(sampledIndirectColor, 1.0f);
	}
#else
	OutIndirectColorMap[globalIdx.xy] = float4(indirectColor, 1.0f);
#endif
}