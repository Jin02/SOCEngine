#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectIdManager.h"
#include "Singleton.h"
#include "Object.hpp"

namespace Core
{
	class Engine;
	class ObjectManager final
	{
	public:
		ObjectManager(Engine& engine) : _engine(engine) {}
		DISALLOW_ASSIGN_COPY(ObjectManager);

	public:
		Object& Add(const std::string& name);
		void Delete(const std::string& name);
		bool Has(const std::string& name) const;
		auto Find(const std::string& name);
		void DeleteAll();

		GET_ACCESSOR(ObjectIdManager, ObjectIdManager&, _objIdMgr);

	private:
		VectorHashMap<std::string, Object>	_objects;
		ObjectIdManager						_objIdMgr;
		Engine&								_engine;
	};
}