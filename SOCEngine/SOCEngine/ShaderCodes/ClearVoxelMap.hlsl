//EMPTY_META_DATA

RWTexture3D<uint> OutTarget : register( u0 );

[numthreads(8, 8, 8)]
void CS(uint3 globalIdx : SV_DispatchThreadID, 
	uint3 localIdx	: SV_GroupThreadID,
	uint3 groupIdx	: SV_GroupID)
{
	OutTarget[globalIdx] = 0;
}
