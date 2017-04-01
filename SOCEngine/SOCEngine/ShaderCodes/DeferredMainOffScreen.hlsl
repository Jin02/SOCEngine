//EMPTY_META_DATA

#include "FullScreenShader.h"

SamplerState			linerSamplerState					: register( s0 );
Texture2D<float4>		directDiffuseLightColorMap			: register( t0 );
Texture2D<float4>		directSpecularLightColorMap			: register( t1 );

#ifdef USE_GI
Texture2D<float4> indirectLightColorMap	: register( t2 );
#endif

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 diffuse	= directDiffuseLightColorMap.Sample(linerSamplerState, input.uv);
	float4 specular	= directSpecularLightColorMap.Sample(linerSamplerState, input.uv);
	float4 direct	= diffuse + specular;

#ifdef USE_GI
	float4 indirect	= indirectLightColorMap.Sample(linerSamplerState, input.uv);
	return direct + indirect;
#else
	return direct;
#endif
}