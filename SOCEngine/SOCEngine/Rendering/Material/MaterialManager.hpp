#pragma once

#include "PhysicallyBasedMaterial.h"
#include "SkyBoxMaterial.h"

#include "VectorIndexer.hpp"
#include <tuple>

namespace Rendering
{
	namespace Manager
	{
		class MaterialManager final
		{
		public:
			MaterialManager() = default;
			DISALLOW_ASSIGN_COPY(MaterialManager);

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
			using MaterialPool = Core::VectorHashMap<std::string, MaterialType>;

			std::tuple<	MaterialPool<Material::PhysicallyBasedMaterial>,
						MaterialPool<Material::SkyBoxMaterial> >	_materials;
		};
	}
}