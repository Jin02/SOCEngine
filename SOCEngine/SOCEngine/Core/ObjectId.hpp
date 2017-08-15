#pragma once

#include "UniqueIdManager.hpp"
#include "UniqueId.hpp"
#include "VectorIndexer.hpp"

namespace Core
{
	class ObjectId : public UniqueId
	{
	private:
		operator UniqueId::LiteralType() const { return Literal(); }

	public:
		using UniqueId::UniqueId;

		using IndexMap = Core::IndexMap<Core::ObjectId::LiteralType>;
		using IndexHashMap = Core::IndexHashMap<Core::ObjectId::LiteralType>;

	public:
		friend class ObjectIdManager;
		friend class ObjectManager;
		friend class Indexer<ObjectId, std::unordered_map<ObjectId::LiteralType, uint>>;
		friend class Indexer<ObjectId, std::map<ObjectId::LiteralType, uint>>;
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