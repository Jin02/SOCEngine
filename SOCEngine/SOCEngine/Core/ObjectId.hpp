#pragma once

#include "UniqueIDManager.hpp"
#include "UniqueID.hpp"
#include "VectorIndexer.hpp"

namespace Core
{
	class ObjectID : public UniqueID
	{
	private:
		operator UniqueID::LiteralType() const { return Literal(); }

	public:
		using UniqueID::UniqueID;

		using IndexMap = Core::IndexMap<ObjectID::LiteralType>;
		using IndexHashMap = Core::IndexHashMap<ObjectID::LiteralType>;

	public:
		friend class ObjectIDManager;
		friend class ObjectManager;
		friend class Indexer<ObjectID, std::unordered_map<ObjectID::LiteralType, uint>>;
		friend class Indexer<ObjectID, std::map<ObjectID::LiteralType, uint>>;
	};


	class ObjectIDManager : public UniqueIDManager
	{
	public:
		ObjectID Acquire()
		{
			uint literalID = UniqueIDManager::Acquire();
			return ObjectID(literalID);
		}

		bool Has(ObjectID id)
		{
			return UniqueIDManager::Has(id.Literal());
		}

		void Delete(ObjectID id)
		{
			UniqueIDManager::Delete(id.Literal());
		}
	};
}