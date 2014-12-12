#pragma once

#include "Material.h"
#include "Structure.h"

namespace Rendering
{
	namespace Manager
	{
		class MaterialManager
		{
		private:
			Structure::Map< Structure::Map<Material> >	_map;
			Structure::HashMap<Material>				_hash;

		public:
			MaterialManager();
			~MaterialManager();

		public:
			Material* Add(const std::string& file, const std::string& name, Material* material, bool copy = false);
			Material* Find(const std::string& file, const std::string& name);
			void Delete(const std::string& file, const std::string& name, bool contentRemove = false);
			void DeleteFile(const std::string& file, bool remove = false);
			void DeleteAll(bool remove = false);

		public:
			Material* Add(const std::string& key, Material* material, bool copy = false);
			Material* Find(const std::string& key);
			void Delete(const std::string& key, bool contentRemove = false);
		};
	}
}