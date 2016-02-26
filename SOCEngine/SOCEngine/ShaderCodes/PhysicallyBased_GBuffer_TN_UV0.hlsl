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
	float3 worldPos			: WORLD_POS;

	float3 normal 			: NORMAL;
	float3 tangent 			: TANGENT;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;

	ps.worldPos 	= mul( float4(input.position, 1.0f),	transform_world );
	ps.position 	= mul( ps.worldPos,						cameraMat_viewProj );

	ps.normal 		= normalize( mul(input.normal, (float3x3)transform_worldInvTranspose ) );
	ps.tangent 		= normalize( mul(input.tangent, (float3x3)transform_worldInvTranspose ) );
 
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
	MakeGBuffer(diffuseTex, float4(normal, roughness), specular, input.worldPos,
		outGBuffer.albedo_sunOcclusion, outGBuffer.specular_metallic, outGBuffer.normal_roughness, outGBuffer.emission);
#else
	MakeGBuffer(diffuseTex, normal, specular, input.worldPos,
	outGBuffer.albedo_sunOcclusion, outGBuffer.specular_metallic, outGBuffer.normal_roughness, outGBuffer.emission);
#endif

	return outGBuffer;
}