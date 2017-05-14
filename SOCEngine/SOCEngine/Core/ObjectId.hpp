#pragma once

#include "Common.h"

namespace Core
{
	class ObjectId final
	{
	public:
		using LiteralType = uint;
//		LiteralType operator()() { return _id; }
		operator LiteralType() const { return _id; }
		bool operator==(const ObjectId& rhs) const
		{
			return _id == rhs._id;
		}
		bool operator!=(const ObjectId& rhs) const
		{
			return !operator==(rhs);
		}

		static constexpr LiteralType Undefined() { return -1; }

		ObjectId() : _id(Undefined()) {}

	private:
		friend class ObjectIdManager;
		explicit ObjectId(LiteralType id) : _id(id) {}

		LiteralType _id;
	};
}