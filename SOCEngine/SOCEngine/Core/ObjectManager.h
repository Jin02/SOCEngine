#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectID.hpp"
#include "Singleton.h"
#include "Object.h"

namespace Core
{
	class ComponentSystem;
	class TransformPool;
	class ObjectManager final
	{
	public:
		ObjectManager() = default;
		DISALLOW_ASSIGN_COPY(ObjectManager);

		friend class Object;

	public:
		Object&		Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool);
		void		Delete(const std::string& name);
		bool		Has(const std::string& name) const;
		Object*		Find(const std::string& name);

		void		Delete(ObjectID id);
		bool		Has(ObjectID id) const;
		Object*		Find(ObjectID id);

		void		DeleteAll();

	public:
		void CheckRootObjectIDs(const TransformPool& tfPool);
		void AddNewRootObject(ObjectID id);
		void DeleteRootObject(ObjectID id);

		GET_CONST_ACCESSOR(RootObjectIDs, const auto&, _rootObjectIDs.GetVector());

	private:
		GET_ACCESSOR(ObjectIDManager, ObjectIDManager&, _objIDMgr);

	private:
		VectorHashMap<ObjectID::LiteralType, Object>		_objects;
		std::vector<ObjectID::LiteralType>					_newObjectIDs;

		VectorHashMap<	ObjectID::LiteralType,
						ObjectID::LiteralType>				_rootObjectIDs;

		IndexHashMap<std::string>							_toIndex;

		ObjectIDManager										_objIDMgr;
	};
}