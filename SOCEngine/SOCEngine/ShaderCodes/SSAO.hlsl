//EMPTY_META_DATA

#define USE_TBR_PARAM
#include "FullScreenShader.h"
#include "ShaderCommon.h"
#include "TBDRInput.h"

cbuffer SSAOParam : register(b1)
{
	float2		ssao_stepUV;
	uint		ssao_sampleCountWithScale;
	uint		ssao_skipDistWithOccluedRate;
};

uint GetSamplingCount()
{
	return ssao_sampleCountWithScale & 0xffff;
}

float GetScale()
{
	return f16tof32(ssao_sampleCountWithScale >> 16);
}

float GetSkipDist()
{
	return f16tof32(ssao_skipDistWithOccluedRate & 0xffff);
}

float GetOccluedRate()
{
	return f16tof32(ssao_skipDistWithOccluedRate >> 16);
}

Texture2D<float4>	InputSceneMap			: register( t0 );
SamplerState		PointSampler			: register( s0 );

float4 SSAO_InFullScreen_PS(PS_INPUT input) : SV_Target
{
#if (MSAA_SAMPLES_COUNT > 1)
	float depth		= GBufferDepth.Load(input.position.xy, 0).r;
#else
	float depth		= GBufferDepth.Sample(PointSampler, input.uv).r;
#endif
	float4 H		= float4(input.uv.x * 2.0f - 1.0f, (1.0f - input.uv.y) * 2.0f - 1.0f, depth, 1.0f);
	float4 viewPos = mul(H, tbrParam_invProjMat);
	viewPos /= viewPos.w;

#if (MSAA_SAMPLES_COUNT > 1)
	float3 normal	= GBufferNormal_roughness.Load(input.position.xy, 0).rgb * 2.0f - 1.0f;
#else
	float3 normal	= GBufferNormal_roughness.Sample(PointSampler, input.uv).rgb * 2.0f - 1.0f;
#endif

	const float2 samples[24] = {   
							float2(-1.0f, -1.0f),	//tl
							float2( 0.0f, -1.0f),	//t
							float2(-1.0f,  1.0f),	//tr
							float2( 1.0f,  0.0f),	//r
							float2( 1.0f,  1.0f),	//br
							float2( 0.0f,  1.0f),	//b
							float2(-1.0f,  1.0f),	//bl
							float2(-1.0f,  0.0f),	//l

							float2(-2.0f, -2.0f),	//tl
							float2( 0.0f, -2.0f),	//t
							float2(-2.0f,  2.0f),	//tr
							float2( 2.0f,  0.0f),	//r
							float2( 2.0f,  2.0f),	//br
							float2( 0.0f,  2.0f),	//b
							float2(-2.0f,  2.0f),	//bl
							float2(-2.0f,  0.0f),	//l

							float2(-3.0f, -3.0f),	//tl
							float2( 0.0f, -3.0f),	//t
							float2(-3.0f,  3.0f),	//tr
							float2( 3.0f,  0.0f),	//r
							float2( 3.0f,  3.0f),	//br
							float2( 0.0f,  3.0f),	//b
							float2(-3.0f,  3.0f),	//bl
							float2(-3.0f,  0.0f),	//l
	};
	float2 viewportSize = GetViewportSize();

	const uint samplingCount	= GetSamplingCount();
	const float occluedRate		= GetOccluedRate();
	const float skipDist		= GetSkipDist();
	const float ssaoScale		= GetScale();

	float result = 0.0f;
	for(uint i=0; i<samplingCount; ++i)
	{
		float2 sampledUV		= input.uv + (samples[i] * rcp(viewportSize) * ssao_stepUV) * abs( SimpleNoise(float3(input.uv.xy, input.uv.x + input.uv.y)) );
#if (MSAA_SAMPLES_COUNT > 1)
		uint2 sampledPos		= int2(viewportSize * sampledUV); 
#endif

#if (MSAA_SAMPLES_COUNT > 1)
		float occluedDepth		= GBufferDepth.Load(sampledPos, 0).r;
#else
		float occluedDepth		= GBufferDepth.Sample(PointSampler, sampledUV).r;
#endif


		if(abs( ProjDepthToView(occluedDepth) - ProjDepthToView(depth) ) > skipDist )
			continue;

		float4 occluedH			= float4(sampledUV.x * 2.0f - 1.0f, sampledUV.y * 2.0f - 1.0f, occluedDepth, 1.0f);
		float4 occluedViewPos	= mul(occluedH, tbrParam_invProjMat);
		occluedViewPos /= occluedViewPos.w;

#if (MSAA_SAMPLES_COUNT > 1)
		float3 occluedNormal	= GBufferNormal_roughness.Load(sampledPos, 0).rgb * 2.0f - 1.0f;
#else
		float3 occluedNormal	= GBufferNormal_roughness.Sample(PointSampler, sampledUV).rgb * 2.0f - 1.0f;
#endif

		float3 posToOcclued = (occluedViewPos - viewPos).xyz;
		float3 dir			= normalize(posToOcclued);
		float  dist			= length(posToOcclued);

		if( dot(occluedNormal, normal) < occluedRate )
			result += ssaoScale * saturate( dot(normal, dir) ) * rcp(1.0f + dist);
	}

	float3 color = InputSceneMap.Sample(PointSampler, input.uv).rgb;

	const float lumRate = 0.5f;
	float ao = 1.0f - saturate(result / float(samplingCount));
	ao = lerp(ao, 1.0f, Luminance(color) * lumRate);

	return float4(color * ao, 1.0f);
}