#pragma once

#include <utility>
#include <string>
#include "Common.h"

namespace Structure
{
	template <class Object>
	class BaseStructure
	{
	public:
		typedef std::pair<bool, Object*>		Data;

	public:
		BaseStructure(void){}
		virtual ~BaseStructure(void){}

	public:
		virtual Object* Add(const std::string& key, Object* object, bool copy = false) = 0;
		virtual Object* Find(const std::string& key) = 0;
		virtual void Delete(const std::string& key, bool contentRemove = false) = 0;
		virtual void DeleteAll(bool contentRemove = false) = 0;

	public:
#define GET_CONTENT_FROM_ITERATOR(iter)	iter->second.second
#define GET_IS_COPY_FROM_ITERATOR(iter)	iter->second.first
#define GET_KEY_FROM_ITERATOR(iter)		iter->first
	};

}