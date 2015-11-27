//EMPTY_META_DATA

#include "Voxelization_Common.h"

[numthreads(8, 8, 8)]
void ClearVoxelMapCS(	uint3 globalIdx : SV_DispatchThreadID, 
						uint3 localIdx	: SV_GroupThreadID,
						uint3 groupIdx	: SV_GroupID)
{
	OutAnistropicVoxelTexture[globalIdx] = uint3(0, 0, 0);
}