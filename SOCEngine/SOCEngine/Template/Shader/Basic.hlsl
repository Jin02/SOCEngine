struct VS_INPUT
{
	float4 pos 		: POSITION;
};

/* DepthWrite */
DEPTH_WRITE_PS_INPUT DepthWriteVS(VS_INPUT input)
{
	DEPTH_WRITE_PS_INPUT ps;
	ps.pos = mul( input.pos, transform_worldViewProj );
	VS_CALC_DEPTH_WITH_POS(ps, input.pos);

    return ps;
}
/* end */


/* Common */
struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
};

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(1.0f, 0.0f, 1.0f, 1.0f);
}

//alphatest
struct ALPHA_TEST_PS_INPUT
{
	float4 pos 		: SV_POSITION;
};

ALPHA_TEST_PS_INPUT AlphaTestVS(VS_INPUT input)
{
	ALPHA_TEST_PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
    return ps;
}

float4 AlphaTestPS(ALPHA_TEST_PS_INPUT input) : SV_Target
{
	if(material_opacity < ALPHA_TEST)
		discard;
	return float4(material_mainColor, material_opacity);
}