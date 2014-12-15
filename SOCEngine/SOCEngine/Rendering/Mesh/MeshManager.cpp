#include "MeshManager.h"

using namespace Rendering;
using namespace Rendering::Manager;

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
}
bool MeshManager::Add(Mesh::Mesh* mesh, MeshType type)
{
	unsigned int key = reinterpret_cast<unsigned int>(mesh);
	if(type == MeshType::hasAlpha)
		_alphaMeshes.Add(key, mesh);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.Add(key, mesh);
	else
	{
		DEBUG_LOG("undeclartion MeshType");
		return false;
	}

	return true;
}
void MeshManager::Change(Mesh::Mesh* mesh, MeshType type)
{
	unsigned int key = reinterpret_cast<unsigned int>(mesh);

	if( type == MeshType::hasAlpha )
	{
		_nonAlphaMeshes.Delete(key);
		_alphaMeshes.Add(key, mesh);
	}
	else if( type == MeshType::nonAlpha )
	{
		_alphaMeshes.Delete(key);
		_nonAlphaMeshes.Add(key, mesh);
	}
}
Mesh::Mesh* MeshManager::Find(unsigned int meshAddress, MeshType type)
{
	if(type == MeshType::hasAlpha)
		return _alphaMeshes.Find(meshAddress);
	else if(type == MeshType::nonAlpha)
		return _nonAlphaMeshes.Find(meshAddress);

	ASSERT("Error!, undefined MeshType");
	return nullptr;
}
Mesh::Mesh* MeshManager::Find(unsigned int meshAddress, MeshType* outType)
{
	Mesh::Mesh* res = Find(meshAddress, MeshType::hasAlpha);
	if(res)
	{
		if(outType)
			(*outType) = MeshType::hasAlpha;
		return res;
	}
	else
	{
		res = Find(meshAddress, MeshType::nonAlpha);
		if(res)
		{
			if(outType)
				(*outType) = MeshType::nonAlpha;
			return res;
		}
	}

	return nullptr;
}

void MeshManager::Iterate(std::function<void(Mesh::Mesh* mesh)>& recvFunc, MeshType type)
{
	if(type == MeshType::hasAlpha)
	{
		auto map = _alphaMeshes.GetMap();
		for(auto iter = map.begin(); iter != map.end(); ++iter)
			recvFunc(iter->second.second);
	}
	else if(type == MeshType::nonAlpha)
	{
		auto map = _nonAlphaMeshes.GetMap();
		for(auto iter = map.begin(); iter != map.end(); ++iter)
			recvFunc(iter->second.second);
	}
	else
	{
		ASSERT("Error!, undefined MeshType");
	}
}