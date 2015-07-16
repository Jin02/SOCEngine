#pragma once

#include "Material.h"
#include "Structure.h"
#include <hash_map>

namespace Rendering
{
	namespace Manager
	{
		class MaterialManager
		{
		private:
			std::hash_map<std::string, Material*>				_hash;

		public:
			MaterialManager();
			~MaterialManager();

		public:
			void Add(const std::string& file, const std::string& name, Material* material);
			Material* Find(const std::string& file, const std::string& name);
			void Delete(const std::string& file, const std::string& name);
			void DeleteAll();

		public:
			void Add(const std::string& key, Material* material);
			Material* Find(const std::string& key);
			void Delete(const std::string& key);
		};
	}
}