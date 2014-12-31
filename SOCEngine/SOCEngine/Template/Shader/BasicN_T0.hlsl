struct VS_INPUT
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;
};

struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float3 normal 	: NORMAL;
	float2 tex		: TEXCOORD0;
	float depth 	: LINEAR_DEPTH;
};

Texture2D txDiffuse : register( t1 );
SamplerState testSampler : register( s0 );

struct DEPTH_WRITE_PS_INPUT
{
	float4 pos : SV_POSITION;	
};

DEPTH_WRITE_PS_INPUT DepthWriteVS(VS_INPUT input)
{
	DEPTH_WRITE_PS_INPUT ps;

	ps.pos = mul( input.pos, worldViewProj );
    return ps;
}

float4 DepthWritePS(DEPTH_WRITE_PS_INPUT input) : SV_Target
{
	return float4(0.0f, 0.0f, 1.0f, 1.0f);
}


#define PROJECTION_A  clippingFar/(clippingFar - clippingNear)
#define PROJECTION_B (-clippingNear) / (clippingFar - clippingNear)

float ConvertToLinear(float inDepth)
{
	return PROJECTION_B / (inDepth - PROJECTION_A);
}

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, worldViewProj );
	ps.normal 	= normalize( mul( input.normal, world ) );
	ps.tex		= input.tex;

	float4 viewPos = mul(input.pos, worldView);
	ps.depth = viewPos.z / clippingFar;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{		
	float depth = input.depth;
	float4 color = float4(depth, depth, depth, 1.0f);
	return color;
}

PS_INPUT AlphaTestVS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, worldViewProj );
	ps.normal 	= normalize( mul( input.normal, world ) );
	ps.tex		= input.tex;

    return ps;
}

float4 AlphaTestPS( PS_INPUT input ) : SV_Target
{		
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}
