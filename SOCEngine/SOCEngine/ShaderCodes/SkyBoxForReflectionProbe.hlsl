
// #include "ReflectionProbe_Common.h" ¿¡ ÀÖÀ½
cbuffer RPInfo : register( b5 )
{
	matrix	rpInfo_viewProjs[6];

	uint	rpInfo_packedNumOfLights;
	float3	rpInfo_camWorldPos;

	float	rpInfo_range;
	float	rpInfo_near;
	float2	dummy;
};

TextureCube	cubeMap			: register(t0);
SamplerState	linearSampler		: register(s0);

struct VS_INPUT
{
	float3 position 		: POSITION;
};

struct VS_OUTPUT
{
	float3 localPos			: LOCAL_POSITION;
};

VS_OUTPUT VS(VS_INPUT input)
{
	VS_OUTPUT output;
	//output.worldPos = mul(float4(input.position, 1.0f), transform_world);
	output.localPos = input.position;

	return output;
}

struct PS_INPUT
{
	float4 position 	 	: SV_POSITION;
	float3 localPos			: LOCAL_POSITION;

	uint rtIndex			: SV_RenderTargetArrayIndex;
};

[maxvertexcount(18)]
void GS(triangle VS_OUTPUT input[3], inout TriangleStream<PS_INPUT> stream)
{
	matrix worldMat = (matrix)0;
	{
		worldMat._11 = rpInfo_range;
		worldMat._22 = rpInfo_range;
		worldMat._33 = rpInfo_range;

		worldMat._41 = rpInfo_camWorldPos.x;
		worldMat._42 = rpInfo_camWorldPos.y;
		worldMat._43 = rpInfo_camWorldPos.z;
		worldMat._44 = 1.0f;
	}

    for(uint faceIdx = 0; faceIdx < 6; ++faceIdx)
    {
		PS_INPUT output;
		output.rtIndex = faceIdx;

		for(uint i=0; i<3; ++i)
		{
			float4 worldPos = mul(float4(input[i].localPos, 1.0f), worldMat);
			output.position = mul(worldPos, rpInfo_viewProjs[faceIdx]);
			output.localPos = input[i].localPos;

			stream.Append(output);
		}
		stream.RestartStrip();
    }
}

float4 PS(PS_INPUT input) : SV_Target
{
	return cubeMap.Sample(linearSampler, input.localPos);
}
