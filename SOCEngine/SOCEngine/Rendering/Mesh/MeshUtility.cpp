#include "MeshUtility.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Intersection;
using namespace Rendering::Manager;
using namespace Core;
using namespace Math;

const Transform& MeshUtility::_FindTransform(const Mesh& mesh, const TransformPool& transformPool)
{
	auto id = mesh.GetObjectID();
	uint findIdx = transformPool.GetIndexer().Find(id.Literal());
	assert(findIdx != TransformPool::IndexerType::FailIndex());

	return transformPool.Get(findIdx);				
}

void MeshUtility::_SortTransparentMesh(TransparentMeshPtrs& refMeshes,
						 				const Vector3& viewDir, const TransformPool& transformPool)
{
	auto SortingByDistance = [&transformPool, &viewDir](const Mesh* left, const Mesh* right) -> bool
	{
		auto SortKey = [&viewDir, &transformPool](const Mesh* mesh) -> float
		{
			auto& pos = _FindTransform(*mesh, transformPool).GetWorldPosition();				
			return Vector3::Dot(pos, viewDir);
		};
	
		return SortKey(left) < SortKey(right);
	};
	
	std::sort(refMeshes.begin(), refMeshes.end(), SortingByDistance);				
}
