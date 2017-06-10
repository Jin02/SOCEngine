#pragma once

#include <bitset>
#include <vector>

namespace Core
{
	class UniqueIdManager 
	{
	public:
		constexpr static const uint BitSize = sizeof(uint);

		UniqueIdManager() = default;

	public:
		const uint Acquire()
		{
			uint bitFieldIdx = 0;
			uint allSet = -1;
			for (; bitFieldIdx < _bitFields.size(); ++bitFieldIdx)
			{
				auto& bitField = _bitFields[bitFieldIdx];
				if (bitField == allSet)
					continue;

				uint pos = 0;
				while (pos < BitSize && (bitField & (1 << pos++)));

				if (--pos != BitSize)
				{
					bitField |= (1 << pos);
					return pos + bitFieldIdx * BitSize;
				}
			}

			_bitFields.push_back(1);
			return bitFieldIdx * BitSize;
		}

		bool Has(uint id)
		{
			uint pos = id / BitSize;
			return (pos >= _bitFields.size()) ? false : (_bitFields[pos] & (1 << (id % BitSize))) != 0;
		}

		void Delete(uint id)
		{
			if ( Has(id) )
			{
				uint& bits = _bitFields[id / BitSize];
				bits &= ~(1 << (id % BitSize));
			}
		}

		void DeleteAll()
		{
			_bitFields.clear();
		}

		DISALLOW_ASSIGN(UniqueIdManager);
		DISALLOW_COPY_CONSTRUCTOR(UniqueIdManager);

	private:
		std::vector<uint> _bitFields;
	};
}