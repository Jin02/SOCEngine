#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "ComponentSystem.h"
#include "Transform.h"

using namespace Core;

Object& ObjectManager::Acquire(const std::string& name)
{
	ObjectID key = _objIDMgr.Acquire();
	_idBookmark.Add(name, key.Literal());

	return _objects.Add(key, Object::Create(key, name));
}

void ObjectManager::Delete(const std::string& name)
{
	uint objLiteralID	= _idBookmark.Find(name);
	ObjectID id			= ObjectID(objLiteralID);

	_objects.Find(id)->Destroy();
	_objects.Delete(id);

	_idBookmark.Delete(name);
	_objIDMgr.Delete(id);
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
	_objects.Iterate([](Object& obj){ obj.Destroy(); });
	_objects.DeleteAll();
	_idBookmark.DeleteAll();
		
	_objIDMgr.DeleteAll();
}
