//EMPTY_META_DATA

#define VOXEL_CONE_TRACING

#include "GBufferParser.h"
#include "GICommon.h"
#include "TBDRInput.h"

Texture3D<float4> g_inputVoxelMap						: register(t29);
Texture2D<float4> g_inputDirectColorMap					: register(t30);

RWTexture2D<float4> g_outIndirectColorMap				: register(u0);
SamplerState linearSampler								: register(s0);

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


float3 GetAnisotropicVoxelUV(float3 worldPos, uniform uint faceIdx, uint cascade, float3 bbMin)
{
	float3 uv = (worldPos - bbMin) / GetVoxelizeSize(cascade);

	uv.x = (uv.x + (float)faceIdx) * rcp(6.0f);
	uv.y = (uv.y + (float)cascade) * rcp((float)GetMaximumCascade());

	return uv;
}

float3 GetVoxelUV(float3 worldPos, uint cascade, float3 bbMin)
{
	float3 uv = (worldPos - bbMin) / GetVoxelizeSize(cascade);
	uv.y = (uv.y + (float)cascade) * rcp((float)GetMaximumCascade());

	return uv;
}

float4 SampleAnisotropicVoxelTex
	(float3 samplePos, float3 dir, uint cascade, float lod)
{
	float3 bbMin, bbMax;
	ComputeVoxelizationBound(bbMin, bbMax, cascade, tbrParam_cameraWorldPosition.xyz);

#ifdef USE_ANISOTROPIC_INJECTION_MAP
	uint3 dirIdx;
	dirIdx.x = (dir.x < 0.0f) ? 0 : 1;
	dirIdx.y = (dir.y < 0.0f) ? 2 : 3;
	dirIdx.z = (dir.z < 0.0f) ? 4 : 5;

	float4 colorAxisX = g_inputVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.x, cascade, bbMin), lod);
	float4 colorAxisY = g_inputVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.y, cascade, bbMin), lod);
	float4 colorAxisZ = g_inputVoxelMap.SampleLevel(linearSampler, GetAnisotropicVoxelUV(samplePos, dirIdx.z, cascade, bbMin), lod);

	dir = abs(dir);
	float4 result = ((dir.x * colorAxisX) + (dir.y * colorAxisY) + (dir.z * colorAxisZ));
	//result.rgb *= 8.0f;

	return result;
#else
	return g_inputVoxelMap.SampleLevel(linearSampler, GetVoxelUV(samplePos, cascade, bbMin), lod);
#endif
}

float ComputeDistanceLOD(float oneVoxelSize, float currLength, float halfConeAngleRadian)
{
	float mip = log2(currLength / oneVoxelSize * tan(halfConeAngleRadian));
	return max(mip, 0);
}

//float3 SpecularVCT(float3 worldPos, float3 worldNormal, float roughness, float halfConeAngle, uniform float minMipLevel)
//{
//	float3 viewDir		= normalize(tbrParam_cameraWorldPosition.xyz - worldPos);
//	float3 reflectDir	= reflect(-viewDir, worldNormal);
//
//	// reflect dir은 roughness가 0일때만 정확하다.
//	// 그래서 그냥 roughness가 1로 갈수록 노멀쪽으로 값을 옮김
//	// ndc2015 모두를 위한 물리 기반 렌더링의 이론과 활용, p69 참고
//	float3 dir			= lerp(worldNormal, reflectDir, roughness);
//
//	// w or a is occlusion
//	float4 accumColor	= float4(0.0f, 0.0f, 0.0f, 0.0f);
//
//	float currLength	= gi_initVoxelSize * CONE_TRACING_BIAS;
//	float3 samplePos	= worldPos + (dir * currLength);
//
//	float3 bbMin, bbMax;
//	ComputeVoxelizationBound(bbMin, bbMax, GetMaximumCascade()-1, tbrParam_cameraWorldPosition.xyz);
//
//	for(uint i=0; i<512; ++i)
//	{
//		uint cascade = ComputeCascade(samplePos, tbrParam_cameraWorldPosition.xyz);
//		
//		if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
//			samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
//			samplePos.z < bbMin.z || samplePos.x >= bbMax.x )
//			break;
//
//		float oneVoxelSize	= ComputeVoxelSize(cascade);
//		float mipLevel		= ComputeDistanceLOD(oneVoxelSize, currLength, halfConeAngle) + minMipLevel; //정확하진 않을듯. 임시용
//
//		float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);
//		//accumColor = PremultipliedAlphaBlending(accumColor, sampleColor);
//
//		if(accumColor.a >= SPECULAR_OCCLUSION)
//			break;
//
//		currLength += oneVoxelSize * CONE_TRACING_NEXT_STEP_RATIO;//pow(2.0f, mipLevel) * 0.5f;
//		samplePos = worldPos + (dir * currLength);
//	}
//
//	return accumColor.xyz;
//}

