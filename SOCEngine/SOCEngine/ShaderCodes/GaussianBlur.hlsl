//EMPTY_META_DATA

static const float Weights[7]	= {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};

#ifdef	BLUR_HORIZONTAL
static const int Offsets[7]	= { {-3, 0}, {-2, 0}, {-1, 0}, {0, 0}, {1, 0}, {2, 0}, {3, 0} };
#elif	BLUR_VERTICAL
static const int Offsets[7]	= { {0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3} };
#endif


Texture2D<float4>	InputMap			: register( t0 );

#ifdef	IN_QUAD_SCREEN

#define USE_VIEW_INFORMATION
#include "FullScreenShader.h"
SamplerState		DefaultSampler		: register( s0 );

#elif	IN_COMPUTE_SHADER
RWTexture2D<float4>	OutMap				: register( u0 );
#endif


#ifdef	IN_QUAD_SCREEN
float4 BlurTexture2D_InFullScreen_PS(VS_INPUT input) : SV_Target
#elif	IN_COMPUTE_SHADER
[numthreads(16, 16, 1)]
void BlurTexture2D_InCS(uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
#endif
{
	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
#ifdef	IN_QUAD_SCREEN
	[unroll]
	for(uint i=0; i<7; ++i)
		resultColor += InputMap.Sample(DefaultSampler, uv, Offsets[i] ) * Weights[i];

	return float4(resultColor.rgb, 1.0f);
#elif	IN_COMPUTE_SHADER
	[unroll]
	for(uint i=0; i<7; ++i)
		resultColor += InputMap.Load( int3(globalIdx), Offsets[i] ) * Weights[i];

	OutMap[globalIdx.xy] = float4(resultColor.rgb, 1.0f);
#endif
}

#endif
