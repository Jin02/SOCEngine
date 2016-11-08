#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float3 position 			: POSITION;
	float3 normal				: NORMAL;
	float2 uv				: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 position 		 	: SV_POSITION;
	float2 uv				: TEXCOORD0;
	float3 worldPos				: WORLD_POS;
	float3 velocity				: VELOCITY;

	float3 normal 				: NORMAL;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;
	
	float4 localPos		= float4(input.position, 1.0f);
	float4 worldPos		= mul(localPos, transform_world);
	ps.position 		= mul(worldPos, camera_viewProjMat);
	ps.worldPos		= worldPos.xyz / worldPos.w;

	ps.uv			= input.uv;
	ps.normal 		= mul(input.normal, (float3x3)transform_worldInvTranspose);
	
	float4 prevWorldPos	= mul(localPos, transform_prevWorld);
	float4 prevPos		= mul(prevWorldPos, camera_prevViewProjMat);

	ps.velocity		= ((ps.position.xyz / ps.position.w) - (prevPos.xyz / prevPos.w)) / 2.0f;
 
    return ps;
}

GBuffer PS( VS_OUTPUT input ) : SV_Target
{
	GBuffer outGBuffer;

#ifdef ENABLE_ALPHA_TEST
	float alpha		= GetAlpha(GBufferDefaultSampler, input.uv);

	if(alpha < ALPHA_TEST_BIAS)
		discard;
#endif

	float3 normal = normalize(input.normal);
	MakeGBuffer(normal, input.uv, outGBuffer.albedo_occlusion, outGBuffer.motionXY_metallic_specularity, outGBuffer.normal_roughness, outGBuffer.emission_materialFlag);

	return outGBuffer;
}
