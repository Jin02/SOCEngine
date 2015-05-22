cbuffer Transform : register( b0 )
{
	matrix transform_worldViewProj;
};

struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position 	 	: SV_POSITION;
	float2 uv				: TEXCOORD0;
};

Texture2D txDiffuse 		: register( t0 );
SamplerState defaultSampler	: register( s0 );

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	//float4 texDiffuse = txDiffuse.Sample(defaultSampler, input.uv);
	return float4(1, 0, 0, 1);
}