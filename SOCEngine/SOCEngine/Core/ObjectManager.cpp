#include "ObjectManager.h"
#include "Object.hpp"

using namespace Core;

void ObjectManager::ObjectDeleter(Object* obj)
{
	ObjectManager* objMgr = ObjectManager::SharedInstance();
	objMgr->Delete(obj->GetName());
}

std::shared_ptr<Object> ObjectManager::Add(const std::string & name)
{
	ObjectManager* objMgr = SharedInstance();
	objMgr->_objects.Add(name, Object(objMgr->_objIdMgr.Acquire(), objMgr->_engine));

	Object& obj = objMgr->_objects.Get(objMgr->_objects.GetSize() - 1);
	return std::shared_ptr<Object>(&obj, ObjectDeleter);
}

void ObjectManager::Delete(const std::string & name)
{
	ObjectManager* objMgr = SharedInstance();
	auto find = objMgr->_objects.Find(name);
	if (find == nullptr)
		return;

	objMgr->_objIdMgr.Delete(find->GetId());
	objMgr->_objects.Delete(name);
}

bool ObjectManager::Has(const std::string & name)
{
	ObjectManager* objMgr = SharedInstance();
	return objMgr->_objects.GetIndexer().Has(name);
}

auto ObjectManager::Find(const std::string & name)
{
	ObjectManager* objMgr = SharedInstance();
	return objMgr->_objects.Find(name);
}

void ObjectManager::DeleteAll()
{
	ObjectManager* objMgr = SharedInstance();
	objMgr->_objects.DeleteAll();
}
