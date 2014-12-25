
cbuffer Transform : register( b0 )
{
	matrix world;
	matrix worldView;
	matrix worldViewProj;
};

struct VS_INPUT
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float2 tex		: TEXCOORD0;
};

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

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, worldViewProj );
	ps.tex		= input.tex;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(0.0f, 0.0f, 1.0f, 1.0f);
}


PS_INPUT AlphaTestVS(VS_INPUT input)
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, worldViewProj );
	ps.tex		= input.tex;

    return ps;
}

float4 AlphaTestPS(PS_INPUT input) : SV_Target
{
	return float4(1.0f, 1.0f, 0.0f, 1.0f);
}