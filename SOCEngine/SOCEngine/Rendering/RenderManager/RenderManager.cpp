#include "RenderManager.h"

using namespace Rendering;
using namespace Rendering::Manager;

RenderManager::RenderManager()
{
}

RenderManager::~RenderManager()
{
	_alphaMeshes.DeleteAll(true);
	_nonAlphaMeshes.DeleteAll(true);
}

bool RenderManager::Add(BasicMaterial* material, Mesh::Mesh* mesh, MeshType type)
{
	unsigned int materialAddress = reinterpret_cast<unsigned int>(material);
	unsigned int meshAddress = reinterpret_cast<unsigned int>(mesh);

	std::pair<BasicMaterial*, Mesh::Mesh*>* pair = Find(material, mesh, type);
	if(pair == nullptr)
		pair = new std::pair<BasicMaterial*, Mesh::Mesh*>(material, mesh);

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

void RenderManager::Change(const BasicMaterial* material, const Mesh::Mesh* mesh, MeshType type)
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

std::pair<BasicMaterial*, Mesh::Mesh*>* RenderManager::Find(BasicMaterial* material, Mesh::Mesh* mesh, MeshType type)
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

void RenderManager::Iterate(const std::function<void(BasicMaterial* material, Mesh::Mesh* mesh)>& recvFunc, MeshType type) const
{
	auto MapInMapIter = [&](Structure::Map<unsigned int, std::pair<BasicMaterial*, Mesh::Mesh*>>* content)
	{
		auto MapIter = [&](std::pair<BasicMaterial*, Mesh::Mesh*>* content)
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