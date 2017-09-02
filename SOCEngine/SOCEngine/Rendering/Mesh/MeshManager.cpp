#include "MeshManager.hpp"

using namespace Rendering;
using namespace Device;
using namespace Core;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Math;

Mesh& MeshManager::Acquire(Core::ObjectID objID)
{
	auto mesh = Mesh(objID);
	return GetOpaqueMeshPool().Add(mesh);
}

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

void MeshManager::CheckDirty(const Core::TransformPool& tfPool)
{
	auto& dirty = _dirtyMeshes;
	auto Check = [&dirty, &tfPool](auto& pool)
	{
		pool.Iterate(
			[&dirty, &tfPool](Mesh& mesh)
			{
					auto tf = tfPool.Find(mesh.GetObjectID().Literal()); assert(tf);

					if (tf->GetDirty())
						dirty.push_back(&mesh);
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
	auto Update = [&dx, &tfPool](auto& vector)
	{		
		for (auto& mesh : vector)
		{
			uint id = mesh->GetObjectID().Literal();
			
			const Transform* tf = tfPool.Find(id); assert(tf);
			mesh->UpdateTransformCB(dx, *tf);
		}
	};

	Update(_dirtyMeshes);
}