#include "MeshManager.h"

using namespace Rendering;
using namespace Rendering::Manager;

MeshManager::MeshManager()
{
}

MeshManager::~MeshManager()
{
	_alphaMeshes.DeleteAll(true);
	_nonAlphaMeshes.DeleteAll(true);
}

bool MeshManager::Add(Material* material, Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	std::pair<Material*, Mesh::Mesh*>* pair = Find(material, mesh, type);
	if(pair == nullptr)
		pair = new std::pair<Material*, Mesh::Mesh*>(material, mesh);

	if(type == MeshType::hasAlpha)
		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
	else
	{
		DEBUG_LOG("undeclartion MeshType");
		return false;
	}

	return true;
}

void MeshManager::Change(const Material* material, const Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	if( type == MeshType::hasAlpha )
	{
		auto pair = _nonAlphaMeshes.Find(materialAddress, meshAddress);
		_nonAlphaMeshes.Delete(materialAddress, meshAddress, false);

		_alphaMeshes.Add(materialAddress, meshAddress, pair);
	}
	else if( type == MeshType::nonAlpha )
	{
		auto pair = _alphaMeshes.Find(materialAddress, meshAddress);
		_alphaMeshes.Delete(materialAddress, meshAddress, false);

		_nonAlphaMeshes.Add(materialAddress, meshAddress, pair);
	}
}

std::pair<Material*, Mesh::Mesh*>* MeshManager::Find(Material* material, Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);
	
	if(type == MeshType::hasAlpha)
		return _alphaMeshes.Find(materialAddress, meshAddress);
	else if(type == MeshType::nonAlpha)
		return _nonAlphaMeshes.Find(materialAddress, meshAddress);

	ASSERT("Error!, undefined MeshType");
	return nullptr;
}

void MeshManager::Iterate(const std::function<void(Material* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const
{
	auto MapInMapIter = [&](Structure::Map<unsigned int, std::pair<Material*, Mesh::Mesh*>>* content)
	{
		auto MapIter = [&](std::pair<Material*, Mesh::Mesh*>* content)
		{
			recvFunc(content->first, content->second);
		};
		content->IterateContent(MapIter);
	};

	if(type == MeshType::hasAlpha)
		_alphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else if(type == MeshType::nonAlpha)
		_nonAlphaMeshes.GetMapInMap().IterateContent(MapInMapIter);
	else
	{
		ASSERT("Error!, undefined MeshType");
	}
}