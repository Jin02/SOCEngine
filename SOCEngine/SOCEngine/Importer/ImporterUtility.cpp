#include "ImporterUtility.h"

using namespace Importer;
using namespace Math;

void ImporterUtility::ReCalculateTBN(std::vector<Math::Vector3>& outTangents, std::vector<Math::Vector3>& outBinormal, std::vector<Math::Vector3>& outNormal,
					const Math::Vector3* vertices, unsigned int vertexCount, const Math::Vector3* normals, const Math::Vector2* texcoords, const ENGINE_INDEX_TYPE* indices, unsigned int indexCount)
{
	std::vector<Vector3> tangents(vertexCount * 2);

	for(unsigned i = 0; i < indexCount; i+=3)
	{
		const Vector3& v1 = vertices[ indices[i + 0] ];
		const Vector3& v2 = vertices[ indices[i + 1] ];
		const Vector3& v3 = vertices[ indices[i + 2] ];

		Vector3 vtx_21 = v2 - v1;
		Vector3 vtx_31 = v3 - v1;

		const Vector2& uv1 = texcoords[ indices[i + 0] ];
		const Vector2& uv2 = texcoords[ indices[i + 1] ];
		const Vector2& uv3 = texcoords[ indices[i + 2] ];

		Vector2 uv_21 = uv2 - uv1;
		Vector2 uv_31 = uv3 - uv1;

		float r = 1.0f / (uv_21.x * uv_31.y - uv_31.x * uv_21.y);

		Vector3 sdir;
		sdir.x = (uv_31.y * vtx_21.x - uv_21.y * vtx_31.x) * r;
		sdir.y = (uv_31.y * vtx_21.y - uv_21.y * vtx_31.y) * r;
		sdir.z = (uv_31.y * vtx_21.z - uv_21.y * vtx_31.z) * r;

		Vector3 tdir;
		sdir.x = (uv_21.x * vtx_31.x - uv_31.x * vtx_21.x) * r;
		sdir.y = (uv_21.x * vtx_31.y - uv_31.x * vtx_21.y) * r;
		sdir.z = (uv_21.x * vtx_31.z - uv_31.x * vtx_21.z) * r;

		tangents[ indices[i + 0] ] += sdir;
		tangents[ indices[i + 1] ] += sdir;
		tangents[ indices[i + 2] ] += sdir;

		tangents[ indices[i + 0] + vertexCount ] += sdir;
		tangents[ indices[i + 1] + vertexCount ] += sdir;
		tangents[ indices[i + 2] + vertexCount ] += sdir;
	}

	for(unsigned int idx = 0; idx < vertexCount; ++idx)
	{
		const Vector3& n = normals[idx];
		const Vector3& t = tangents[idx];

		//Gram-Schmidt orthogonalize
		Vector3 tangent = (t - n * Vector3::Dot(n, t)).Normalize();
		outTangents.push_back(tangent);

		// Calculate handedness
		float dot = Vector3::Dot( Vector3::Cross(n, t), tangents[idx + vertexCount]);
		float handedness = (dot < 0.0f) ? -1.0f : 1.0f;

		Vector3 binormal = (Vector3::Cross(n, t) * handedness).Normalize();
		outBinormal.push_back(  binormal );

		// Calculate Normal
		Vector3 normal = Vector3::Cross(tangent, binormal).Normalize();
		outNormal.push_back( normal );
	}
}