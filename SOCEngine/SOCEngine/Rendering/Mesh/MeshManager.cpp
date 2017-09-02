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
	return GetPool<RenderMethod::Opaque>().Add(objID.Literal(), mesh);
}

void MeshManager::Delete(Core::ObjectID objID)
{
	GetPool<RenderMethod::Opaque>().Delete(objID.Literal());
	GetPool<RenderMethod::AlphaBlend>().Delete(objID.Literal());
	GetPool<RenderMethod::Transparency>().Delete(objID.Literal());
}

bool MeshManager::Has(Core::ObjectID objID) const
{
	return	GetPool<RenderMethod::Opaque>().Has(objID.Literal())		|
			GetPool<RenderMethod::AlphaBlend>().Has(objID.Literal())	|
			GetPool<RenderMethod::Transparency>().Has(objID.Literal());
}

Mesh* MeshManager::Find(Core::ObjectID id)
{
	auto opaque = GetPool<RenderMethod::Opaque>().Find(id.Literal());
	if (opaque) return opaque;

	auto alpha = GetPool<RenderMethod::AlphaBlend>().Find(id.Literal());
	if (alpha) return alpha;

	auto transparency = GetPool<RenderMethod::Transparency>().Find(id.Literal());
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

	Check(GetPool<RenderMethod::Opaque>());
	Check(GetPool<RenderMethod::AlphaBlend>());
	Check(GetPool<RenderMethod::Transparency>());
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