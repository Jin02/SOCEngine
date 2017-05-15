#include "MeshUtility.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Intersection;
using namespace Rendering::Manager;
using namespace Core;
using namespace Math;

void Geometry::MeshUtility::Culling(const Frustum& frustum, MeshManager& meshMgr, TransformPool& transformPool)
{
	auto Cull = [&frustum, &meshMgr, &transformPool](auto& pool) -> void
	{
		uint size = pool.GetSize();
		for (uint meshIdx = 0; meshIdx < size; ++meshIdx)
		{
			auto& mesh = pool.Get(meshIdx);

			ObjectId id = mesh.GetObjectId();
			Transform* transform = transformPool.Find(id);
			Vector3 worldPos = transform->GetWorldPosition();

			mesh._culled = frustum.In(worldPos, mesh.GetRadius());
		}
	};

	Cull( meshMgr.GetPool<OpaqueTrait>() );
	Cull( meshMgr.GetPool<TransparencyTrait>() );
	Cull( meshMgr.GetPool<AlphaBlendTrait>() );
}
