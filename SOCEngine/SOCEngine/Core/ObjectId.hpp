#pragma once

#include "Common.h"

namespace Core
{
	class ObjectId final
	{
	public:
		uint operator()() { return _id; }
		static constexpr uint Undefined() { return -1; }

	private:
		friend class ObjectIdManager;
		explicit ObjectId(uint id = -1) : _id(id) {}

		uint _id;
	};
}