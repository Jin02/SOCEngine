#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectId.hpp"
#include "Singleton.h"
#include "Object.h"

namespace Core
{
	class ComponentSystem;
	class ObjectManager final
	{
	public:
		ObjectManager() = default;
		DISALLOW_ASSIGN_COPY(ObjectManager);

	public:
		Object& Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool);
		void Delete(const std::string& name);
		bool Has(const std::string& name) const;
		Object* Find(const std::string& name);
		void DeleteAll();

		GET_ACCESSOR(ObjectIdManager, ObjectIdManager&, _objIdMgr);

	private:
		VectorHashMap<std::string, Object>	_objects;
		ObjectIdManager						_objIdMgr;
	};
}