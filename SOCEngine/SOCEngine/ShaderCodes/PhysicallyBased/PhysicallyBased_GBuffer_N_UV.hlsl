#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float4 position 			: POSITION;
	float2 uv					: TEXCOORD0;
	float3 normal				: NORMAL;
};

struct GEOMETRY_BUFFER_PS_INPUT
{
	float4 position 		 	: SV_POSITION;
	float2 uv					: TEXCOORD0;

	float3 normal 				: NORMAL;
};

GEOMETRY_BUFFER_PS_INPUT VS( VS_INPUT input )
{
	GEOMETRY_BUFFER_PS_INPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;
	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_worldView ) );
 
    return ps;
}

GBuffer PS( GEOMETRY_BUFFER_PS_INPUT input )
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float alpha = diffuseTex.a * opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	outGBuffer.albedo_metallic.rgb	= diffuseTex.rgb * material_mainColor;
	outGBuffer.albedo_metallic.a	= material_metallic;

	outGBuffer.specular_fresnel0	= specularTexture.Sample(GBufferDefaultSampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	float3 normal = normalize(input.normal + DecodeNormalTexture(normalTexture, input.uv, GBufferDefaultSampler));

	outGBuffer.normal_roughness.rgb = float3(normal * 0.5f + 0.5f);
	outGBuffer.normal_roughness.a 	= material_roughness;

	return outGBuffer;
}