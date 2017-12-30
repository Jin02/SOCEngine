#include "VBSortedMeshPool.h"

using namespace Rendering;
using namespace Rendering::Geometry;
using namespace Rendering::Buffer;
using namespace Core;

Mesh& VBSortedMeshPool::Add(ObjectID id, BaseBuffer::Key vbKey, Mesh& mesh)
{
	auto literalID = id.Literal();
	assert(_bookmark.Has(literalID) == false);

	_bookmark.Add(literalID, vbKey);

	auto rawPool = _pool.Find(vbKey);
	if (rawPool == nullptr)
		return _pool.Add(vbKey, { mesh }).Get(0);

	assert(rawPool->Has(literalID) == false);
	return rawPool->Add(mesh);
}
			
void VBSortedMeshPool::Delete(ObjectID::LiteralType literalID)
{
	uint vbKey = _bookmark.Find(literalID);
	assert(vbKey != decltype(_bookmark)::Fail());

	auto rawPool = _pool.Find(vbKey);
	rawPool->Delete(literalID);

	if (rawPool->Empty())
		_pool.Delete(vbKey);				
	
	_bookmark.Delete(literalID);			
}

void VBSortedMeshPool::DeleteAll()
{
	_bookmark.DeleteAll();
	_pool.DeleteAll();
}

bool VBSortedMeshPool::Has(ObjectID::LiteralType literalID) const
{				
	uint vbKey = _bookmark.Find(literalID);
	return (vbKey != decltype(_bookmark)::Fail()) ? _pool.Find(vbKey)->Has(literalID) : false;							
}

const Mesh* VBSortedMeshPool::Find(ObjectID::LiteralType literalID) const
{
	uint vbKey = _bookmark.Find(literalID);
	return (vbKey != decltype(_bookmark)::Fail()) ? _pool.Find(vbKey)->Find(literalID) : nullptr;				
}

bool VBSortedMeshPool::HasVBKey(VertexBuffer::Key key) const
{
	return _pool.Has(key);				
}
