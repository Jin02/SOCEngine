//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "ShaderCommon.h"
#include "HDRCommon.h"
#include "ToneMapping.h"

Texture2D<float4>	CurColorMap				: register( t0 );
Texture2D<float4>	PrevAdaptedLuminanceMap	: register( t1 );
Texture2D<float4>	InputBloomMap			: register( t2 );

SamplerState		Sampler					: register( s0 );

float AverageLuminance()
{
	return PrevAdaptedLuminanceMap.Load(uint3(0,0,0)).x;
}

float3 ComputeExposedColor(float3 color, float avgLum, float threshold)
{
	avgLum = max(avgLum, 0.0001f);

	float linearExposure	= (hdr_exposureStrength / avgLum);

	float	exposure = log2( max(linearExposure, 0.001f) );
			exposure -= threshold;

	return color * exp2(exposure);
}

float4 Bloom_Threshold_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float3 color		= CurColorMap.Sample(Sampler, input.uv).rgb;
	float avgLum		= AverageLuminance();

	color = ComputeExposedColor(color.rgb, avgLum, bloom_threshold);

	return float4(color, 1.0f);
}

float3 BloomToneMapping(float3 color, float avgLum, float threshold)
{
	color = ComputeExposedColor(color, avgLum, threshold);
	color = Uncharted2ToneMapping(color);

	return color;
}

float4 Bloom_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float4 color		= CurColorMap.Sample(Sampler, input.uv);
	float avgLum		= AverageLuminance();
	color.rgb			= ToGamma(BloomToneMapping(color.rgb, avgLum, 0.0f), GetGamma());

	float3 bloom		= InputBloomMap.Sample(Sampler, input.uv).rgb;
	float3 finalColor	= min(color.rgb + bloom.rgb, 1.0f);

	return float4(finalColor, color.a);
}