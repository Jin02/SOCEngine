#pragma once

#include "ObjectId.hpp"
#include <bitset>
#include <vector>

namespace Core
{
	class ObjectIdManager final
	{
	public:
		const ObjectId  Acquire();
		bool            Has(ObjectId id) noexcept;
		void            Delete(ObjectId id) noexcept;

		DISALLOW_ASSIGN(ObjectIdManager);
		DISALLOW_COPY_CONSTRUCTOR(ObjectIdManager);

	private:
		template<class BitSet>
		uint BinarySearchInLargeBitset(BitSet bitField, uint startIdx = 0)
		{
			if (bitField.size() == 2)
				return bitField[0] + startIdx;

			constexpr uint halfSize = bitField.size() / 2;

			uint val = (bitField >> halfSize).to_ulong();
			auto left = std::bitset<halfSize>(val);

			BitSet halfSet = BitSet().set() >> halfSize;
			val = (bitField & halfSet).to_ulong();
			auto right = std::bitset<halfSize>(val);

			uint result = 0;

			if (right != decltype(right)().set())
				result = BinarySearchInLargeBitset(right, startIdx);
			else if (left != decltype(left)().set())
				result = BinarySearchInLargeBitset(left, startIdx + halfSize);

			return result;
		}

	private:
		static const uint _bitSize = 4;
		std::vector<std::bitset<_bitSize>> _bitFields;
	};
}