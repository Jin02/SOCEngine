#include "ObjectIdManager.h"

using namespace Core;

const ObjectId ObjectIdManager::Acquire()
{
	uint bitFieldIdx = 0;
	auto allSet = std::bitset<_bitSize>().set();
	for (; bitFieldIdx < _bitFields.size(); ++bitFieldIdx)
	{
		auto& bitField = _bitFields[bitFieldIdx];
		if (bitField == allSet)
			continue;

		uint pos = 0;
		while (pos < _bitSize && bitField[pos++]);

		if (--pos != _bitSize)
		{
			bitField[pos] = true;
			return ObjectId(pos + bitFieldIdx * _bitSize);
		}
	}

	_bitFields.push_back(1);
	return ObjectId(bitFieldIdx * _bitSize);
}

bool ObjectIdManager::Has(ObjectId id) noexcept
{
	uint pos = id / _bitSize;
	return (pos >= _bitFields.size()) ? false : _bitFields[pos][id % _bitSize];
}

void ObjectIdManager::Delete(ObjectId id) noexcept
{
	if (Has(id))
		_bitFields[id / _bitSize][id % _bitSize] = false;
}

void Core::ObjectIdManager::DeleteAll()
{
	_bitFields.clear();
}
