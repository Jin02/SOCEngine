struct VS_INPUT
{
	float4 pos 		: POSITION;
	float2 tex		: TEXCOORD0;
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

Texture2D txDiffuse 		: register( t1 );
SamplerState testSampler 	: register( s0 );

//Common
struct PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float2 uv		: TEXCOORD0;
};

PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
	ps.uv		= input.tex;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
	float4 texDiffuse = txDiffuse.Sample(testSampler, input.uv);
	return texDiffuse;
}
//End

//Alpha Test
struct ALPHA_TEST_PS_INPUT
{
	float4 pos 		: SV_POSITION;
	float2 uv		: TEXCOORD0;
};

ALPHA_TEST_PS_INPUT AlphaTestVS(VS_INPUT input)
{
	ALPHA_TEST_PS_INPUT ps;

	ps.pos = mul( input.pos, transform_worldViewProj );
	ps.uv  = input.tex;

    return ps;
}

float4 AlphaTestPS(ALPHA_TEST_PS_INPUT input) : SV_Target
{
	float4 texDiffuse = txDiffuse.Sample(testSampler, input.uv);
	float alpha = texDiffuse.a * material_opacity;
	if(alpha < ALPHA_TEST) discard;
	return texDiffuse * float4(material_mainColor, material_opacity);
}
//End