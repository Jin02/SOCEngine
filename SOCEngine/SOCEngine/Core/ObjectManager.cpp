#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "ComponentSystem.hpp"

using namespace Core;

Object& ObjectManager::Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool)
{
	_objects.Add(name, Object(_objIdMgr.Acquire(), compoSystem, tfPool));
	return _objects.Get(_objects.GetSize() - 1);
}

void ObjectManager::Delete(const std::string& name)
{
	ObjectId id = _objects.Find(name)->GetObjectId();

	_objects.Delete(name);
	_objIdMgr.Delete(id);
}

bool ObjectManager::Has(const std::string& name) const
{
	return _objects.GetIndexer().Has(name);
}

Object* ObjectManager::Find(const std::string& name)
{
	return _objects.Find(name);
}

void ObjectManager::DeleteAll()
{
	_objects.DeleteAll();
	_objIdMgr.DeleteAll();
}
