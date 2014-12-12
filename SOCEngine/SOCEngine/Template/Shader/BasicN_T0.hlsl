
cbuffer Transform : register( b0 )
{
	matrix world;
	matrix view;
	matrix proj;
};

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
};

Texture2D txDiffuse : register( t1 );
SamplerState testSampler : register( s0 );

 PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, world );
	ps.pos = mul( ps.pos, view);
	ps.pos = mul( ps.pos, proj);

	ps.normal 	= normalize( mul( input.normal, world ) );
	ps.tex		= input.tex;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{		
	return txDiffuse.Sample(testSampler, input.tex);
}
