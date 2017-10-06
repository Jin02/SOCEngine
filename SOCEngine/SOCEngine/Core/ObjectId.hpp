#pragma once

#include "UniqueIDManager.hpp"
#include "UniqueID.hpp"
#include "VectorIndexer.hpp"

namespace Core
{
	class ObjectID : public Core::UniqueID<ObjectID>
	{
	public:
		using Parent = Core::UniqueID<ObjectID>;
		using Parent::UniqueID;
		using Parent::Literal;

		friend class IDManagerForm<ObjectID>;
		friend class ObjectManager;
		friend class RootObjectIDs;
	};

	using ObjectIDManager = IDManagerForm<ObjectID>;
}