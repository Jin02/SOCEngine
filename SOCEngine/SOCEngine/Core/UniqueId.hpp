#pragma once

#include "Common.h"

namespace Core
{
	class UniqueId
	{
	public:
		using LiteralType = uint;
		inline LiteralType Literal() const { return _id; }
		bool operator==(const UniqueId& rhs) const
		{
			return _id == rhs._id;
		}
		bool operator!=(const UniqueId& rhs) const
		{
			return !operator==(rhs);
		}

		static constexpr LiteralType Undefined() { return -1; }

		UniqueId() : _id(Undefined()) {}

	protected:
		explicit UniqueId(LiteralType id) : _id(id) {}
		LiteralType _id;
	};
}