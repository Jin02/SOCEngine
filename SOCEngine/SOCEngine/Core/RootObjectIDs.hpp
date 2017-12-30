#pragma once

#include "ObjectId.hpp"
#include "VectorIndexer.hpp"

namespace Core
{
	class RootObjectIDs final
	{
	public:
		RootObjectIDs() = default;
		DISALLOW_ASSIGN_COPY(RootObjectIDs);

		ObjectID	Add(ObjectID root)		{ return ObjectID( _rootIDs.Add(root.Literal(), root) ); }
		void		Delete(ObjectID root)	{ _rootIDs.Delete(root.Literal()); }
		bool		Has(ObjectID root)		{ return _rootIDs.Has(root.Literal()); }
		ObjectID	Find(ObjectID id)	
		{
			ObjectID* find = _rootIDs.Find(id.Literal());
			return find ? *find : ObjectID();
		}

		GET_CONST_ACCESSOR(Vector, auto&, _rootIDs.GetVector());

	private:
		Core::VectorHashMap<ObjectID::LiteralType, ObjectID>	_rootIDs;
	};
}