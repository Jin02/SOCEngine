#pragma once

#include "Common.h"
#include "VectorIndexer.hpp"

namespace Core
{
	template <typename T>
	class UniqueID
	{
	public:
		using LiteralType = uint;

		bool operator==(const UniqueID& rhs) const
		{
			return _id == rhs._id;
		}
		bool operator!=(const UniqueID& rhs) const
		{
			return !operator==(rhs);
		}

		static constexpr LiteralType Undefined() { return -1; }
		UniqueID() : _id(Undefined()) {}

	public:
		using IndexMap		= Core::IndexMap<LiteralType>;
		using IndexHashMap	= Core::IndexHashMap<LiteralType>;
		friend class Core::Indexer<T, std::unordered_map<LiteralType, uint>>;
		friend class Core::Indexer<T, std::map<LiteralType, uint>>;

	protected:
		inline LiteralType Literal() const { return _id; }
		inline operator LiteralType() const { return _id; }

	protected:
		explicit UniqueID(LiteralType id) : _id(id) {}
		LiteralType _id;
	};
}