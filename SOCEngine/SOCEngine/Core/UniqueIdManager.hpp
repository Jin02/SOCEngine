#pragma once

#include <bitset>
#include <vector>

namespace Core
{
	template<class IdType>
	class UniqueIdManager 
	{
	public:
		constexpr static const uint BitSize = sizeof(uint);

		UniqueIdManager() = default;

	protected:
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

	public:
		bool Has(IdType id)
		{
			uint pos = id.Literal() / BitSize;
			return (pos >= _bitFields.size()) ? false : (_bitFields[pos] & (1 << (id.Literal() % BitSize))) != 0;
		}

		void Delete(IdType id)
		{
			if ( Has(id) )
			{
				uint& bits = _bitFields[id.Literal() / BitSize];
				bits &= ~(1 << (id.Literal() % BitSize));
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