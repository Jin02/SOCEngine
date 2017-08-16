#pragma once

#include "UniqueIDManager.hpp"
#include "UniqueID.hpp"
#include "VectorIndexer.hpp"

namespace Rendering
{
	class MaterialID : public Core::UniqueID
	{
	private:
		operator Core::UniqueID::LiteralType() const { return Literal(); }

	public:
		using Core::UniqueID::UniqueID;

		using IndexMap = Core::IndexMap<MaterialID::LiteralType>;
		using IndexHashMap = Core::IndexHashMap<MaterialID::LiteralType>;

	public:
		friend class MaterialIDManager;
		friend class Core::Indexer<MaterialID, std::unordered_map<MaterialID::LiteralType, uint>>;
		friend class Core::Indexer<MaterialID, std::map<MaterialID::LiteralType, uint>>;
	};

	class MaterialIDManager : public Core::UniqueIDManager
	{
	public:
		MaterialID Acquire()
		{
			uint literalID = Core::UniqueIDManager::Acquire();
			return MaterialID(literalID);
		}

		bool Has(MaterialID id)
		{
			return UniqueIDManager::Has(id.Literal());
		}

		void Delete(MaterialID id)
		{
			UniqueIDManager::Delete(id.Literal());
		}
	};
}