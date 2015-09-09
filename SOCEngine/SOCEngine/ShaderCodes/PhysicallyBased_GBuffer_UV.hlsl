#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float4 position 		: POSITION;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float3 positionView		: POSITION_VIEW; // View Space Position
	float2 uv				: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.position 	= mul( input.position, transform_worldViewProj );
	ps.uv			= input.uv;

    return ps;
}

GBuffer PS( VS_OUTPUT input) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	//float alpha = diffuseTex.a * opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	//if(alpha < ALPHA_TEST_BIAS)
	//	discard;
#endif

	outGBuffer.albedo_metallic.rgb	= diffuseTex.rgb * material_mainColor;
	outGBuffer.albedo_metallic.a	= material_metallic;

	outGBuffer.specular_fresnel0	= specularTexture.Sample(GBufferDefaultSampler, input.uv);
	outGBuffer.specular_fresnel0.a 	= material_fresnel0;

	outGBuffer.normal_roughness.rgb = DecodeNormalTexture(normalTexture, input.uv, GBufferDefaultSampler) * 0.5f + 0.5f;
	outGBuffer.normal_roughness.a 	= material_roughness;

	return outGBuffer;
}
