#include "MeshRawPool.h"

using namespace Rendering;
using namespace Rendering::Geometry;

Mesh* MeshRawPool::Find(Core::ObjectID::LiteralType id)
{
	for (auto& iter : _meshes)
	{
		if( iter.GetObjectID().Literal() == id )
			return &iter;
	}

	return nullptr;
}

void MeshRawPool::Delete(Core::ObjectID::LiteralType id)
{
	for (auto iter = _meshes.begin(); iter != _meshes.end(); ++iter)
	{
		if (iter->GetObjectID().Literal() == id)
		{
			_meshes.erase(iter);
			return;
		}
	}
}

bool MeshRawPool::Has(Core::ObjectID::LiteralType id) const
{
	for (auto& iter : _meshes)
	{
		if (iter.GetObjectID().Literal() == id)
			return true;
	}

	return false;
}

void MeshRawPool::DeleteAll()
{
	_meshes.clear();
}
