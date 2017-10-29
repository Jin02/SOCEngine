#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "ComponentSystem.hpp"
#include "Transform.h"

using namespace Core;

Object& ObjectManager::Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool)
{
	ObjectID key = _objIDMgr.Acquire();
	_idBookmark.Add(name, key.Literal());

	return _objects.Add(key, Object(key));
}

void ObjectManager::Delete(const std::string& name)
{
	uint objLiteralID = _idBookmark.Find(name);

	_objects.Delete(ObjectID(objLiteralID));
	_idBookmark.Delete(name);
	_objIDMgr.Delete(ObjectID(objLiteralID));
}

bool ObjectManager::Has(const std::string& name) const
{
	return _idBookmark.Has(name);
}

Object* ObjectManager::Find(const std::string& name)
{
	return _objects.Find( ObjectID(_idBookmark.Find(name)) );
}

const Object* ObjectManager::Find(const std::string& name) const
{
	return _objects.Find(ObjectID(_idBookmark.Find(name)));
}

void Core::ObjectManager::Delete(ObjectID id)
{
	Object* findObj = _objects.Find(id);
	if(findObj == nullptr) return;

	_idBookmark.Delete(findObj->GetName());
	_objects.Delete(id);
	_objIDMgr.Delete(id);
}

bool Core::ObjectManager::Has(ObjectID id) const
{
	return _objects.Has(id);
}

Object* Core::ObjectManager::Find(ObjectID id)
{
	return _objects.Find(id);
}

const Object* Core::ObjectManager::Find(ObjectID id) const
{
	return _objects.Find(id);
}

void ObjectManager::DeleteAll()
{
	_objects.DeleteAll();
	_idBookmark.DeleteAll();
		
	_objIDMgr.DeleteAll();
}