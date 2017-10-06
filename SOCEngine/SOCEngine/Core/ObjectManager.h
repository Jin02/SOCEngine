#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"
#include "ObjectID.hpp"
#include "Singleton.h"
#include "Object.h"
#include "Bookmark.hpp"

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
		Object&			Add(const std::string& name, ComponentSystem* compoSystem, TransformPool* tfPool);
		void			Delete(const std::string& name);
		bool			Has(const std::string& name) const;
		Object*			Find(const std::string& name);
		const Object*	Find(const std::string& name) const;

		void			Delete(ObjectID id);
		bool			Has(ObjectID id) const;
		Object*			Find(ObjectID id);
		const Object*	Find(ObjectID id) const;

		void		DeleteAll();

	private:
		GET_ACCESSOR(ObjectIDManager, ObjectIDManager&, _objIDMgr);

	private:
		VectorHashMap<ObjectID::LiteralType, Object>		_objects;
		BookHashMapmark<std::string>						_idBookmark;

		ObjectIDManager										_objIDMgr;
	};
}