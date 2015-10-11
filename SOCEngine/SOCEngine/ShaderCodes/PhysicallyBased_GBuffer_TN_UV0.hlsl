#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float3 position 		: POSITION;
	float3 normal			: NORMAL;
	float3 tangent			: TANGENT;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 	 	: SV_POSITION;
	float2 uv				: TEXCOORD0;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.position 	= mul( float4(input.position, 1.0f), transform_worldViewProj );
	ps.uv			= input.uv;

	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_world ) );
	ps.tangent 		= normalize( mul(input.tangent, (float3x3)transform_world ) );
 
    return ps;
}

GBuffer PS( VS_OUTPUT input ) : SV_Target
{
	GBuffer outGBuffer;
	float4 diffuseTex = diffuseTexture.Sample(GBufferDefaultSampler, input.uv);

#ifdef ENABLE_ALPHA_TEST
	float opacityMap = 1.0f - opacityTexture.Sample(GBufferDefaultSampler, input.uv).x;
	float alpha = diffuseTex.a * opacityMap * ParseMaterialAlpha();
	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float4 normalMap = normalTexture.Sample(GBufferDefaultSampler, input.uv);
	bool hasNormalMap = HasNormalTexture();

	float3 bumpedNormal = NormalMapping(normalMap.rgb, input.normal, input.tangent, input.uv);
	float3 normal = lerp(input.normal, bumpedNormal, hasNormalMap);

	float4 specular	= specularTexture.Sample(GBufferDefaultSampler, input.uv);

#if defined(USE_PBR_TEXTURE)
	float roughness = normalMap.a;
	MakeGBuffer(diffuseTex, float4(normal, roughness), specular, outGBuffer.albedo_emission, outGBuffer.specular_metallic,	outGBuffer.normal_roughness);
#else
	MakeGBuffer(diffuseTex, normal, specular, outGBuffer.albedo_emission, outGBuffer.specular_metallic,	outGBuffer.normal_roughness);
#endif

	return outGBuffer;
}