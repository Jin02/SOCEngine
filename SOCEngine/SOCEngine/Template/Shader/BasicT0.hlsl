struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
};

Texture2D txDiffuse 		: register( t1 );
SamplerState testSampler 	: register( s0 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float2 uv		: TEXCOORD0;
};

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position = mul( input.position, transform_worldViewProj );
	ps.uv		= input.tex;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 texDiffuse = txDiffuse.Sample(testSampler, input.uv);
	return texDiffuse;
}