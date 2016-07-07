//EMPTY_META_DATA

#include "FullScreenShader.h"

static const float Weights[7]	= {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};


Texture2D<float4>	InputMap			: register( t0 );
SamplerState		DefaultSampler		: register( s0 );

float4 GuassianBlur_InFullScreen_PS(PS_INPUT input) : SV_Target
{
#if defined(BLUR_HORIZONTAL)
	const int2 Offsets[7]	= { int2(-3, 0),
								int2(-2, 0),
								int2(-1, 0),
								int2( 0, 0),
								int2( 1, 0),
								int2( 2, 0),
								int2( 3, 0)	};
#elif defined(BLUR_VERTICAL)
	const int2 Offsets[7]	= { int2(0, -3),
								int2(0, -2),
								int2(0, -1),
								int2(0,  0),
								int2(0,  1),
								int2(0,  2),
								int2(0,  3) };
#endif

	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

	[unroll]
	for(uint i=0; i<7; ++i)
		resultColor += InputMap.Sample(DefaultSampler, input.uv, Offsets[i] ) * Weights[i];

	return float4(resultColor.rgb, 1.0f);
}
