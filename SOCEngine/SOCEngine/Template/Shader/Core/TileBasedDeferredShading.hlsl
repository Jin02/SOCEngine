//EMPTY_META_DATA
//This file include LightCulling.hlsl

[numthreads(TILE_RES_HALF, TILE_RES_HALF, 1)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
					uint3 localIdx	: SV_GroupThreadID,
					uint3 groupIdx	: SV_GroupID)
{

}