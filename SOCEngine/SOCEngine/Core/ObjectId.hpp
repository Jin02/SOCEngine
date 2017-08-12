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
		friend class ObjectManager;
	};

	class ObjectIdManager : public UniqueIdManager
	{
	public:
		ObjectId Acquire()
		{
			uint literalId = UniqueIdManager::Acquire();
			return ObjectId(literalId);
		}

		bool Has(ObjectId id)
		{
			return UniqueIdManager::Has(id.Literal());
		}

		void Delete(ObjectId id)
		{
			UniqueIdManager::Delete(id.Literal());
		}
	};
}