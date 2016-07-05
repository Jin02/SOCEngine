//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState		LinerSamplerState	: register( s0 );
Texture2D<float4>	DirectLightColorMap	: register( t0 );

#ifdef USE_GI
Texture2D<float4>	IndirectLightColorMap	: register( t1 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 direct	= DirectLightColorMap.Sample(LinerSamplerState, input.uv);
	return direct;
//#ifdef USE_GI
//	float4 indirect	= IndirectLightColorMap.Sample(LinerSamplerState, input.uv);
//	return direct + indirect;
//#else
//	return direct;
//#endif
}
