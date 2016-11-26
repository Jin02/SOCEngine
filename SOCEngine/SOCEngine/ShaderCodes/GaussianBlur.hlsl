//EMPTY_META_DATA

#include "FullScreenShader.h"

#define USE_INCREMENTAL_GAUSSIAN_COEFFICENT
#define PI 3.14159265359f

cbuffer BlurParam : register(b0)
{
	float BlurParam_sigma;
	float BlurParam_numPixelsPerSide;
	float BlurParam_blurSize;
	float BlurParam_scale;			// dummy
};

Texture2D<float4>	InputMap			: register( t0 );
SamplerState		DefaultSampler		: register( s0 );

float4 GaussianBlur(Texture2D<float4> tex, float2 uv, float2 multiplyVec, float blurSize, float numPixelsPerSide, float sigma)
{
	// Incremental Gaussian Coefficent Calculation (See GPU Gems 3 pp. 877 - 889)
	float3 incrementalGaussian;
  	incrementalGaussian.x = 1.0f / (sqrt(2.0f * PI) * sigma);
  	incrementalGaussian.y = exp(-0.5f / (sigma * sigma));
  	incrementalGaussian.z = incrementalGaussian.y * incrementalGaussian.y;

  	float4 avgValue			= float4(0.0f, 0.0f, 0.0f, 0.0f);
  	float coefficientSum	= 0.0f;

  	// Take the central sample first...
  	avgValue				+= tex.Sample(DefaultSampler, uv) * incrementalGaussian.x;
  	coefficientSum			+= incrementalGaussian.x;
  	incrementalGaussian.xy	*= incrementalGaussian.yz;

  	// Go through the remaining 8 vertical samples (4 on each side of the center)
  	
  	for (float i = 1.0; i <= numPixelsPerSide; i+=1.0f)
	{
  		float2 offset = float2(i * blurSize * multiplyVec);
	    
	    avgValue += tex.Sample(DefaultSampler, uv - offset) * incrementalGaussian.x;  		         
	    avgValue += tex.Sample(DefaultSampler, uv + offset) * incrementalGaussian.x;
	             
	    coefficientSum += 2.0 * incrementalGaussian.x;
	    incrementalGaussian.xy *= incrementalGaussian.yz;
  	}
  	
  	return float4(avgValue / coefficientSum);
}

float4 GuassianBlur_InFullScreen_PS(PS_INPUT input) : SV_Target
{
	float4 resultColor = float4(0.0f, 0.0f, 0.0f, 0.0f);

#if defined(USE_INCREMENTAL_GAUSSIAN_COEFFICENT)

	float w, h;
	InputMap.GetDimensions(w, h);

	const float sigma				= BlurParam_sigma;
	const float numPixelsPerSide	= BlurParam_numPixelsPerSide;
	const float blurSize			= BlurParam_blurSize;
	const float calcBlurSize		= blurSize * 0.2f * 1.0f / w * BlurParam_scale;

#if defined(BLUR_HORIZONTAL)
	resultColor.xyz = GaussianBlur(InputMap, input.uv, float2(1.0f, 0.0f), calcBlurSize, numPixelsPerSide, sigma).rgb;
#elif defined(BLUR_VERTICAL)
	resultColor.xyz = GaussianBlur(InputMap, input.uv, float2(0.0f, 1.0f), calcBlurSize, numPixelsPerSide, sigma).rgb;
#endif

#else
	const float Weights[7]	= {0.001f, 0.028f, 0.233f, 0.474f, 0.233f, 0.028f, 0.001f};
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

	[unroll]
	for(uint i=0; i<7; ++i)
		resultColor += InputMap.Sample(DefaultSampler, input.uv, Offsets[i] ) * Weights[i];
#endif

	return float4(resultColor.rgb, 1.0f);
}
