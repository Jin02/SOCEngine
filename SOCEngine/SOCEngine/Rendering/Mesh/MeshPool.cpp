#include "MeshPool.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;

Mesh& VBSortedMeshPool::Add(Mesh& mesh)
{
	const auto& vbKey = mesh.GetVBKey();
	assert(_bookmark.Has(mesh.GetObjectID().Literal()) == false);

	_bookmark.Add(mesh.GetObjectID().Literal(), vbKey);

	if (HasVBKey(vbKey) == false)
	{
		MeshRawPool first;
		first.Add(mesh);

		return _pool.Add(vbKey, first).Get(0);
	}

//	else
	auto rawPool = _pool.Find(vbKey);
	assert(rawPool->Has(mesh.GetObjectID().Literal()) == false);
	return rawPool->Add(mesh);
}

void VBSortedMeshPool::Delete(Core::ObjectID::LiteralType id)
{
	uint vbKey = _bookmark.Find(id);
	assert(vbKey != decltype(_bookmark)::Fail());

	auto rawPool = _pool.Find(vbKey);
	rawPool->Delete(id);
	_bookmark.Delete(id);

	if (rawPool->Empty())
		_pool.Delete(vbKey);
}

bool VBSortedMeshPool::Has(Core::ObjectID::LiteralType id) const
{
	uint vbKey = _bookmark.Find(id);
	return (vbKey != decltype(_bookmark)::Fail()) ? 
		_pool.Find(vbKey)->Has(id) : false;
}

Mesh* VBSortedMeshPool::Find(Core::ObjectID::LiteralType id)
{
	uint vbKey = _bookmark.Find(id);
	return (vbKey != decltype(_bookmark)::Fail()) ?
		_pool.Find(vbKey)->Find(id) : nullptr;
}

bool VBSortedMeshPool::HasVBKey(VertexBuffer::Key key) const
{
	return _pool.Has(key);
}