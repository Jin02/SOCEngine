#include "MeshManager.h"

#include "CameraManager.h"
#include "Object.h"

using namespace Rendering;
using namespace Device;
using namespace Core;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Math;
using namespace Intersection;

void MeshManager::Delete(Core::ObjectID objID)
{
	GetOpaqueMeshPool()->Delete(objID.Literal());
	GetAlphaTestMeshPool()->Delete(objID.Literal());
	GetTransparentMeshPool()->Delete(objID.Literal());
}

bool MeshManager::Has(Core::ObjectID objID) const
{
	return	GetOpaqueMeshPool()->Has(objID.Literal())		|
			GetAlphaTestMeshPool()->Has(objID.Literal())	|
			GetTransparentMeshPool()->Has(objID.Literal());
}

Mesh* MeshManager::Find(Core::ObjectID id)
{
	auto opaque = GetOpaqueMeshPool()->Find(id.Literal());
	if (opaque) return opaque;

	auto alpha = GetAlphaTestMeshPool()->Find(id.Literal());
	if (alpha) return alpha;

	auto transparency = GetTransparentMeshPool()->Find(id.Literal());
	return transparency;
}

void MeshManager::CheckDirty(	const TransformPool& tfPool,
								const ObjectManager& objMgr,
								const CameraManager& camMgr	)
{
	auto Check = [&tfPool, &camMgr, &objMgr,
				  &dirty				= _dirtyMeshes,
				  &mustUpdateCBMeshes	= _mustUpdateCBMeshes](auto& pool) -> bool
	{
		bool hasChanged = false;

		pool.Iterate(
			[&dirty, &tfPool, &camMgr, &objMgr, &mustUpdateCBMeshes, &hasChanged](Mesh& mesh)
			{
					const Transform*	tf	= tfPool.Find(mesh.GetObjectID().Literal()); assert(tf);
					const Object*		obj	= objMgr.Find(mesh.GetObjectID()); assert(obj);

					bool meshDirty	= mesh.GetTFChangeState() != TransformCB::ChangeState::No;
					bool mustUpdate	= (tf->GetDirty() | meshDirty) & obj->GetUse();
					if (mustUpdate)
					{
						dirty.push_back(&mesh);

						if (camMgr.InFrustumAllCamera(tf->GetWorldPosition(), mesh.GetRadius()))
							mustUpdateCBMeshes.push_back(&mesh);

						hasChanged |= mesh.GetTFChangeState() == TransformCB::ChangeState::HasChanged;
					}
			}
		);

		return hasChanged;
	};

	_dirty |= Check(*GetOpaqueMeshPool());
	_dirty |= Check(*GetAlphaTestMeshPool());
	_dirty |= Check(*GetTransparentMeshPool());
}

void MeshManager::ComputeWorldSize(
	Vector3& refWorldMin, Vector3& refWorldMax,
	const TransformPool& tfPool) const
{
	auto Compute = [&refWorldMin, &refWorldMax, &tfPool](auto& vector)
	{
		for (auto& iter : vector)
		{
			ObjectID id = iter->GetObjectID();

			const Transform* tf = tfPool.Find(id.Literal()); assert(tf);
			if (tf->GetDirty())
				iter->CalcWorldSize(refWorldMin, refWorldMax, *tf);
		}
	};

	Compute(_dirtyMeshes);
}

void MeshManager::UpdateTransformCB(DirectX& dx, const Core::TransformPool& tfPool)
{
	for (auto& mesh : _mustUpdateCBMeshes)
	{
		uint id = mesh->GetObjectID().Literal();

		const Transform* tf = tfPool.Find(id); assert(tf);
		mesh->UpdateTransformCB(dx, *tf);
	}
}
