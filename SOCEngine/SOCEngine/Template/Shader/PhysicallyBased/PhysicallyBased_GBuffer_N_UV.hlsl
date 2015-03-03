struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
	float3 normal			: NORMAL;
};

struct GEOMETRY_BUFFER_PS_INPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionView		: POSITION_VIEW; // View Space Position
	float2 uv				: TEXCOORD0;
	float  depth			: DEPTH;

	float3 normal 			: NORMAL;
};

Texture2D txDiffuse 		: register( t0 );
Texture2D txNormal	 		: register( t1 );
Texture2D txSpecular 		: register( t2 );
Texture2D txOpacity 		: register( t3 );

SamplerState sampler 		: register( s0 );

GEOMETRY_BUFFER_PS_INPUT VS( VS_INPUT input )
{
	GEOMETRY_BUFFER_PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.positionView = mul( input.position, transform_worldView );
	ps.uv			= input.uv;
	ps.depth 		= ps.positionView.z / camera_far;
	ps.normal 		= normalize( mul( input.normal, transform_worldView ) );
 
    return ps;
}

void PS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	outGBuffer.albedo 	= txDiffuse.Sample(sampler, input.uv) * material_mainColor;

	outGBuffer.specular_fresnel0	= txSpecular.Sample(sampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness.rgb = input.normal;
	outGBuffer.normal_roughness.a 	= material_roughness;

	outGBuffer.depth.r = input.depth;
}

void AlphaTestPS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	PS(input, outGBuffer);
	
	outGBuffer.albedo.a = (1.0f - txOpacity.Sample(sampler, input.uv)) * material_opacity;
	
	// Alpha Test
	clip(outGBuffer.albedo.a - ALPHA_TEST_COMP_VALUE);

	outGBuffer.normal_roughness.rgb = normalize( ComputeFaceNormal(input.positionView) );
}