struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
};

struct GEOMETRY_BUFFER_PS_INPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionView		: POSITION_VIEW; // View Space Position
	float2 uv				: TEXCOORD0;
};

Texture2D txDiffuse 		: register( t0 );
Texture2D txNormal	 		: register( t1 );
Texture2D txSpecular 		: register( t2 );
Texture2D txOpacity 		: register( t3 );

SamplerState samplerState 		: register( s0 );

GEOMETRY_BUFFER_PS_INPUT VS( VS_INPUT input )
{
	GEOMETRY_BUFFER_PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

    return ps;
}

void PS( GEOMETRY_BUFFER_PS_INPUT input, out GBuffer outGBuffer )
{
	float4 diffuseTex = txDiffuse.Sample(samplerState, input.uv);
#ifdef ENABLE_ALPHA_TEST
	if(diffuseTex.a < ALPHA_TEST_BIAS)
		discard;
#endif

#ifdef ENABLE_ALPHA_TEST
	outGBuffer.albedo 	= diffuseTex * material_mainColor;
	outGBuffer.albedo.a = 1;
#else
	outGBuffer.albedo 	= diffuseTex * material_mainColor;
#endif

	outGBuffer.specular_fresnel0	= txSpecular.Sample(samplerState, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness 	= float4(0, 0, 0, material_roughness);
}