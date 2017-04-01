#pragma once

#include "Common.h"

namespace Core
{
	class ObjectId final
	{
	public:
		using LiteralType = uint;
//		LiteralType operator()() { return _id; }
		operator LiteralType() { return _id; }
		bool operator==(const ObjectId& rhs) const
		{
			return _id == rhs._id;
		}
		bool operator!=(const ObjectId& rhs) const
		{
			return !operator==(rhs);
		}

		static constexpr LiteralType Undefined() { return -1; }

	private:
		friend class ObjectIdManager;
		explicit ObjectId(LiteralType id = -1) : _id(id) {}

		LiteralType _id;
	};
}