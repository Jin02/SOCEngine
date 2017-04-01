#pragma once

#include "Transform.h"
#include "ArrayVector.hpp"
#include "VectorIndexer.hpp"

namespace Core
{
#define ToUint(State) static_cast<uint>(State)
	enum class ComponentState : uint
	{
		Active,
		UnUsed,
		Delete,
		MAX
	};

	template <class ObjectType, typename StateType>
	class ComponentPool final
	{
	public:
		using ObjectPtr = std::shared_ptr<ObjectType>;

	public:
		ObjectPtr Acquire(const ObjectId id)
		{
			auto& obj = _pool[ToUint(StateType::Active)].Add(id, ObjectType(id));
			return ObjectPtr(obj, SharedObjDelete);
		}

		void Delete(const ObjectId id)
		{
			for (uint i = 0; i < ToUint(StateType::Max); ++i)
				_pool[i].Delete(id);
		}

		template <typename State>
		bool Delete(const ObjectId id)
		{
			_pool[ToUint(State)].Delete(id);
		}

		template <typename State>
		bool Has(const ObjectId id)
		{
			return _pool[ToUint(State)].GetIndexer().Has(id);
		}

		template <typename State>
		ObjectPtr Find(const ObjectId id)
		{
			auto& indexer = _pool[ToUint(State)].GetIndexer();
			uint findIdx = indexer.Find(id);
			return (findIdx != indexer.FailIndex()) ? ObjectPtr(_pool[ToUint(State)], SharedObjDelete) : nullptr;
		}

		template<StateType FromState, StateType ToState>
		void Copy(const ObjectId id)
		{
			auto& from		= _pool[ToUint(FromState)];
			uint findIndex	= from.GetIndexer().Find(id);
			if (findIndex != decltype(from)::IndexerType::FailIndex())
			{
				auto& object = from.Get(findIndex);
				_pool[ToUint(ToState)].Add(object.GetObjectId(), object);
			}
		}

		template<StateType FromState, StateType ToState>
		void Move(const ObjectId id)
		{
			Copy<FromState, ToState>(id);
			Delete<FromState>(fromIndex);
		}

	private:
		void SharedObjDelete(ObjectType* obj)
		{
			Delete(tf->GetObjectId());
		}

	private:		
		using Pool = std::array<VectorHashMap<ObjectId::LiteralType, ObjectType>, ToUint(State::Max)>;
		Pool _pool;
	};

#undef ToUint
}