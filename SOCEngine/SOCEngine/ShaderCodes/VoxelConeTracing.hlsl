//EMPTY_META_DATA

#include "GBufferCommon.h"
#include "Voxelization_Common.h"
#include "AlphaBlending.h"

cbuffer GIInfoCB : register( b7 )
{
	uint	gi_maxCascade;
	uint	gi_voxelDimension;
	float	gi_initVoxelSize;

	float3	gi_initWorldMinPos;
	float	gi_initWorldSize;
}

Texture3D<float4> g_inputVoxelTexture		: register(t?);
Texture3D<float4> g_inputDirectColorTexture	: register(t?);
RWTexture2D<float4> g_outputIndirectMap		: register(u?);

#define MAXIMUM_CONE_COUNT				6
#define CONE_TRACING_BIAS				2.5f
#define CONE_TRACING_NEXT_STEP_RATIO	0.3333f
#define SPECULAR_OCCLUSION				0.95f
#define DIFFUSE_OCCLUSION				0.95f
#define DIFFUSE_AO_K					8.0f

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


float3 GetAnisotropicVoxelUV(float3 worldPos, uniform uint faceIdx, uint cascade)
{
//	float3 uv = (worldPos - gi_worldMinPos) / gi_worldSize;

	//uv.x = (uv.x * 6) + ( (float)faceIdx / voxelization_voxelizeSize );
	//uv.x *= rcp(6); //faceCount
	uv.x = (uv.x + (float)faceIdx) * rcp(6);
	uv.y = (uv.y + (float)cascade) * rcp(gi_maximumCascade);

	return uv;
}

float4 SampleAnisotropicVoxelTex
	(float3 samplePos, float3 dir, uint cascade, float lod)
{
	//defaultSampler is linearSampler

	float4 colorAxisX = (dir.x > 0.0f) ? 
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 0, cascade), lod) :
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 1, cascade), lod);

	float4 colorAxisY = (dir.y > 0.0f) ?
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 2, cascade), lod) :
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 3, cascade), lod);
	
	float4 colorAxisZ = (dir.z > 0.0f) ?
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 4, cascade), lod) :
		anisotropicVoxelTexture.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 5, cascade), lod);

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
}

uint ComputeCascade(float3 worldPos)
{
	//x, y, z 축 중에 가장 큰걸 고름
	float	dist = max(	abs(worldPos.x - tbrParam_cameraWorldPosition.x),
						abs(worldPos.y - tbrParam_cameraWorldPosition.y));
			dist = max(	dist,
						abs(worldPos.z - tbrParam_cameraWorldPosition.z) );

	float halfWorldSize = gi_initWorldSize / 2.0f;

	return (dist >= halfWorldSize) ? ceil( log2(dist/halfWorldSize) ) : 0;
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngleRadian)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngleRadian));
	return (mip < 0) ? 0 : mip;
}

float GetVoxelizeSize(uint cascade)
{
	return gi_initWorldSize * ( (float)((cascade+1) * (cascade+1)) );
}

void ComputeVoxelizationBound(out float3 outBBMin, out float3 outBBMax, uint cascade)
{
	float worldSize		= GetVoxelizeSize(cascade);
	float offset		= (worldSize / (float)(cascade+1)) / 2.0f;
	float3 worldMinPos	= tbrParam_cameraWorldPos - offset.xxx;

	outBBMin = worldMinPos * float(cascade + 1).xxx;
	outBBMax = outBBMin + worldSize.xxx;
}

float ComputeVoxelSize(uint cascade)
{
	float worldSize = GetVoxelizeSize(cascade);
	return worldSize / (float)gi_voxelDimension;
}

float3 SpecularVCT(float3 worldPos, float3 worldNormal, float roughness, float halfConeAngle, uniform float minMipLevel)
{
	float3 viewDir		= normalize(tbrParam_cameraWorldPos - worldPos);
	float3 reflectDir	= reflect(-viewDir, worldNormal);

	// reflect dir은 roughness가 0일때만 정확하다.
	// 그래서 그냥 roughness가 1로 갈수록 노멀쪽으로 값을 옮김
	// ndc2015 모두를 위한 물리 기반 렌더링의 이론과 활용, p69 참고
	float3 dir			= lerp(worldNormal, reflectDir, roughness);

	// w or a is occlusion
	float4 accumColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);

	float currLength	= gi_initVoxelSize * CONE_TRACING_BIAS;
	float3 samplePos	= worldPos + (dir * currLength);

	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, gi_maxCascade-1);

	for(uint i=0; i<512; ++i)
	{
		uint cascade = ComputeCascade(samplePos);
		
		if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
			samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
			samplePos.z < bbMin.z || samplePos.x >= bbMax.x )
			break;

		float oneVoxelSize = ComputeVoxelSize(cascade);
		float mipLevel = ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngle) + minMipLevel; //정확하진 않을듯. 임시용

		float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);
		accumColor = PremultipliedAlphaBlending(accumColor, sampleColor);

		if(accumColor.a >= SPECULAR_OCCLUSION)
			break;

		currLength += oneVoxelSize * CONE_TRACING_NEXT_STEP_RATIO;//pow(2.0f, mipLevel) * 0.5f;
		samplePos = worldPos + (dir * currLength);
	}

	return accumColor.xyz;
}

