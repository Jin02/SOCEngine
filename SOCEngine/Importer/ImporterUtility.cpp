#include "ImporterUtility.h"

using namespace Importer;

void Utility::CalculateTangentBinormal(Math::Vector3& outTangent, Math::Vector3& outBinormal, 
									   const std::pair<const Math::Vector3&, const TexCoord&>& vertex1,
									   const std::pair<const Math::Vector3&, const TexCoord&>& vertex2,
									   const std::pair<const Math::Vector3&, const TexCoord&>& vertex3
									   )
{
	Math::Vector3 v1 = vertex2.first - vertex1.first;
	Math::Vector3 v2 = vertex3.first - vertex1.first;

	Math::Vector2 tu = vertex2.second - vertex1.second;
	Math::Vector2 tv = vertex3.second - vertex1.second;

	float denominator = 1.0f / (tu.x * tv.y - tu.y * tv.x);

	outTangent.x = (tv.y * v1.x - tv.x * v2.x) * denominator;
	outTangent.y = (tv.y * v1.y - tv.x * v2.y) * denominator;
	outTangent.z = (tv.y * v1.z - tv.x * v2.z) * denominator;
	outTangent.Normalize();

	outBinormal.x = (tu.x * v2.x - tu.y * v1.x) * denominator;
	outBinormal.y = (tu.x * v2.y - tu.y * v1.y) * denominator;
	outBinormal.z = (tu.x * v2.z - tu.y * v1.z) * denominator;
	outBinormal.Normalize();
};

void Utility::CalculateNormal(Math::Vector3& outNormal, const Math::Vector3& tangent, const Math::Vector3& binormal)
{
	outNormal.x = (tangent.y * binormal.z) - (tangent.z * binormal.y);
	outNormal.y = (tangent.z * binormal.x) - (tangent.x * binormal.z);
	outNormal.z = (tangent.x * binormal.y) - (tangent.y * binormal.x);
	outNormal.Normalize();
}