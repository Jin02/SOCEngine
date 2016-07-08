//EMPTY_META_DATA

#include "FullScreenShader.h"

Texture2D<float4> InputColorMap			: register( t0 );
Texture2D<float4> DepthBuffer			: register( t1 );

SamplerState DefaultSampler				: register( s0 );
SamplerState ShadowSampler				: register( s1 );

float GaussWeight(int sampleDist, float sigma)
{
	float g = 1.0f / sqrt(2.0f * 3.14159f * sigma * sigma);
	return (g * exp( -(sampleDist * sampleDist) / (2 * sigma * sigma)) );
}
/*
float4 BilateralGaussNear(float2 uv, uniform float2 uvScale, uniform float sigma)
{
    float4 accum		= 0;
    float accumWeight	= 0;

	float w, h;
	InputColorMap.GetDimensions(w,h);

    float4 centerTapColor	= InputColorMap.Sample(DefaultSampler, uv);
    float centerDepthTap	= DepthBuffer.Sample(ShadowSampler, uv).x;

    float dw,dh;
	DepthBuffer.GetDimensions(dw,dh);

    [unroll]
    for (int i = -6; i < 6; i++)
    {
		float2 tc	= uv + (float(i) / float2(w, h))	* uvScale;
        float2 dtc	= uv + (float(i) / float2(dw, dh))	* uvScale;

        // fetch tap
		float4 tap			= InputColorMap.Sample(ShadowSampler, tc);
        float depth_tap		= DepthBuffer.Sample(ShadowSampler, dtc).x;

        float depth_diff	= abs(depth_tap - centerDepthTap);
        float r2			= depth_diff * 100.0 / centerDepthTap;
        float g				= depth_diff == 0.0f ? 1.0f : exp(-r2 * r2);

        float weight		= GaussWeight(i, sigma) * g;

        if (depth_tap < centerDepthTap)
            accum += tap * weight;
        else
            accum += centerTapColor * weight;

        accumWeight += weight;
    }

    return float4(accum.rgb/accumWeight, 1);
}*/

float4 BilateralGaussNear(float2 uv, uniform float2 uvScale, uniform float sigma, uniform float sigmaz)
{
    float4 accum		= 0;
    float accumWeight	= 0;

	float2 colorMapSize;
	InputColorMap.GetDimensions(colorMapSize.x, colorMapSize.y);

    float4 color	= InputColorMap.Sample(DefaultSampler, uv);
	float depth		= DepthBuffer.Sample(ShadowSampler, uv).r;

	for(int i=-6; i<6; ++i)
	{
		for(int j=-6; j<6; ++j)
		{
			float2 coord	= float2(i, j) * float2(1.0f / colorMapSize);

			float3 sampledColor	= InputColorMap.Sample(DefaultSampler, uv + coord).rgb;
			float  sampledDepth	= DepthBuffer.Sample(ShadowSampler, uv + coord).r;

			float dist			= clamp(float(i*i+j*j)/float(colorMapSize.x*colorMapSize.x), 0.0f, 1.0f);
		    float dz			= (depth-sampledDepth)*(depth-sampledDepth);
		
		    float Fi			= exp(-dist*dist/(2.0f * sigma*sigma));
		    float Gi			= exp(-dz*dz/(2.0f * sigmaz*sigmaz));

			accum.rgb			+= (sampledDepth < depth) ? sampledColor : color.rgb;
			accum.rgb			*= Fi * Gi;
			accumWeight			+= Fi * Gi;
		}
	}

    return float4(accum.rgb/accumWeight, 1);
}


#if defined(BLUR_VERTICAL)
float4 BilateralGaussNear_InFullScreen_PS(PS_INPUT input) : SV_TARGET
{
    return BilateralGaussNear(input.uv, float2(0.0f, 1.0f), 2.5f, 0.005f);
}
#elif defined(BLUR_HORIZONTAL)
float4 BilateralGaussNear_InFullScreen_PS(PS_INPUT input) : SV_TARGET
{
    return BilateralGaussNear(input.uv, float2(1.0f, 0.0f), 2.5f, 0.005f);
}
#endif