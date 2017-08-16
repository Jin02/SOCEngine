#include "ObjectManager.h"
#include "Object.h"
#include "Engine.h"
#include "ComponentSystem.hpp"
#include "Transform.h"

using namespace Core;

Object& ObjectManager::Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool)
{
	ObjectId key = _objIdMgr.Acquire();
	_toIndex.Add(name, key.Literal());

	_newObjectIds.push_back(key);
	return _objects.Add(key, Object(key, compoSystem, tfPool, this));
}

void ObjectManager::Delete(const std::string& name)
{
	uint objLiteralId = _toIndex.Find(name);

	uint size = _rootObjectIds.GetSize();
	for(uint i=0; i<size;)
	{
		uint id = _rootObjectIds.Get(i);
		if (id != objLiteralId)
			continue;

		_rootObjectIds.Delete(id);
		break;
	}

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
	return _objects.Find( ObjectId(_toIndex.Find(name)) );
}

void Core::ObjectManager::Delete(ObjectId id)
{
	Object* findObj = _objects.Find(id);
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

void ObjectManager::CheckRootObjectIDs(const TransformPool& tfPool)
{
	for (uint newID : _newObjectIds)
	{
		auto tf = tfPool.Find(newID); assert(tf);
		uint parentId = tf->GetParentId().Literal();

		if (parentId == ObjectId::Undefined())
		{
			if (_rootObjectIds.GetIndexer().Has(newID) == false)
				_rootObjectIds.Add(newID, newID);
		}
	}

	_newObjectIds.clear();
}

void ObjectManager::AddNewRootObject(ObjectId id)
{
	_newObjectIds.push_back(id.Literal());
}

void ObjectManager::DeleteRootObject(ObjectId id)
{
	_rootObjectIds.Delete(id);
}
