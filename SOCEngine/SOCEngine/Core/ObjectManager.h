#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectIdManager.h"
#include "Singleton.h"

namespace Core
{
	class Object;
	class Engine;
	class ObjectManager final : private Singleton<ObjectManager>
	{
	public:
		ObjectManager() = default;
		DISALLOW_ASSIGN_COPY(ObjectManager);

	public:
		static std::shared_ptr<Object> Add(const std::string& name);
		static void Delete(const std::string& name);
		static bool Has(const std::string& name);
		static auto Find(const std::string& name);
		static void DeleteAll();

	public:
		GET_ACCESSOR(ObjectIdManager, ObjectIdManager&, _objIdMgr);
		SET_ACCESSOR(Engine, Engine*, _engine);
		GET_CONST_ACCESSOR(Objects, const std::vector<Object>&, _objects.GetVector());

	private:
		static void ObjectDeleter(Object* obj);

	private:
		VectorHashMap<std::string, Object>	_objects;
		ObjectIdManager						_objIdMgr;
		Engine*								_engine;
	};
}