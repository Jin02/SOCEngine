struct VS_INPUT
{
	float4 pos 		: POSITION;
	float3 normal	: NORMAL;
};

//DepthWrite
DEPTH_WRITE_PS_INPUT DepthWriteVS(VS_INPUT input)
{
	DEPTH_WRITE_PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
	VS_CALC_DEPTH_WITH_POS(ps, input.pos);

    return ps;
}
float4 DepthWritePS(DEPTH_WRITE_PS_INPUT input) : SV_Target
{
	float4 depth;
	depth.x	= input.depth;
	return depth;
}
//End


//Common
struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float3 normal 	: NORMAL;
};

 PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
	ps.normal 	= normalize( mul( input.normal, transform_world ) );

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	return float4(0.0f, 1.0f, 0.0f, 1.0f);
}
//End

//AlphaTest
PS_INPUT AlphaTestVS(VS_INPUT input)
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
    return ps;
}

float4 AlphaTestPS(PS_INPUT input) : SV_Target
{
	if(material_opacity < ALPHA_TEST)
		discard;
	return float4(material_mainColor, material_opacity);
}
//End