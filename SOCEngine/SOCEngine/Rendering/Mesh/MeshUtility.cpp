#include "MeshUtility.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Intersection;
using namespace Rendering::Manager;
using namespace Core;
using namespace Math;

void MeshUtility::Culling(const Frustum& frustum, MeshManager& meshMgr, const TransformPool& transformPool)
{
	auto Cull = [&frustum, &meshMgr, &transformPool](auto& pool) -> void
	{
		pool.Iterate(
			[&frustum, &meshMgr, &transformPool](Mesh& mesh)
			{
				ObjectID id = mesh.GetObjectID();
				const Transform* transform = transformPool.Find(id.Literal());
				Vector3 worldPos = transform->GetWorldPosition();
	
				mesh._culled = frustum.In(worldPos, mesh.GetRadius());
			}
		);
	};

	Cull( meshMgr.GetOpaqueMeshPool() );
	Cull( meshMgr.GetAlphaBlendMeshPool() );
	Cull( meshMgr.GetTransparentMeshPool() );
}
