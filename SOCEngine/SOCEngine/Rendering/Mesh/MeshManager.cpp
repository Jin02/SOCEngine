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
	AddMeshIDToTraitArray<OpaqueTrait>(mesh);

	return _pool.Add(objID.Literal(), mesh);
}
void MeshManager::Delete(Core::ObjectID objID)
{
	auto mesh = _pool.Find(objID.Literal());
	DeleteMeshIDToTraitArray<OpaqueTrait>(*mesh);
	_pool.Delete(objID.Literal());
}
bool MeshManager::Has(Core::ObjectID objID) const
{
	return _pool.Has(objID.Literal());
}
Mesh* MeshManager::Find(Core::ObjectID id)
{
	return _pool.Find(id.Literal());
}

void MeshManager::CheckTraitWithDirty(const Core::TransformPool& tfPool)
{
	auto& dirty = _dirtyMeshes;

	uint size = _pool.GetSize();
	for (uint i = 0; i<size; ++i)
	{
		Mesh& mesh = _pool.Get(i);
		auto tf = tfPool.Find(mesh.GetObjectID().Literal()); assert(tf);

		if (tf->GetDirty())
			dirty.push_back(&mesh);

		if (mesh.GetChangedTrait())
		{
			/*
			기존껀? 어딘가에 추가되있을거야
			그럼 그 값을 가져오고, 기존에 있는걸 제거해
			그리고 가져온 값을 새곳에 추가해. 그럼 되잖아.
			*/


			if (mesh.GetPrevTrait() == Trait::Opaque)
			{
			}
			else if (mesh.GetPrevTrait() == Trait::AlphaBlend)
			{

			}
			else if (mesh.GetPrevTrait() == Trait::Transparency)
			{

			}
		}
	}
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

void MeshManager::ClearDirty()
{
	for (auto iter : _dirtyMeshes)
		iter->ClearDirty();

	_dirtyMeshes.clear();
}
