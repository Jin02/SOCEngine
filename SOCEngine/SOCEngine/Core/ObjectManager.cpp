#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "ComponentSystem.hpp"

using namespace Core;

Object& ObjectManager::Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool)
{
	ObjectId key = _objIdMgr.Acquire();
	_toIndex.Add(name, key.Literal());

	return _objects.Add(key, Object(key, compoSystem, tfPool));
}

void ObjectManager::Delete(const std::string& name)
{
	uint objLiteralId = _toIndex.Find(name);

	_objects.Delete(ObjectId(objLiteralId));
	_toIndex.Delete(name);
	_objIdMgr.Delete(ObjectId(objLiteralId));
}

bool ObjectManager::Has(const std::string& name) const
{
	return _toIndex.Has(name);
}

Object* ObjectManager::Find(const std::string& name)
{
	return _objects.Find(_toIndex.Find(name));
}

void Core::ObjectManager::Delete(ObjectId id)
{
	Object* findObj = _objects.Find(id.Literal());
	if(findObj == nullptr) return;

	_toIndex.Delete(findObj->GetName());
	_objects.Delete(id);
	_objIdMgr.Delete(id);
}

bool Core::ObjectManager::Has(ObjectId id) const
{
	return _objects.GetIndexer().Has(id);
}

Object * Core::ObjectManager::Find(ObjectId id)
{
	return _objects.Find(id);
}

void ObjectManager::DeleteAll()
{
	_objects.DeleteAll();
	_toIndex.DeleteAll();
		
	_objIdMgr.DeleteAll();
}
