//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "ShaderCommon.h"
#include "HDRCommon.h"
#include "ToneMapping.h"

Texture2D<float4>	CurColorMap				: register( t0 );
Texture2D<float4>	PrevAdaptedLuminanceMap	: register( t1 );
Texture2D<float4>	InputBloomMap			: register( t2 );

SamplerState		DefaultSampler			: register( s0 );
SamplerState		LinearSampler			: register( s1 );

float AverageLuminance()
{
	return PrevAdaptedLuminanceMap.Load(uint3(0,0,0)).x;
}

float3 ComputeExposedColor(out float outExposure, float3 color, float avgLum, float threshold)
{
	avgLum = max(avgLum, 0.0001f);

	float linearExposure	= (hdr_exposureKey / avgLum);

	outExposure = log2( max(linearExposure, 0.001f) );
	outExposure -= threshold;

	return color * exp2(outExposure);
}

float4 Bloom_Threshold_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float3 color		= CurColorMap.Sample(DefaultSampler, input.uv).rgb;

	float avgLum		= AverageLuminance();

	float exposure		= 0.0f;
	color = ComputeExposedColor(exposure, color.rgb, avgLum, bloom_threshold);

	if( dot(color, 0.333f) <= 0.01f )
		color = float3(0.0f, 0.0f, 0.0f);

	return float4(color, 1.0f);
}

float3 BloomToneMapping(out float outExposure, float3 color, float avgLum, float threshold)
{
	color = ComputeExposedColor(outExposure, color, avgLum, threshold);
	color = Uncharted2ToneMapping(color);

	return color;
}

float4 Bloom_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float4 color		= CurColorMap.Sample(DefaultSampler, input.uv);
	float avgLum		= AverageLuminance();

	float exposure		= 0.0f;
	color.rgb			= BloomToneMapping(exposure, color.rgb, avgLum, bloom_threshold);

	float4 bloom = InputBloomMap.Sample(LinearSampler, input.uv);
	return float4( (color + bloom).rgb, color.a);
}