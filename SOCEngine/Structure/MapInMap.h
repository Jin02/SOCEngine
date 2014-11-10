#pragma once

#include <map>
#include "Map.h"

namespace Structure
{
	template <class Object>
	class MapInMap
	{
	private:
		Map<Map<Object>>	_mim;

	public:
		MapInMap(void){}
		virtual ~MapInMap(void)
		{
			DeleteAll(true);
		}

	public:
		virtual Object* Add(const std::string& key1, const std::string& key2, Object* object, bool copy = false)
		{
			Map<Object>* inMap = _mim.Find(key1);

			if(inMap == nullptr)
				inMap = _mim.Add(key1, new Map<Object>, false);

			return inMap->Add(key2, object, copy);
		}

		virtual Object* Find(const std::string& key1, const std::string& key2)
		{
			Object* object = nullptr;
			Map<Object>* inMap = _mim.Find(key1);

			if( inMap )
				object = inMap->Find(key2);

			return object;
		}

		virtual void Delete(const std::string& key1, const std::string& key2, bool contentRemove = false)
		{
			Map<Object>* inMap = _mim.Find(key1);

			if(inMap)
				_mim.Delete(key2, contentRemove);
		}

		virtual void Delete(const std::string& key1, bool contentRemove = false)
		{
			_mim.Delete(key1, contentRemove);
		}

		virtual void DeleteAll(bool contentRemove = false)
		{
			_mim.DeleteAll(contentRemove);
		}

		inline const Map<Map<Object>>& GetMapInMap() const { return _mim; }
		GET_ACCESSOR(Size, unsigned int, _mim.GetSize());
	};
}