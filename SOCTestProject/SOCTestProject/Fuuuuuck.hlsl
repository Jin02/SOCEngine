//NOT_CREATE_META_DATA

struct Data
{
	float3 v1;
	float2 v2;
};

StructuredBuffer<Data> gInputA;
RWStructuredBuffer<Data> gOutput;


[numthreads(32, 1, 1)]
void CS(uint3 dtid : SV_DispatchThreadID, uint3 gtid : SV_GroupThreadID, uint3 gid : SV_GroupID)
{
	gOutput[dtid.x].v1 = gInputA[dtid.x].v1;
	gOutput[dtid.x].v2 = gInputA[dtid.x].v2;
}