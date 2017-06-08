#pragma once

#include "UniqueIdManager.hpp"
#include "UniqueId.hpp"

namespace Core
{
	class ObjectId : public UniqueId
	{
	private:
		operator UniqueId::LiteralType() const { return Literal(); }

	public:
		using UniqueId::UniqueId;

	public:
		friend class ObjectIdManager;
		friend class Transform;
		friend class Object;
	};

	class ObjectIdManager : public UniqueIdManager<ObjectId>
	{
	public:
		ObjectId Acquire()
		{
			uint literalId = UniqueIdManager<ObjectId>::Acquire();
			return ObjectId(literalId);
		}
	};
}