struct VS_INPUT
{
	float4 position 		: POSITION;
};


struct PS_INPUT
{
	float4 position 		: SV_POSITION;
};

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.position = mul( input.position, transform_worldViewProj );
    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}