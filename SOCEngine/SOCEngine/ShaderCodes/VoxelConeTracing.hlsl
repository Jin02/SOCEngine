//EMPTY_META_DATA

//#define USE_VOXEL_CONE_TRACING_TEST
//

#define VOXEL_CONE_TRACING

#include "GBufferParser.h"
#include "GICommon.h"
#include "TBDRInput.h"

Texture3D<float4>	SourceVoxelMap						: register( t29 );
Texture3D<float4>	MipmappedAnisotropicVoxelMap		: register( t30 );

RWTexture2D<float4>	OutIndirectColorMap					: register( u0 );
SamplerState		linearSampler						: register( s0 );

#define MAXIMUM_CONE_COUNT				6
#define SAMPLE_START_OFFSET_RATE			2.0f
#define AMBIENT_OCCLUSION_K				8.0f

// 콘의 각도에 관련한 데이터 값은 아래 글 참고했음.
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

// 이것도 위의 링크 참고
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
	float worldSize = GetVoxelizationSize();
	float3 uv = (worldPos - bbMin) * rcp(worldSize);
	uv.x = (uv.x + (float)faceIdx) * rcp(6.0f);

	return uv;
}

float3 GetVoxelUV(float3 worldPos, float3 bbMin)
{
	float3 uv = (worldPos - bbMin) / GetVoxelizationSize();
	return uv;
}

float4 SampleAnisotropicVoxelTex
	(float3 samplePos, float3 dir, float lod, float3 bbMin)
{
	float anisotropicLod = max(lod - 1.0f, 0.0f);

	uint3 dirIdx;
	dirIdx.x = (dir.x < 0.0f) ? 0 : 1;
	dirIdx.y = (dir.y < 0.0f) ? 2 : 3;
	dirIdx.z = (dir.z < 0.0f) ? 4 : 5;

	float4 colorAxisX = MipmappedAnisotropicVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.x, bbMin), anisotropicLod);
	float4 colorAxisY = MipmappedAnisotropicVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.y, bbMin), anisotropicLod);
	float4 colorAxisZ = MipmappedAnisotropicVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.z, bbMin), anisotropicLod);

	float3 weight = dir * dir;//abs(dir);
	float4 result = ((weight.x * colorAxisX) + (weight.y * colorAxisY) + (weight.z * colorAxisZ));

	float4 source = SourceVoxelMap.SampleLevel(linearSampler, GetVoxelUV(samplePos, bbMin), 0.0f);
	result = (lod < 1.0f) ? lerp(source, result, saturate(lod)) : result;

	return result;
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngleRadian)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngleRadian));
	return max(mip, 0);
}

bool IsInBound(float3 bbMin, float3 bbMax, float3 pos)
{
	return	bbMin.x <= pos.x && pos.x < bbMax.x &&
			bbMin.y <= pos.y && pos.y < bbMax.y &&
			bbMin.z <= pos.z && pos.z < bbMax.z;
}

float4 TraceCone(float3 origin, float3 normal, float3 dir, float halfConeAngleRad, uniform uint sampleCount, uniform bool useOcclusion)
{
	float voxelSize			= gi_voxelSize * SAMPLE_START_OFFSET_RATE;
	float currLength		= voxelSize;
	float3 sampleStartPos	= origin + normal * currLength;

	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, gi_startCenterWorldPos);

	float4 colorAccumInCone	= float4(0.0f, 0.0f, 0.0f, 0.0f); // w is occulusion
	float occlusion			= 0.0f;

	bool isInBound = IsInBound(bbMin, bbMax, origin);
	colorAccumInCone.a = (isInBound == false) ? 1.0f : 0.0f;

	for(uint i=0; (i<sampleCount) && (colorAccumInCone.a < 1.0f); ++i)
	{
		float3 samplePos	= sampleStartPos + dir * currLength;

		if(IsInBound(bbMin, bbMax, samplePos) == false)
			break;

		float diameter		= 2.0f * halfConeAngleRad * currLength;
		float mipLevel		= log2(diameter / voxelSize);
		
		float4 sampleColor	= SampleAnisotropicVoxelTex(samplePos, dir, mipLevel, bbMin);

		colorAccumInCone	+= sampleColor * (1.0f - colorAccumInCone.a);
		occlusion			+= sampleColor.a * (1.0f / (1.0f + AMBIENT_OCCLUSION_K * currLength));

		currLength			+= diameter * 0.5f;
	}

	occlusion = saturate(occlusion);
	colorAccumInCone.rgb *= useOcclusion ? (1.0f - occlusion) : 1.0f;

	return float4(colorAccumInCone.rgb, occlusion);
}

float3 SpecularVCT(float3 worldPos, float3 normal, float halfConeAngleRad, uniform uint sampleCount)
{
	float3 viewDir			= normalize(tbrParam_cameraWorldPosition - worldPos);
	float3 reflectDir		= reflect(-viewDir, normal);

	float4 colorAccum = TraceCone(worldPos, normal, reflectDir, halfConeAngleRad, sampleCount, false);
	return colorAccum.rgb;
}

float3 DiffuseVCT(float3 worldPos, float3 normal, uniform uint sampleCount)
{
	float3 up		= (normal.y * normal.y) > 0.95f ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right	= cross(normal, up);
	up				= cross(normal, right);

	const float halfConeAngleRad	= tan(DEG_2_RAD(60.0f) * 0.5f);
	float4	colorAccum				= float4(0.0f, 0.0f, 0.0f, 0.0f); // w is ao

	[unroll]
	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		float3 dir = normalize(normal + ConeDirLS[coneIdx].x * right + ConeDirLS[coneIdx].z * up);

		float4 colorAccumInCone = TraceCone(worldPos, normal, dir, halfConeAngleRad, sampleCount, true);
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

	float3 diffuseVCT	= DiffuseVCT(surface.worldPos, surface.normal, 128);

	// 1.73206060282 is tan( 60 degree )
	// 0.01745506492 is tan(  1 degree )
	float halfConeAngle	= lerp(1.73206060282f, 0.0174533f, (1.0f - surface.roughness));
	float3 specularVCT	= SpecularVCT(surface.worldPos, surface.normal, halfConeAngle, 256);

	float3 indirectDiffuse	= diffuseVCT	* surface.albedo;
	float3 indirectSpecular	= specularVCT	* surface.specular;

	float3 indirectColor	= max(indirectDiffuse + indirectSpecular, 0.0f);

	OutIndirectColorMap[globalIdx.xy] = float4(indirectColor, 1.0f);
}
