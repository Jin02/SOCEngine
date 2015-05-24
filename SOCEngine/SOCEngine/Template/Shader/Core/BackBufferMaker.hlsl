cbuffer Transform : register( b0 )
{
	matrix transform_worldViewProj;
};

struct VS_INPUT
{
	float4 position				: POSITION;
	float2 uv					: TEXCOORD0;
};

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;
};

PS_INPUT VS( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

    return ps;
}


Texture2D mainScene				: register( t0 );
Texture2D uiScene				: register( t1 );

SamplerState pointSamplerState	: register( s0 );

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 uiTex = uiScene.Sample(pointSamplerState, input.uv);
	return uiTex;
}