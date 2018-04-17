//NOT_CREATE_META_DATA

#ifndef __SOC_NORMAL_MAPPING_H__
#define __SOC_NORMAL_MAPPING_H__

float3 UnpackNormal(float3 normalMapXYZ, float3 normal, float3 tangent)
{
	float3 binormal = normalize( cross(normal, tangent) );

	float3x3 TBN = float3x3(normalize(tangent), normalize(binormal), normalize(normal));

	float3 tangentNormal = normalize(normalMapXYZ * 2.0f - 1.0f);
	return mul(TBN, tangentNormal);
}

#endif
