//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "ShaderCommon.h"
#include "HDRCommon.h"

Texture2D<float4>	CurColorMap				: register( t0 );
Texture2D<float4>	PrevAdaptedLuminanceMap	: register( t1 );

SamplerState		DefaultSampler			: register( s0 );

float EyeAdaptation_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float curLum	= Luminance(CurColorMap.SampleLevel(DefaultSampler, float2(0.5f, 0.5f), 11).rgb);
	float prevLum	= PrevAdaptedLuminanceMap.Load(uint3(0, 0, 0)).x;

	float result	= prevLum + (curLum - prevLum) * (1.0f - exp(-hdr_timeDelta * hdr_exposureSpeed));

	return result;
}