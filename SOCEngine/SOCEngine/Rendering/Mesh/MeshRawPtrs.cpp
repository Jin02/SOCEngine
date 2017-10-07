#include "MeshRawPtr.h"

using namespace Rendering;
using namespace Rendering::Geometry;

MeshRawPtrs::MeshRawPtrs(const std::initializer_list<MeshRawPtrs::PtrType>& meshes)
{
	_meshes.insert(_meshes.end(), meshes.begin(), meshes.end());
}

MeshRawPtrs::PtrType MeshRawPtrs::Find(Core::ObjectID::LiteralType id) const
{
	for (auto& iter : _meshes)
	{
		if( iter->GetObjectID().Literal() == id )
			return &iter;
	}

	return nullptr;
}

void MeshRawPtrs::Delete(Core::ObjectID::LiteralType id)
{
	for (auto& iter : _meshes)
	{
		if (iter->GetObjectID().Literal() == id)
		{
			_meshes.erase(iter);
			return;
		}
	}
}

bool MeshRawPtrs::Has(Core::ObjectID::LiteralType id) const
{
	for (auto& iter : _meshes)
	{
		if (iter->GetObjectID().Literal() == id)
			return true;
	}

	return false;
}

void MeshRawPtrs::DeleteAll()
{
	_meshes.clear();
}
