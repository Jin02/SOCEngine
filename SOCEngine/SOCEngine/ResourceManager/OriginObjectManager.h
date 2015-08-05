#pragma once

#include <map>
#include <string>

#include "Object.h"

namespace Resource
{
	class OriginObjectManager
	{
	private:
		std::map<std::string, const Core::Object*> _map;

	public:
		OriginObjectManager();
		~OriginObjectManager();

	public:
		void Add(const std::string& key, const Core::Object* object);
		const Core::Object* Find(const std::string& key);
		void Delete(const std::string& key);
		void DeleteAll();
	};
}