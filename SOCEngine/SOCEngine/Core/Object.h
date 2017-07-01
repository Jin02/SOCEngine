#pragma once

#include "Common.h"
#include <string>

#include "ObjectId.hpp"

namespace Core
{
	class TransformPool;
	class Object final
	{
	public:
		Object(ObjectId id, TransformPool* tfPool);
		~Object();

		void AddChild(Object & child);
		bool HasChild(const Object & child) const;
		void DeleteChild(Object & child);
		auto FindChildUsingIndex(uint index);

		uint GetChildCount() const;



	public:
		GET_CONST_ACCESSOR(Name, auto&, _name);
		GET_CONST_ACCESSOR(ObjectId, auto, _id);

	private:
		std::string			_name;
		ObjectId			_id;
		TransformPool*		_tfPool;
	};
}
