#pragma once

#include "BasicMaterial.h"
#include "Structure.h"

namespace Rendering
{
	namespace Manager
	{
		class MaterialManager
		{
		private:
			Structure::Map<std::string, Structure::Map<std::string, BasicMaterial> >	_map;
			Structure::HashMap<std::string, BasicMaterial>							_hash;

		public:
			MaterialManager();
			~MaterialManager();

		public:
			BasicMaterial* Add(const std::string& file, const std::string& name, BasicMaterial* material, bool copy = false);
			BasicMaterial* Find(const std::string& file, const std::string& name);
			void Delete(const std::string& file, const std::string& name, bool contentRemove = false);
			void DeleteFile(const std::string& file, bool remove = false);
			void DeleteAll(bool remove = false);

		public:
			BasicMaterial* Add(const std::string& key, BasicMaterial* material, bool copy = false);
			BasicMaterial* Find(const std::string& key);
			void Delete(const std::string& key, bool contentRemove = false);
		};
	}
}