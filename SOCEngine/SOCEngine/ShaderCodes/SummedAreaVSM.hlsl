//EMPTY_META_DATA

SamplerState			linearSampler		: register(s0);
Texture2D<float>		inputDepthTexture	: register(t0);
RWTexture2D<float>		outputDepthTexture	: register(u0);

[numthreads(32, 1, 1)]
void BuildSummedAreaAxisXCS(uint3 globalIdx : SV_DispatchThreadID)
{
	uint2 texSize;
	inputTexture.GetDimensions(texSize.x, texSize.y);

	float4 accum = inputTexture[globalIdx.xy];
	outputTexture[globalIdx.xy] = accum;
	
	for(uint i = 1; i < texSize.y; ++i)
	{
		uint2 idx = globalIdx.xy + uint2(0, i);

		float4 read = inputTexture[idx];
		accum += float4(read.xy, 1.0f, 1.0f);

		outputTexture[idx] = float4(accum.x, accum.y, read.z, 1.0f);
	}
}

[numthreads(1, 32, 1)]
void BuildSummedAreaAxisYCS(uint3 globalIdx : SV_DispatchThreadID)
{
	uint2 texSize;
	inputTexture.GetDimensions(texSize.x, texSize.y);

	float4 accum = inputTexture[globalIdx.xy];
	outputTexture[globalIdx.xy] = accum;

	for(uint i = 1; i < texSize.x; ++i)
	{
		uint2 idx = globalIdx.xy + uint2(i, 0);

		float4 read = inputTexture[idx];
		accum += float4(read.xy, 1.0f, 1.0f);

		outputTexture[idx] = float4(accum.x, accum.y, read.z, 1.0f);
	}
}

[numthreads(1, 64, 1)]
void CSMain(uint3 DispatchThreadID : SV_DispatchThreadID)
{
	uint2 texInfo;
	Texture_Read.GetDimensions(texInfo.x, texInfo.y);
	float sizeX = texInfo.x;

	uint2 idx = DispatchThreadID.xy;
	float4 accum = Texture_Read[idx];
	Texture_Write[idx] = accum;

	for(int j = 1; j < sizeX; j++)
	{
		idx.x++;
		
		float4 read = Texture_Read[idx];
		accum = accum + float4(read.x, read.y, 1.0f, 1.0f);
	
		Texture_Write[idx] = float4(accum.x, accum.y, read.z, 1.0f);
	}
}