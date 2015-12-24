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

Texture3D<float4> g_inputVoxelTexture	: register(t?);
RWTexture2D<uint> g_outputIndirectMap	: register(u?);

#define MAXIMUM_CONE_COUNT				6
#define CONE_TRACING_BIAS				2.5f
#define CONE_TRACING_NEXT_STEP_RATIO	0.3333f
#define SPECULAR_OCCLUSION				0.95f
#define DIFFUSE_OCCLUSION				0.95f
#define DIFFUSE_AO_K					8.0f

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
	//x, y, z �� �߿� ���� ū�� ��
	float	dist = max(	abs(worldPos.x - tbrParam_cameraWorldPosition.x),
						abs(worldPos.y - tbrParam_cameraWorldPosition.y));
			dist = max(	dist,
						abs(worldPos.z - tbrParam_cameraWorldPosition.z) );

	float halfWorldSize = gi_initWorldSize / 2.0f;

	return (dist >= halfWorldSize) ? ceil( log2(dist/halfWorldSize) ) : 0;
}

// Unreal4�� ReflectionEnvironmentShared.usf�� �ִ�
// ComputeReflectionCaptureMipFromRoughness �̰���
float ComputeRoughnessLOD(float roughness)
{
#define REFLECTION_CAPTURE_ROUGHEST_MIP 1.0f
#define REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE 1.2f

	float levelFrom1x1 = REFLECTION_CAPTURE_ROUGHEST_MIP - REFLECTION_CAPTURE_ROUGHNESS_MIP_SCALE * log2(roughness);

	const float HardcodedNumCaptureArrayMips = 7;
	float mip = HardcodedNumCaptureArrayMips - 1 - LevelFrom1x1;
	return (mip < 0) ? 0 : mip;
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngle)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngle));
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

float4 SpecularVCT(float3 worldPos, float3 worldNormal, float roughness, float halfConeAngle, uniform float minMipLevel)
{
	float3 viewDir		= normalize(tbrParam_cameraWorldPos - worldPos);
	float3 reflectDir	= reflect(-viewDir, worldNormal);

	// reflect dir�� roughness�� 0�϶��� ��Ȯ�ϴ�.
	// �׷��� �׳� roughness�� 1�� ������ ��������� ���� �ű�
	// ndc2015 ��θ� ���� ���� ��� �������� �̷а� Ȱ��, p69 ����
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

		float distanceMip = ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngle);
		float roughnessMip = roughnessMip = ComputeRoughnessLOD(roughness);
		float mipLevel = min(roughnessMip, distanceMip) + minMipLevel; //���ľ���
	
		float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);
		accumColor = PremultipliedAlphaBlending(accumColor, sampleColor);

		if(accumColor.a >= SPECULAR_OCCLUSION)
			break;

		currLength += oneVoxelSize * CONE_TRACING_NEXT_STEP_RATIO;//pow(2.0f, mipLevel) * 0.5f;
		samplePos = worldPos + (dir * currLength);
	}

	return accumColor;
}

float4 DiffuseVCT(float3 worldPos, float3 worldNormal, uniform float minMipLevel)
{
	float3 up = (worldNormal.y > 0.95f) ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, worldNormal);
	up = cross(worldNormal, right);

	float4 accumColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //w is occlusion
	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		// worldNormal�� �������� Cone Dir Local Space ��ŭ ������ ����Ʈ ó��
		// ���� ���µ� ���� ���ذ� �Ȱ��� ���� �׷����鼭 �����ϴ°� ��õ -_-
		float3 dir = normalize( worldNormal + (ConeDirLS[coneIdx].x * right) + (ConeDirLS[coneIdx].z * up) );

		// w or a is occlusion
		float4 accumColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);

		float currLength	= 0//gi_initVoxelSize * CONE_TRACING_BIAS;
		float3 samplePos	= 0;//worldPos + (dir * currLength);
		float oneVoxelSize	= gi_initVoxelSize;// * CONE_TRACING_BIAS;
		
		float3 bbMin, bbMax;
		ComputeVoxelizationBound(bbMin, bbMax, gi_maxCascade-1);

		float radiusRatio = sin(ConeWeights[coneIdx] * 0.5f);
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
			float mipLevel = ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngle);

			float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);

			// Interactive Indirect Illumination Using Voxel Cone Tracing by Cyril Crassin �� ����.
			// ���� ���ܻ󿡼� ���Ǵ� ���? ���� diffuse��� ���� �ɵ�?
			float currentRadius = radiusRatio * currentLength;	// cascade�� �ʹ� ���߳��� ���� �� ���Ƽ�
																// ���� voxel ũ�� ��� �� ���� ���.
			float deltaLengthRatio = (currLength - lastLength) / currentRadius;
			sampleColor.a = 1.0f - pow(1.0f - sampleColor.a, deltaLengthRatio);
			accumDiffuse = PremultipliedAlphaBlending(accumDiffuse, sampleColor);

			// Cyril ���� �ִ� AO�� ���� ������
			accumDiffuse.a += sampleColor.a * (1.0f / (1.0f + DIFFUSE_AO_K * currentLength));

			if(accumDiffuse.a >= DIFFUSE_OCCLUSION)
				break;
		}

		accumColor += accumDiffuse * ConeWeights[coneIdx];
	}

	return accumColor;
}

[numthreads(VOXEL_CONE_TRACING_TILE_RES, VOXEL_CONE_TRACING_TILE_RES, 1)]
void GlobalIlluminationCS(	uint3 globalIdx : SV_DispatchThreadID, 
							uint3 localIdx	: SV_GroupThreadID,
							uint3 groupIdx	: SV_GroupID)
{
	Surface surface;
	ParseGBufferSurface(surface, globalIdx.xy);
}