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
	using MeshType = TransparentMeshRenderQueue::value_type;

	auto SortingByDistance = [&transformPool, &viewDir](MeshType left, MeshType right) -> bool
	{
		auto SortKey = [&viewDir, &transformPool](MeshType mesh) -> float
		{
			auto& pos = _FindTransform(mesh->GetObjectID(), transformPool).GetWorldPosition();				
			return Vector3::Dot(pos, viewDir);
		};
	
		return SortKey(left) > SortKey(right);
	};
		
	std::sort(renderQueue.begin(), renderQueue.end(), SortingByDistance);				
}
