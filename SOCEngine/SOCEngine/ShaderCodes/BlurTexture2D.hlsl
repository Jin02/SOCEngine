//EMPTY_META_DATA
//#16_0 기반으로 작업 중..

//#include "FullScreenShader.h" 좀 범용적으로 사용하기 위해 이건 좀 다시 생각해봐야할 가치가 있음

static const float Weights[7]	= {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};

#ifdef	CONVOLVE_HORIZONTAL
static const int Offsets[7]		= { {0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3} };
#elif	CONVOLVE_VERTICAL
static const int Offsets[7]		= { {0, -3}, {0, -2}, {0, -1}, {0, 0}, {0, 1}, {0, 2}, {0, 3} };
#endif



float4 Blur_InPS(VS_INPUT input) : SV_Target
{
	int3 pixelPos = int3(input.uv * ??????, 0);
	
}
