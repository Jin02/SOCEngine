//NOT_CREATE_META_DATA

#ifndef __SOC_NORMAL_MAPPING_H__
#define __SOC_NORMAL_MAPPING_H__

float3 NormalMapping(float3 normalMapXYZ, float3 normal, float3 tangent, float2 uv)
{
	float3 binormal = normalize( cross(normal, tangent) );

	float3 texNormal = normalMapXYZ;
	texNormal *= 2.0f; texNormal -= float3(1.0f, 1.0f, 1.0f);

	float3x3 TBN = float3x3(normalize(binormal), normalize(tangent), normalize(normal));

	return normalize( mul(texNormal, TBN) );
}

#endif