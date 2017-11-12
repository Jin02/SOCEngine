#include "PhysicallyBased_GBuffer_Common.h"

struct VS_INPUT
{
	float3 position 			: POSITION;
	float3 normal				: NORMAL;
	float2 uv					: TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 sv_position 		 	: SV_POSITION;

//#ifdef USE_MOTION_BLUR
	float4 position				: NDC_POSITION;
	float4 prevPosition			: PREV_POSITION;
//#endif

	float3 worldPos				: WORLD_POS;
	float3 normal 				: NORMAL;
	float2 uv					: TEXCOORD0;
};

VS_OUTPUT VS( VS_INPUT input )
{
	VS_OUTPUT ps;
	
	float4 localPos		= float4(input.position, 1.0f);
	float4 worldPos		= mul(localPos, transform_world);
	float4 position		= mul(worldPos, camera_viewProjMat);
	
	ps.worldPos			= worldPos.xyz / worldPos.w;

	ps.uv				= input.uv;
	ps.normal 			= mul(input.normal, (float3x3)transform_worldInvTranspose);

//#ifdef USE_MOTION_BLUR
	float4 prevWorldPos	= mul(localPos, transform_prevWorld);
	ps.prevPosition		= mul(prevWorldPos, camera_prevViewProjMat);
	
	ps.position			= position;
//#endif
 	ps.sv_position 		= ps.prevPosition;

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

	float2 curScreenPos		= input.position.xy / input.position.w;
	float2 prevScreenPos	= input.prevPosition.xy / input.prevPosition.w;
	float2 velocity			= curScreenPos - prevScreenPos; velocity.y = -velocity.y;
	
	float3 normal = normalize(input.normal);
	MakeGBuffer(normal, input.uv, velocity, outGBuffer.albedo_occlusion, outGBuffer.velocity_metallic_specularity, outGBuffer.normal_roughness, outGBuffer.emission_materialFlag);

	return outGBuffer;
}
