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
	return GetPool<OpaqueTrait>().Add(objID.Literal(), mesh);
}

void MeshManager::Delete(Core::ObjectID objID)
{
	GetPool<OpaqueTrait>().Delete(objID.Literal());
	GetPool<AlphaBlendTrait>().Delete(objID.Literal());
	GetPool<TransparencyTrait>().Delete(objID.Literal());
}

bool MeshManager::Has(Core::ObjectID objID) const
{
	return	GetPool<OpaqueTrait>().GetIndexer().Has(objID.Literal()) |
		GetPool<AlphaBlendTrait>().GetIndexer().Has(objID.Literal()) |
		GetPool<TransparencyTrait>().GetIndexer().Has(objID.Literal());
}

Mesh* MeshManager::Find(Core::ObjectID id)
{
	auto opaque = GetPool<OpaqueTrait>().Find(id.Literal());
	if (opaque) return opaque;

	auto alpha = GetPool<AlphaBlendTrait>().Find(id.Literal());
	if (alpha) return alpha;

	auto transparency = GetPool<TransparencyTrait>().Find(id.Literal());
	return transparency;
}

void MeshManager::CheckDirty(const Core::TransformPool& tfPool)
{
	auto& dirty = _dirtyMeshes;
	auto Check = [&dirty, &tfPool](auto& pool)
	{
		uint size = pool.GetSize();
		for (uint i=0; i<size; ++i)
		{
			auto& mesh = pool.Get(i);
			auto tf = tfPool.Find(mesh.GetObjectID().Literal()); assert(tf);

			if (tf->GetDirty())
				dirty.push_back(&mesh);
		}
	};

	Check(GetPool<OpaqueTrait>());
	Check(GetPool<AlphaBlendTrait>());
	Check(GetPool<TransparencyTrait>());
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

void MeshManager::UpdateTraits()
{
	std::vector<Mesh> changedMeshes;
	auto Check = [&changedMeshes](auto& pool)
	{
		uint size = pool.GetSize();
		for (uint i = 0; i < size;)
		{
			auto& mesh = pool.Get(i);
			if (mesh.GetChangedTrait())
			{
				changedMeshes.push_back(mesh);
				pool.Delete(mesh.GetObjectID().Literal());
			}
		}
	};

	Check(GetPool<OpaqueTrait>());
	Check(GetPool<AlphaBlendTrait>());
	Check(GetPool<TransparencyTrait>());

	for (auto iter : changedMeshes)
	{
		if( iter.GetTrait() == Trait::Opaque )
			GetPool<OpaqueTrait>().Add(iter.GetObjectID().Literal(), iter);
		else if(iter.GetTrait() == Trait::AlphaBlend )
			GetPool<AlphaBlendTrait>().Add(iter.GetObjectID().Literal(), iter);
		else if (iter.GetTrait() == Trait::Transparency)
			GetPool<TransparencyTrait>().Add(iter.GetObjectID().Literal(), iter);

		iter.ClearDirty();
	}
}
