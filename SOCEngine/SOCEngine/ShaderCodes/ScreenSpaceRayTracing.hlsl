//EMPTY_META_DATA

#include "FullScreenShader.h"
#include "TBDRInput.h"


float4 PS(PS_INPUT input) : SV_Target
{
    int3 screenPos = int3(input.position.xy, 0);

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
    float3 worldNormal = GBufferNormal_roughness.Load( screenPos.xy, input.sampleIdx ).xyz;
#else
    float3 worldNormal = GBufferNormal_roughness.Load( screenPos ).xyz;
#endif
    
    worldNormal *= 2.0f;
    worldNormal -= float3(1.0f, 1.0f, 1.0f);
    
    float3 viewSpaceNormal = mul(float4(worldNormal, 0.0f), tbrParam_viewMat).xyz;

    if( any(viewSpaceNormal) == 0.0f )
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
        
    float depth                 = FetchLinerDepthFromGBuffer(screenPos.xy);
    float3 rayOriginViewSpace   = input.viewRay * depth;
}
