
cbuffer Transform : register( b0 )
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 pos 		: POSITION;
	float3 normal	: NORMAL;
};

struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float3 normal 	: NORMAL;
};

 PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, world );
	ps.pos = mul( ps.pos, view);
	ps.pos = mul( ps.pos, proj);

	ps.normal 	= normalize( mul( input.normal, world ) );

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}
