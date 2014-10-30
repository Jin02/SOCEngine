
cbuffer Transform : register( b0 )
{
	matrix world;
	matrix view;
	matrix proj;
};

struct VS_INPUT
{
	float4 Pos : POSITION;
};

struct PS_INPUT
{
	float4 Pos : SV_POSITION;
};

 PS_INPUT VS ( VS_INPUT input )
{
	PS_INPUT ps;
	ps.Pos = mul( input.Pos, world );
	ps.Pos = mul( ps.Pos, view);
	ps.Pos = mul( ps.Pos, proj);
	//ps.Pos = input.Pos;

    return ps;
}

float4 PS( PS_INPUT input ) : SV_Target
{
    return float4( 1.0f, 0.0f, 0.0f, 1.0f );    // Yellow, with Alpha = 1
}
