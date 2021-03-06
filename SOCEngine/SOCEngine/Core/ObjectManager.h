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
		Object&			Acquire(const std::string& name);
		void			Delete(const std::string& name);
		bool			Has(const std::string& name) const;
		Object*			Find(const std::string& name);
		const Object*	Find(const std::string& name) const;

		void			Delete(ObjectID id);
		bool			Has(ObjectID id) const;
		Object*			Find(ObjectID id);
		const Object*	Find(ObjectID id) const;

		void			DeleteAll();

		GET_CONST_ACCESSOR(ObjectIDManager, const ObjectIDManager&, _objIDMgr);

	private:
		VectorHashMap<ObjectID::LiteralType, Object>		_objects;
		BookHashMapmark<std::string>						_idBookmark;

		ObjectIDManager										_objIDMgr;
	};
}
