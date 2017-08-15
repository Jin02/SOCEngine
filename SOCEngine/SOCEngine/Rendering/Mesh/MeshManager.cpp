#include "MeshManager.hpp"

using namespace Rendering;
using namespace Device;
using namespace Core;
using namespace Rendering::Geometry;
using namespace Rendering::Manager;
using namespace Math;

Mesh& MeshManager::Acquire(Core::ObjectId objId)
{
	auto mesh = Mesh(objId);
	return GetPool<OpaqueTrait>().Add(objId.Literal(), mesh);
}

void MeshManager::Delete(Core::ObjectId objId)
{
	GetPool<OpaqueTrait>().Delete(objId.Literal());
	GetPool<AlphaBlendTrait>().Delete(objId.Literal());
	GetPool<TransparencyTrait>().Delete(objId.Literal());
}

bool MeshManager::Has(Core::ObjectId objId) const
{
	return	GetPool<OpaqueTrait>().GetIndexer().Has(objId.Literal()) |
		GetPool<AlphaBlendTrait>().GetIndexer().Has(objId.Literal()) |
		GetPool<TransparencyTrait>().GetIndexer().Has(objId.Literal());
}

Mesh* MeshManager::Find(Core::ObjectId id)
{
	auto opaque = GetPool<OpaqueTrait>().Find(id.Literal());
	if (opaque) return opaque;

	auto alpha = GetPool<AlphaBlendTrait>().Find(id.Literal());
	if (alpha) return alpha;

	auto transparency = GetPool<TransparencyTrait>().Find(id.Literal());
	return transparency;
}

void MeshManager::ComputeWorldSize(
	Vector3& refWorldMin, Vector3& refWorldMax,
	const TransformPool& tfPool) const
{
	auto Compute = [&refWorldMin, &refWorldMax, &tfPool](auto& vector)
	{
		for (auto& iter : vector)
		{
			ObjectId id = iter.GetObjectId();

			const Transform* tf = tfPool.Find(id.Literal()); assert(tf);
			iter.CalcWorldSize(refWorldMin, refWorldMax, *tf);
		}
	};

	Compute(GetPool<OpaqueTrait>().GetVector());
	Compute(GetPool<AlphaBlendTrait>().GetVector());
	Compute(GetPool<TransparencyTrait>().GetVector());
}

void MeshManager::UpdateTransformCB(DirectX& dx, const Core::TransformPool& tfPool)
{
	auto Update = [&dx, &tfPool](auto& pool)
	{		
		uint size = pool.GetSize();
		for (uint i=0; i<size; ++i)
		{
			auto& mesh = pool.Get(i);
			uint id = mesh.GetObjectId().Literal();
			
			const Transform* tf = tfPool.Find(id); assert(tf);
			mesh.UpdateTransformCB(dx, *tf);
		}
	};

	Update(GetPool<OpaqueTrait>());
	Update(GetPool<AlphaBlendTrait>());
	Update(GetPool<TransparencyTrait>());
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
				pool.Delete(mesh.GetObjectId().Literal());
			}
		}
	};

	Check(GetPool<OpaqueTrait>());
	Check(GetPool<AlphaBlendTrait>());
	Check(GetPool<TransparencyTrait>());

	for (auto iter : changedMeshes)
	{
		if( iter.GetTrait() == Trait::Opaque )
			GetPool<OpaqueTrait>().Add(iter.GetObjectId().Literal(), iter);
		else if(iter.GetTrait() == Trait::AlphaBlend )
			GetPool<AlphaBlendTrait>().Add(iter.GetObjectId().Literal(), iter);
		else if (iter.GetTrait() == Trait::Transparency)
			GetPool<TransparencyTrait>().Add(iter.GetObjectId().Literal(), iter);

		iter.ClearDirty();
	}
}