float3 DiffuseVCT(float3 worldPos, float3 worldNormal)
{
	float3 up = (worldNormal.y * worldNormal.y) > 0.95f ? float3(0.0f, 0.0f, 1.0f) : float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(worldNormal, up);
	up = cross(worldNormal, right);

	float4	colorAccum	= float4(0.0f, 0.0f, 0.0f, 0.0f);
	float	aoAccum		= 0.0f;

	for(uint coneIdx = 0; coneIdx < MAXIMUM_CONE_COUNT; ++coneIdx)
	{
		float3 sampleStartPos = worldPos + worldNormal * gi_initVoxelSize * 2.0f;
		float3 samplePos = sampleStartPos;

		const float offset = gi_initVoxelSize * 0.5f;
		float3 dir = normalize(worldNormal + ConeDirLS[coneIdx].x * right + ConeDirLS[coneIdx].z * up);
		samplePos += dir * offset;

		float halfConeAngleRad	= DEG_2_RAD(60.0f) * 0.5f;
		float currRadius		= halfConeAngleRad * offset;
		float currLength		= offset;

		float3 bbMin, bbMax;
		ComputeVoxelizationBound(bbMin, bbMax, GetMaximumCascade()-1, tbrParam_cameraWorldPosition.xyz);

		float4 colorAccumInCone	= float4(0.0f, 0.0f, 0.0f, 0.0f); // w is occulusion
		float aoAccumInCone		= 0.0f;

		for(uint i=0; i<64; ++i)
		{
			uint cascade	= ComputeCascade(samplePos, tbrParam_cameraWorldPosition.xyz);
			float voxelSize	= ComputeVoxelSize(cascade);
			float mipLevel	= ComputeDistanceLOD(voxelSize, currLength, halfConeAngleRad);

			float4 sampleColor = SampleAnisotropicVoxelTex(samplePos, dir, cascade, mipLevel);

			float prevLength = currLength;
			currLength	= max(currLength / (1.0f - halfConeAngleRad), currLength + voxelSize);
			samplePos	= sampleStartPos + dir * currLength;
			currRadius	= tan(halfConeAngleRad) * currLength;

			//sampleColor.a = 1.0f - pow(1.0f - sampleColor.a, ((currLength - prevLength) / currRadius));
			colorAccumInCone += (1.0f - colorAccumInCone.a) * sampleColor;

			if(	samplePos.x < bbMin.x || samplePos.x >= bbMax.x ||
				samplePos.y < bbMin.y || samplePos.x >= bbMax.x ||
				samplePos.z < bbMin.z || samplePos.x >= bbMax.x ||
				colorAccumInCone.a >= DIFFUSE_OCCLUSION )
				break;
		}

		colorAccum	+= colorAccumInCone * ConeWeights[coneIdx];
		aoAccum		+= aoAccumInCone * ConeWeights[coneIdx];
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

//	float halfConeAngle = sin(1.5 * sqrt( pow(surface.roughness, 1.5) ));// 그냥.. roughness를 적당한 값으로 변경해준다.
																		 // 전혀 정확하지 않다. 그냥 적당히 값을 변경해준것 뿐이다.
																		 // 나중에 해결 방안을 찾으면 고쳐야한다.
	//const float minConeHalfAngle = 0.0436332313f;	// 2.5	degree
	//const float maxConeHalfAngle = 0.3490658500f;	// 20	degree
	//float halfConeAngle = lerp(minConeHalfAngle, maxConeHalfAngle, surface.roughness);
	//float3 specularVCT	= SpecularVCT(surface.worldPos, surface.normal, surface.roughness, halfConeAngle, 0.0f);

	//surface.metallic = 0.3f; //임시

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
	
		// Metallic 값을 이용해서 대충 섞는다.
		float3 indirectDiffuse	= baseColor + (diffuseVCT * directColor.rgb * surface.metallic);
		float3 indirectSpecular	= baseColor + (specularVCT * directColor.rgb * surface.metallic);

		g_outIndirectColorMap[texIndex[i]] = float4(indirectDiffuse + indirectSpecular, 1.0f);
	}

	g_outIndirectColorMap[globalIdx.xy] = float4(0.0f, 0.0f, 1.0f, 1.0f);//directColor;//float4(indirectDiffuse + indirectSpecular, 1.0f);
#else
	float4 directColor	= g_inputDirectColorMap.Load( uint3(globalIdx.xy, 0) );
	//float3 baseColor	= directColor.rgb;// * (1.0f - surface.metallic);

	// Metallic 값을 이용해서 대충 섞는다.
	//float3 indirectDiffuse	= baseColor + (diffuseVCT * directColor.rgb * surface.metallic);
	//float3 indirectSpecular	= baseColor + (specularVCT * directColor.rgb * surface.metallic);
	//float3 indirectDiffuse	= baseColor + (diffuseVCT * directColor.rgb);
	//float3 indirectSpecular	= float3(0.0f, 0.0f, 0.0f);

	g_outIndirectColorMap[globalIdx.xy] = float4(diffuseVCT, 1.0f);
#endif
}