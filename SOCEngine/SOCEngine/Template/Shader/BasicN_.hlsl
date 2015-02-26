struct VS_INPUT
{
	float4 position : POSITION;
	float3 normal	: NORMAL;
};

struct PS_INPUT
{
	float4 position : SV_POSITION;
	float3 normal 	: NORMAL;
};

 PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position = mul( input.position, transform_worldViewProj );
	ps.normal 	= normalize( mul( input.normal, transform_world ) );

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}