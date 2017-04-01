#pragma once

#include "PhysicallyBasedMaterial.h"
#include "SkyBoxMaterial.h"

#include "VectorIndexer.hpp"
#include <tuple>

namespace Rendering
{
	namespace Manager
	{
		template <class... Materials>
		class MaterialSystem final
		{
		public:
			MaterialSystem() = default;
			DISALLOW_ASSIGN_COPY(MaterialSystem);

			template <typename MaterialType>
			void Add(const std::string& key, MaterialType& material)
			{
				std::get<MaterialPool<MaterialType>>(_materials).Add(key, material);
			}
			template <typename MaterialType>
			void Delete(const std::string& key)
			{
				std::get<MaterialPool<MaterialType>>(_materials).Delete(key);
			}
			template <typename MaterialType>
			auto Find(const std::string& key)
			{
				return std::get<MaterialPool<MaterialType>>(_materials).Find(key);
			}

			template <typename MaterialType>
			const std::vector<MaterialType>& GetMaterials() const
			{
				return std::get<MaterialPool<MaterialType>>(_materials).GetVector();
			}

		private:
			template <class MaterialType>
			class MaterialPool : public Core::VectorHashMap<std::string, MaterialType> {};

			std::tuple<MaterialPool<Materials>...>	_materials;
		};

		class MaterialManager final
		{
		public:
			MaterialManager() = default;
			DISALLOW_ASSIGN_COPY(MaterialManager);

			auto& Get()
			{
				return _materialSystem;
			}

		private:
			MaterialSystem<PhysicallyBasedMaterial, SkyBoxMaterial> _materialSystem;
		};
	}
}