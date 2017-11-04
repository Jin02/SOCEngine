#include "MeshManager.hpp"

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
	GetOpaqueMeshPool().Delete(objID.Literal());
	GetAlphaBlendMeshPool().Delete(objID.Literal());
	GetTransparentMeshPool().Delete(objID.Literal());
}

bool MeshManager::Has(Core::ObjectID objID) const
{
	return	GetOpaqueMeshPool().Has(objID.Literal())		|
			GetAlphaBlendMeshPool().Has(objID.Literal())	|
			GetTransparentMeshPool().Has(objID.Literal());
}

Mesh* MeshManager::Find(Core::ObjectID id)
{
	auto opaque = GetOpaqueMeshPool().Find(id.Literal());
	if (opaque) return opaque;

	auto alpha = GetAlphaBlendMeshPool().Find(id.Literal());
	if (alpha) return alpha;

	auto transparency = GetTransparentMeshPool().Find(id.Literal());
	return transparency;
}

void MeshManager::CheckDirty(	const TransformPool& tfPool,
								const ObjectManager& objMgr,
								const CameraManager& camMgr	)
{
	auto Check = [&tfPool, &camMgr, &objMgr,
				  &dirty				= _dirtyMeshes,
				  &mustUpdateCBMeshes	= _mustUpdateCBMeshes](auto& pool)
	{
		pool.Iterate(
			[&dirty, &tfPool, &camMgr, &objMgr, &mustUpdateCBMeshes](Mesh& mesh)
			{
					const Transform*	tf	= tfPool.Find(mesh.GetObjectID().Literal()); assert(tf);
					const Object*		obj	= objMgr.Find(mesh.GetObjectID()); assert(obj);

					if (tf->GetDirty() & obj->GetUse())
					{
						dirty.push_back(&mesh);

						if (camMgr.InFrustumAllCamera(tf->GetWorldPosition(), mesh.GetRadius()))
							mustUpdateCBMeshes.push_back(&mesh);
					}
			}
		);
	};

	Check(GetOpaqueMeshPool());
	Check(GetAlphaBlendMeshPool());
	Check(GetTransparentMeshPool());
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
			iter->CalcWorldSize(refWorldMin, refWorldMax, *tf);
		}
	};

	Compute(_dirtyMeshes);
}

void MeshManager::UpdateTransformCB(DirectX& dx, const Core::TransformPool& tfPool)
{
	auto Update = [& dx, &tfPool](auto& vector)
	{		
		for (auto& mesh : vector)
		{
			uint id = mesh->GetObjectID().Literal();
			
			const Transform* tf = tfPool.Find(id); assert(tf);
			mesh->UpdateTransformCB(dx, *tf);
		}
	};

	Update(_mustUpdateCBMeshes);
}
