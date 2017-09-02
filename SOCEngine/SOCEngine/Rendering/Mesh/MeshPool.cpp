#include "MeshPool.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;

Mesh& VBSortedMeshPool::Add(Mesh& mesh)
{
	const auto& vbKey = mesh.GetVBKey();
	assert(_marker.Has(mesh.GetObjectID().Literal()) == false);

	_marker.Add(mesh.GetObjectID().Literal(), vbKey);

	if (HasVBKey(vbKey) == false)
	{
		MeshRawPool first;
		first.Add(mesh);

		return _pool.Add(vbKey, first).Get(0);
	}

//	else
	auto rawPool = _pool.Find(vbKey);
	assert(rawPool->Has(mesh.GetObjectID()) == false);
	return rawPool->Add(mesh);
}

void VBSortedMeshPool::Delete(Core::ObjectID id)
{
	uint literalID = id.Literal();
	uint vbKey = _marker.Find(literalID);
	assert(vbKey != decltype(_marker)::Fail());

	auto rawPool = _pool.Find(vbKey);
	rawPool->Delete(id);
	_marker.Delete(literalID);

	if (rawPool->Empty())
		_pool.Delete(vbKey);
}

bool VBSortedMeshPool::Has(Core::ObjectID id) const
{
	uint vbKey = _marker.Find(id.Literal());
	return (vbKey != decltype(_marker)::Fail()) ? 
		_pool.Find(vbKey)->Has(id) : false;
}

Mesh* VBSortedMeshPool::Find(Core::ObjectID id)
{
	uint vbKey = _marker.Find(id.Literal());
	return (vbKey != decltype(_marker)::Fail()) ?
		_pool.Find(vbKey)->Find(id) : nullptr;
}

bool VBSortedMeshPool::HasVBKey(VertexBuffer::Key key) const
{
	return _pool.Has(key);
}