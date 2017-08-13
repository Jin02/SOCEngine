//NOT_CREATE_META_DATA

#ifndef __SOC_NORMAL_MAPPING_H__
#define __SOC_NORMAL_MAPPING_H__

float3 NormalMapping(float3 normalMapXYZ, float3 normal, float3 tangent)
{
	float3 binormal = normalize( cross(normal, tangent) );

	float3x3 TBN = float3x3(normalize(tangent), normalize(binormal), normalize(normal));
	TBN = transpose(TBN);

	float3 tangentNormal = normalize(normalMapXYZ * 2 - 1);
	return mul(TBN, tangentNormal);
}

#endif