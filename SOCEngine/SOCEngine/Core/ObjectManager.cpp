#include "ObjectManager.h"
#include "Object.hpp"
#include "Engine.h"

using namespace Core;

Object& ObjectManager::Add(const std::string & name)
{
	_objects.Add(name, Object(_objIdMgr.Acquire(), _engine.GetComponentSystem(), _engine.GetTransformPool()));
	return _objects.Get(_objects.GetSize() - 1);
}

void ObjectManager::Delete(const std::string& name)
{
	ObjectId id = _objects.Find(name)->GetId();

	_objects.Delete(name);
	_objIdMgr.Delete(id);
}

bool ObjectManager::Has(const std::string& name) const
{
	return _objects.GetIndexer().Has(name);
}

auto ObjectManager::Find(const std::string& name)
{
	return _objects.Find(name);
}

void ObjectManager::DeleteAll()
{
	_objects.DeleteAll();
	_objIdMgr.DeleteAll();
}
