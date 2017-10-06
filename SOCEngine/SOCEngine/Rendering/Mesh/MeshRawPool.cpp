#include "MeshRawPool.h"

using namespace Rendering;
using namespace Rendering::Geometry;

MeshRawPool::MeshRawPool(const std::initializer_list<Mesh>& meshes)
{
	_meshes.insert(_meshes.end(), meshes.begin(), meshes.end());
}

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