float3 DiffuseVCT(float3 worldPos, float3 worldNormal, uniform float minMipLevel)
{
	float3 up = (worldNormal.y > 0.95f) ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, worldNormal);
	up = cross(worldNormal, right);

	float4 accumColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //w is occlusion
	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		// worldNormal을 기준으로 Cone Dir Local Space 만큼 방향을 시프트 처리
		// 별거 없는데 공식 이해가 안가면 직접 그려보면서 이해하는걸 추천 -_-
		float3 dir = normalize( worldNormal + (ConeDirLS[coneIdx].x * right) + (ConeDirLS[coneIdx].z * up) );

		// w or a is occlusion
		float4 accumColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);

		float currLength	= 0//gi_initVoxelSize * CONE_TRACING_BIAS;
		float3 samplePos	= 0;//worldPos + (dir * currLength);
		float oneVoxelSize	= gi_initVoxelSize;// * CONE_TRACING_BIAS;
		
		float3 bbMin, bbMax;
		ComputeVoxelizationBound(bbMin, bbMax, gi_maxCascade-1);

		float halfConeAngleRad = ConeWeights[coneIdx] * 0.5f;
		float radiusRatio = tan(halfConeAngleRad);
		float4 accumDiffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
		for(uint i=0; i<64; ++i)
		{
			uint cascade = ComputeCascade(samplePos);

			if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
				samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
				samplePos.z < bbMin.z || samplePos.x >= bbMax.x )
				break;

			float lastLength = currLength;
			currLength += oneVoxelSize * CONE_TRACING_NEXT_STEP_RATIO;//pow(2.0f, mipLevel) * 0.5f;
			samplePos = worldPos + (dir * currLength);

			oneVoxelSize = ComputeVoxelSize(cascade);
			float mipLevel = ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngleRad);

			float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);

			// Interactive Indirect Illumination Using Voxel Cone Tracing by Cyril Crassin 논문 참고.
			// 작은 스텝상에서 사용되는 방법? 거의 diffuse라고 보면 될듯?
			float currentRadius = radiusRatio * currentLength;	// cascade라 결과 값이 너무 차이가 커서
																// 현재 voxel 크기 대신 반지름 값을 사용.
			float deltaLengthRatio = (currLength - lastLength) / currentRadius;
			sampleColor.a = 1.0f - pow(1.0f - sampleColor.a, deltaLengthRatio);
			accumDiffuse = PremultipliedAlphaBlending(accumDiffuse, sampleColor);

			// Cyril 논문에 있는 AO쪽 공식 참고함
			accumDiffuse.a += sampleColor.a * (1.0f / (1.0f + DIFFUSE_AO_K * currentLength));

			if(accumDiffuse.a >= DIFFUSE_OCCLUSION)
				break;
		}

		accumColor += accumDiffuse * ConeWeights[coneIdx];
	}

	return accumColor.xyz;
}

[numthreads(VOXEL_CONE_TRACING_TILE_RES, VOXEL_CONE_TRACING_TILE_RES, 1)]
void GlobalIlluminationCS(	uint3 globalIdx : SV_DispatchThreadID, 
							uint3 localIdx	: SV_GroupThreadID,
							uint3 groupIdx	: SV_GroupID)
{
	Surface surface;
	ParseGBufferSurface(surface, globalIdx.xy);

	float3 diffuseVCT	= DiffuseVCT(surface.worldPos, surface.worldNormal, 0.0f);

	float halfConeAngle = sin(1.5 * sqrt( pow(surface.roughness, 1.5) ));// 그냥.. roughness를 적당한 값으로 변경해준다.
																		 // 전혀 정확하지 않다. 그냥 적당히 값을 변경해준것 뿐이다.
																		 // 나중에 해결 방안을 찾으면 고쳐야한다.
	//const float minConeHalfAngle = 0.0436332313f;	// 2.5	degree
	//const float maxConeHalfAngle = 0.3490658500f;	// 20	degree
	//float halfConeAngle = lerp(minConeHalfAngle, maxConeHalfAngle, surface.roughness);
	float3 specularVCT	= SpecularVCT(surface.worldPos, surface.worldNormal, surface.roughness, halfConeAngle, 0.0f);

	//surface.metallic = 0.3f; //임시

#if (MSAA_SAMPLES_COUNT > 1) // MSAA only 4x
	uint2 scaledGlobalIdx = globalIdx.xy * 2;

	uint2 texIndex[4] =
	{
		globalIdx.xy + uint2(0, 0),
		globalIdx.xy + uint2(0, 1),
		globalIdx.xy + uint2(1, 0),
		globalIdx.xy + uint2(1, 1)
	};

	[unroll] for(uint i=0; i<4; ++i)
	{
		float4 directColor = g_inputDirectColorTexture.Load(texIndex[i], 0);
		float4 baseColor = directColor * (1.0f - surface.metallic);

		// Metallic 값을 이용해서 대충 섞는다.
		float4 indirectDiffuse = baseColor + diffuseVCT * directColor * surface.metallic;
		float4 indirectSpecular = baseColor + specularVCT * directColor * surface.metallic;

		g_outputIndirectMap[globalIdx.xy] = float4(indirectDiffuse + indirectSpecular, 1.0f);
	}

#else
	float4 directColor = g_inputDirectColorTexture.Load(globalIdx.xy, 0);
	float4 baseColor = directColor * (1.0f - surface.metallic);

	// Metallic 값을 이용해서 대충 섞는다.
	float4 indirectDiffuse = baseColor + diffuseVCT * directColor * surface.metallic;
	float4 indirectSpecular = baseColor + specularVCT * directColor * surface.metallic;

	g_outputIndirectMap[globalIdx.xy] = float4(indirectDiffuse + indirectSpecular, 1.0f);
#endif
}