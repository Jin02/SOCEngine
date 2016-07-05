//EMPTY_META_DATA

#define NOT_USE_BRDF_LIGHTING

#include "MonteCarlo.h"
#include "BRDF.h"

RWTexture2D<float2> OutMap : register( u0 );

[numthreads(PRE_INTEGRATE_TILE_RES, PRE_INTEGRATE_TILE_RES, 1)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
	uint3 localIdx	: SV_GroupThreadID,
	uint3 groupIdx	: SV_GroupID)
{
	uint2 size;
	OutMap.GetDimensions(size.x, size.y);

	float2 sampleUV		= (float2(globalIdx.xy) + float2(0.5f, 0.5f)) / float2(size);
	float2 sampledBRDF	= IntegrateBRDF(sampleUV.x, sampleUV.y, BRDF_SAMPLES);

	OutMap[globalIdx.xy] = sampledBRDF;
}
