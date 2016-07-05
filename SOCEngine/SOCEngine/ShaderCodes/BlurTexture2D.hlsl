//EMPTY_META_DATA
//#16_0 기반으로 작업 중..

// Blurring Common
static const float Weights[7]	= {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};

#ifdef	CONVOLVE_HORIZONTAL
static const int Offsets[7]	= { {0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3} };
#elif	CONVOLVE_VERTICAL
static const int Offsets[7]	= { {0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3} };
#endif


Texture2D<float4>	InputMap : register( t0 );

#ifdef	IN_QUAD_SCREEN
#define USE_VIEW_INFORMATION
#include "FullScreenShader.h"
float4 BlurTexture2D_InFullScreen_PS(VS_INPUT input) : SV_Target

#elif	IN_COMPUTE_SHADER
RWTexture2D<float4>	OutMap : register( u0 );

[numthreads(16, 16, 1)]
void BlurTexture2D_InCS(uint3 globalIdx : SV_DispatchThreadID, 
			uint3 localIdx	: SV_GroupThreadID,
			uint3 groupIdx	: SV_GroupID)
#endif
{
	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
#ifdef	IN_QUAD_SCREEN
	float2 viewportSize	= GetViewportSize();
	float2 uv		= input.uv * viewportSize;
	int3 globalIdx		= int3(uv, 0);
#endif

	[unroll]
	for(uint i=0; i<7; ++i)
		resultColor += inputTex2D.Load( int3(globalIdx), Offsets[i] ) * Weights[i];

#ifdef	IN_QUAD_SCREEN
	return float4(resultColor.rgb, 1.0f);
#elif	IN_COMPUTE_SHADER
	OutMap[globalIdx.xy] = float4(resultColor.rgb, 1.0f);
#endif
}

#endif
