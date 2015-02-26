struct VS_INPUT
{
	float4 position : POSITION;
	float2 tex		: TEXCOORD0;
	float3 normal	: NORMAL;	
};

Texture2D txDiffuse 		: register( t1 );
SamplerState testSampler 	: register( s0 );

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal 	: NORMAL;
	float2 tex		: TEXCOORD0;
};

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position = mul( input.position, transform_worldViewProj );
	ps.normal 	= normalize( mul( input.normal, transform_world ) );
	ps.tex		= input.tex;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 texDiffuse = txDiffuse.Sample(testSampler, input.tex);
	return texDiffuse * float4(material_mainColor, material_opacity);
}