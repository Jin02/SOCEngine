#pragma once

#include "Common.h"

namespace Core
{
	class UniqueID
	{
	public:
		using LiteralType = uint;
		inline LiteralType Literal() const { return _id; }
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

	protected:
		explicit UniqueID(LiteralType id) : _id(id) {}
		LiteralType _id;
	};
}