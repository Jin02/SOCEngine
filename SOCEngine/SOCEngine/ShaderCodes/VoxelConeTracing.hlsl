//EMPTY_META_DATA

#define VOXEL_CONE_TRACING

#include "GBufferParser.h"
#include "AlphaBlending.h"
#include "GICommon.h"

Texture3D<float4> g_inputAnisotropicVoxelMap	: register(t29);
Texture2D<float4> g_inputDirectColorMap			: register(t30);

RWTexture2D<float4> g_outputIndirectMap			: register(u0);
SamplerState defaultSampler						: register(s0);

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
	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, cascade);

	float3 uv = (worldPos - bbMin) / GetVoxelizeSize(cascade);

	uv.x = (uv.x + (float)faceIdx) * rcp(6.0f);
	uv.y = (uv.y + (float)cascade) * rcp((float)GetMaximumCascade());

	return uv;
}

float4 SampleAnisotropicVoxelTex
	(float3 samplePos, float3 dir, uint cascade, float lod)
{
	//defaultSampler is linearSampler

	float4 colorAxisX = (dir.x > 0.0f) ? 
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 0, cascade), lod) :
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 1, cascade), lod);

	float4 colorAxisY = (dir.y > 0.0f) ?
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 2, cascade), lod) :
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 3, cascade), lod);
	
	float4 colorAxisZ = (dir.z > 0.0f) ?
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 4, cascade), lod) :
		g_inputAnisotropicVoxelMap.SampleLevel(defaultSampler, GetAnisotropicVoxelUV(samplePos, 5, cascade), lod);

	dir = abs(dir);
	return ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngleRadian)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngleRadian));
	return (mip < 0) ? 0 : mip;
}

float3 SpecularVCT(float3 worldPos, float3 worldNormal, float roughness, float halfConeAngle, uniform float minMipLevel)
{
	float3 viewDir		= normalize(tbrParam_cameraWorldPosition.xyz - worldPos);
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
	ComputeVoxelizationBound(bbMin, bbMax, GetMaximumCascade()-1);

	for(uint i=0; i<512; ++i)
	{
		uint cascade = ComputeCascade(samplePos);
		
		if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
			samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
			samplePos.z < bbMin.z || samplePos.x >= bbMax.x )
			break;

		float oneVoxelSize	= ComputeVoxelSize(cascade);
		float mipLevel		= ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngle) + minMipLevel; //��Ȯ���� ������. �ӽÿ�

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
	float3 up		= (worldNormal.y > 0.95f) ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right	= cross(up, worldNormal);
	up = cross(worldNormal, right);

	float4 accumColor = float4(0.0f, 0.0f, 0.0f, 0.0f); //w is occlusion
	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		// worldNormal�� �������� Cone Dir Local Space ��ŭ ������ �̵���Ŵ
		// ���� ���µ� ���� ���ذ� �Ȱ��� ���� �׷����鼭 �����ϴ°� ��õ -_-
		float3 dir = normalize( worldNormal + (ConeDirLS[coneIdx].x * right) + (ConeDirLS[coneIdx].z * up) );

		// w or a is occlusion
		float4 accumColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);

		float currLength	= 0;//gi_initVoxelSize * CONE_TRACING_BIAS;
		float3 samplePos	= float3(0.0f, 0.0f, 0.0f);//worldPos + (dir * currLength);
		float oneVoxelSize	= gi_initVoxelSize;// * CONE_TRACING_BIAS;
		
		float3 bbMin, bbMax;
		ComputeVoxelizationBound(bbMin, bbMax, GetMaximumCascade()-1);

		float halfConeAngleRad	= ConeWeights[coneIdx] * 0.5f;
		float radiusRatio		= tan(halfConeAngleRad);
		float4 accumDiffuse		= float4(0.0f, 0.0f, 0.0f, 0.0f);
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

			// Interactive Indirect Illumination Using Voxel Cone Tracing by Cyril Crassin �� ����.
			// ���� ���ܻ󿡼� ���Ǵ� ���? ���� diffuse��� ���� �ɵ�?
			float currentRadius = radiusRatio * currLength;	// cascade�� ��� ���� �ʹ� ���̰� Ŀ��
															// ���� voxel ũ�� ��� ������ ���� ���.
			float deltaLengthRatio = (currLength - lastLength) / currentRadius;
			sampleColor.a = 1.0f - pow(1.0f - sampleColor.a, deltaLengthRatio);
			accumDiffuse = PremultipliedAlphaBlending(accumDiffuse, sampleColor);

			// Cyril ���� �ִ� AO�� ���� ������
			accumDiffuse.a += sampleColor.a * (1.0f / (1.0f + DIFFUSE_AO_K * currLength));

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
	ParseGBufferSurface(surface, globalIdx.xy, 0);

	float3 diffuseVCT	= DiffuseVCT(surface.worldPos, surface.normal, 0.0f);

	float halfConeAngle = sin(1.5 * sqrt( pow(surface.roughness, 1.5) ));// �׳�.. roughness�� ������ ������ �������ش�.
																		 // ���� ��Ȯ���� �ʴ�. �׳� ������ ���� �������ذ� ���̴�.
																		 // ���߿� �ذ� ����� ã���� ���ľ��Ѵ�.
	//const float minConeHalfAngle = 0.0436332313f;	// 2.5	degree
	//const float maxConeHalfAngle = 0.3490658500f;	// 20	degree
	//float halfConeAngle = lerp(minConeHalfAngle, maxConeHalfAngle, surface.roughness);
	float3 specularVCT	= SpecularVCT(surface.worldPos, surface.normal, surface.roughness, halfConeAngle, 0.0f);

	//surface.metallic = 0.3f; //�ӽ�

#if (MSAA_SAMPLES_COUNT > 1) // MSAA only 4x
	uint2 scaledGlobalIdx = globalIdx.xy * 2;

	uint2 texIndex[4] =
	{
		scaledGlobalIdx + uint2(0, 0),
		scaledGlobalIdx + uint2(0, 1),
		scaledGlobalIdx + uint2(1, 0),
		scaledGlobalIdx + uint2(1, 1)
	};

	[unroll] for(uint i=0; i<4; ++i)
	{
		float4 directColor	= g_inputDirectColorTexture.Load( uint3(texIndex[i], 0) );
		float3 baseColor	= directColor.rgb * (1.0f - surface.metallic);
	
		// Metallic ���� �̿��ؼ� ���� ���´�.
		float3 indirectDiffuse	= baseColor + (diffuseVCT * directColor.rgb * surface.metallic);
		float3 indirectSpecular	= baseColor + (specularVCT * directColor.rgb * surface.metallic);

		g_outputIndirectMap[texIndex[i]] = float4(indirectDiffuse + indirectSpecular, 1.0f);
	}

#else
	float4 directColor	= g_inputDirectColorMap.Load( uint3(globalIdx.xy, 0) );
	float3 baseColor	= directColor.rgb * (1.0f - surface.metallic);

	// Metallic ���� �̿��ؼ� ���� ���´�.
	float3 indirectDiffuse	= baseColor + (diffuseVCT * directColor.rgb * surface.metallic);
	float3 indirectSpecular	= baseColor + (specularVCT * directColor.rgb * surface.metallic);

	g_outputIndirectMap[globalIdx.xy] = float4(indirectDiffuse + indirectSpecular, 1.0f);
#endif
}