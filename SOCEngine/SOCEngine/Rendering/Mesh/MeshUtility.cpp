#include "MeshUtility.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Intersection;
using namespace Rendering::Manager;
using namespace Rendering::RenderQueue;
using namespace Core;
using namespace Math;

const Transform& MeshUtility::_FindTransform(ObjectID id, const TransformPool& transformPool)
{
	uint findIdx = transformPool.GetIndexer().Find(id.Literal());
	assert(findIdx != TransformPool::IndexerType::FailIndex());

	return transformPool.Get(findIdx);
}

void MeshUtility::_SortTransparentMesh(TransparentMeshRenderQueue& renderQueue,
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
	
	std::sort(renderQueue.begin(), renderQueue.end(), SortingByDistance);				
}
