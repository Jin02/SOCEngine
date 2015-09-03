//NOT_CREATE_META_DATA

#ifndef __SOC_FULL_SCREEN_H__
#define __SOC_FULL_SCREEN_H__

struct PS_INPUT
{
	float4 position 			: SV_POSITION;
	float2 uv					: TEXCOORD0;

#if (MSAA_SAMPLES_COUNT > 1) //MSAA
	uint sampleIdx				: SV_SAMPLEINDEX;
#endif
};

// id는 0, 1, 2 이 3개의 값만 들어온다.
PS_INPUT FullScreenVS(uint id : SV_VERTEXID)
{
	PS_INPUT ps = (PS_INPUT)0;

	ps.position.x = (float)(id / 2) * 4.0f - 1.0f;
	ps.position.y = (float)(id % 2) * 4.0f - 1.0f;
	ps.position.z = 1.0f; //inverted depth
	ps.position.w = 1.0f;

	ps.uv.x = (float)(id / 2) * 2.0f;
	ps.uv.y = 1.0f - (float)(id % 2) * 2.0f;

	return ps;
}

#endif