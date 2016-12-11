#ifndef __SOC_TONE_MAPPING_H__
#define __SOC_TONE_MAPPING_H__

#include "HDRCommon.h"

float3 LinearToneMapping(float3 color)
{
	float exposure = 1.0f;

	color = saturate(exposure * color);
	color = pow(color, (1.0f / hdr_gamma).xxx);

	return color;
}

float3 SimpleReinhardToneMapping(float3 color)
{
	float exposure = 1.5f;

	color *= exposure/(1.0f + color / exposure);
	color = pow(color, (1.0f / hdr_gamma).xxx);

	return color;
}

float3 LumianceBasedReinhardToneMapping(float3 color)
{
	float luma				= dot(color, float3(0.2126f, 0.7152f, 0.0722f));
	float toneMappedLuma	= luma / (1.0f + luma);

	color *= toneMappedLuma / luma;
	color = pow(color, (1.0f / hdr_gamma).xxx);

	return color;
}

float3 whitePreservingLumaBasedReinhardToneMapping(float3 color)
{
	float white = 2.0f;
	float luma = dot(color, float3(0.2126f, 0.7152f, 0.0722f));
	float toneMappedLuma = luma * (1.0f + luma / (white*white)) / (1.0f + luma);
	color *= toneMappedLuma / luma;
	color = pow(color, (1.0f / hdr_gamma).xxx);
	return color;
}

float3 RomBinDaHouseToneMapping(float3 color)
{
    color = exp( -1.0f / ( 2.72f * color + 0.15f) );
	color = pow(color, (1.0f / hdr_gamma).xxx);
	return color;
}

float3 filmicToneMapping(float3 color)
{
	color = max(float3(0.0f, 0.0f, 0.0f), color - (0.004f).xxx);
	color = (color * (6.2f * color + 0.5f)) / (color * (6.2f * color + 1.7f) + 0.06f);
	return color;
}

float3 Uncharted2ToneMapping(float3 color)
{
	float A = 0.15f;
	float B = 0.50f;
	float C = 0.10f;
	float D = 0.20f;
	float E = 0.02f;
	float F = 0.30f;
	float W = 11.2f;
	float exposure = 2.0f;
	color *= exposure;
	color = ((color * (A * color + C * B) + D * E) / (color * (A * color + B) + D * F)) - E / F;
	float white = ((W * (A * W + C * B) + D * E) / (W * (A * W + B) + D * F)) - E / F;
	color /= white;
	color = pow(color, (1.0f / hdr_gamma).xxx);
	return color;
}


#endif
