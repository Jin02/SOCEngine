#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float4 position 			: POSITION;
	float2 uv					: TEXCOORD0;
	float3 normal				: NORMAL;
};

struct VS_OUTPUT
{
	float4 position 		 	: SV_POSITION;
	float2 uv					: TEXCOORD0;

	float3 normal 				: NORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;
	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_worldView ) );
 
    return ps;
}

GBuffer PS( VS_OUTPUT input ) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float opacityMap = 1.0f - opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap;
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float metallic, roughness, emission;
	Parse_Metallic_Roughness_Emission(material_metallic_roughness_emission,
		metallic, roughness, emission);

	outGBuffer.albedo_emission.rgb		= diffuseTex.xyz * material_mainColor;
	outGBuffer.albedo_emission.a		= emission;

	outGBuffer.specular_metallic.rgb	= specularTexture.Sample(GBufferDefaultSampler, input.uv).rgb;
	outGBuffer.specular_metallic.a 		= metallic;

	float3 normal = normalize(input.normal);

	outGBuffer.normal_roughness.rgb = float3(normal * 0.5f + 0.5f);
	outGBuffer.normal_roughness.a 	= roughness;

	return outGBuffer;
}