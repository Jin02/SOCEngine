#include "ImporterUtility.h"

using namespace Importer;
using namespace Math;

void ImporterUtility::CalculateTangentBinormal(Math::Vector3& outTangents, Math::Vector3& outBinormal,
												const std::array<const Math::Vector3*, 3>& vertex,
												const std::array<const Math::Vector2*, 3>& uvs)
{
	const Vector3& v1 = *(vertex[0]);
	const Vector3& v2 = *(vertex[1]);
	const Vector3& v3 = *(vertex[2]);

	Vector3 vtx_21 = v2 - v1;
	Vector3 vtx_31 = v3 - v1;

	const Vector2& uv1 = *(uvs[0]);
	const Vector2& uv2 = *(uvs[1]);
	const Vector2& uv3 = *(uvs[2]);

	Vector2 uv_21 = uv2 - uv1;
	Vector2 uv_31 = uv3 - uv1;

	float r = 1.0f / (uv_21.x * uv_31.y - uv_31.x * uv_21.y);

	Math::Vector3 tangent;
	{
		tangent.x = (uv_31.y * vtx_21.x - uv_21.y * vtx_31.x) * r;
		tangent.y = (uv_31.y * vtx_21.y - uv_21.y * vtx_31.y) * r;
		tangent.z = (uv_31.y * vtx_21.z - uv_21.y * vtx_31.z) * r;
	}
	outTangents = tangent.Normalize();

	Math::Vector3 binormal;
	{
		binormal.x = (uv_21.x * vtx_31.x - uv_31.x * vtx_21.x) * r;
		binormal.y = (uv_21.x * vtx_31.y - uv_31.x * vtx_21.y) * r;
		binormal.z = (uv_21.x * vtx_31.z - uv_31.x * vtx_21.z) * r;
	}
	outBinormal = binormal.Normalize();
}

void ImporterUtility::CalculateNormal(Math::Vector3& outNormal, const Math::Vector3& binormal, const Math::Vector3& tangent)
{
	outNormal = Vector3::Cross(binormal, tangent);
	outNormal = outNormal.Normalize();
}