#pragma once

#include <map>
#include "Map.h"

namespace Structure
{
	template <typename Key, class Object>
	class MapInMap 
	{
	private:
		Map<Key, Map<Key, Object>>	_mim;

	public:
		MapInMap(void){}
		virtual ~MapInMap(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const Key& key1, const Key& key2, Object* object, bool copy = false)
		{
			Map<Key, Object>* inMap = _mim.Find(key1);

			if(inMap == nullptr)
				inMap = _mim.Add(key1, new Map<Key, Object>, false);

			return inMap->Add(key2, object, copy);
		}

		virtual Object* Find(const Key& key1, const Key& key2)
		{
			Object* object = nullptr;
			Map<Key, Object>* inMap = _mim.Find(key1);

			if( inMap )
				object = inMap->Find(key2);

			return object;
		}

		virtual void Delete(const Key& key1, const Key& key2, bool contentRemove = false)
		{
			Map<Key, Object>* inMap = _mim.Find(key1);

			if(inMap)
				_mim.Delete(key2, contentRemove);
		}

		virtual void Delete(const Key& key1, bool contentRemove = false)
		{
			_mim.Delete(key1, contentRemove);
		}

		virtual void DeleteAll(bool contentRemove = false)
		{
			_mim.DeleteAll(contentRemove);
		}

		inline const Map<Key, Map<Key, Object>>& GetMapInMap() const { return _mim; }
		GET_ACCESSOR(Size, unsigned int, _mim.GetSize());
	};
}