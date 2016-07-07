//EMPTY_META_DATA

#include "RayTracingCommon.h"

float4 SSRT_InFullScreen_PS(PS_INPUT input) : SV_Target
{
    int3 screenPos = int3(input.position.xy, 0);

	float3	viewNormal	= float3(0.0f, 0.0f, 0.0f);
	float	roughness	= 0.0f;
#if (MSAA_SAMPLES_COUNT > 1) //MSAA
    ComputeViewNormalFromGBuffer_with_GetRoughness(viewNormal, roughness, screenPos.xy, input.sampleIdx);
#else
    ComputeViewNormalFromGBuffer_with_GetRoughness(viewNormal, roughness, screenPos.xy, 0);
#endif

    if( any(viewNormal) == 0.0f )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
        
    float depth					= FetchLinearDepthFromGBuffer(screenPos.xy);
    float3 viewRayOrigin		= input.viewRay * depth;
    
    float3 eyeToRayOriginDir	= normalize(viewRayOrigin);
    float3 rayDir				= normalize( reflect(eyeToRayOriginDir, viewNormal) );
    
    float RdotV					= dot(rayDir, eyeToRayOriginDir);
    
    float2 hitScreenPos			= float2(0.0f, 0.0f);
    float3 hitPos       		= float3(0.0f, 0.0f, 0.0f);
    
	float jitter				= ssrt_stride > 1.0f ? float( (screenPos.x + screenPos.y) & 1 ) * 0.5f : 0.0f;
	bool intersect				= TraceScreenSpaceRay(hitScreenPos, hitPos, viewRayOrigin, rayDir, jitter);
	
	depth						= GBufferDepth.Load(int3(hitScreenPos, 0)).r;

    float2 viewportSize			= GetViewportSize();
    float2 texelSize			= float2(1.0f, 1.0f) / viewportSize;
    
    hitScreenPos				*= float2(texelSize.x, texelSize.y);
    if(	hitScreenPos.x > 1.0f ||
    	hitScreenPos.x < 0.0f ||
    	hitScreenPos.y > 1.0f ||
    	hitScreenPos.y < 0.0f )
    	intersect = false;

    return float4(hitScreenPos, depth, RdotV) * (intersect ? 1.0f : 0.0f);
}
