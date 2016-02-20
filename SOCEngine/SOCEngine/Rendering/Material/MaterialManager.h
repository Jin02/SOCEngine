#pragma once

#include "Material.h"
#include "Structure.h"
#include <hash_map>
#include "VectorHashMap.h"

namespace Rendering
{
	namespace Manager
	{
		class MaterialManager
		{
		private:
			Structure::VectorHashMap<std::string, Material*>	_materials;

		public:
			MaterialManager();
			~MaterialManager();

		public:
			void Initialize();
			void Add(const std::string& file, const std::string& name, Material* material);
			Material* Find(const std::string& file, const std::string& name);
			void Delete(const std::string& file, const std::string& name);
			void DeleteAll();

		public:
			void Add(const std::string& key, Material* material);
			Material* Find(const std::string& key);
			void Delete(const std::string& key);

		public:
			inline const Structure::VectorHashMap<std::string, Material*>& GetMaterials() const { return _materials; }
		};
	}
}