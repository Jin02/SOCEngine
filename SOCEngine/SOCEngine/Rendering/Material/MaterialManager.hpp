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

		public:
			void UpdateConstBuffer(Device::DirectX& dx);

		public:
			template <typename MaterialType> MaterialType& Add(const std::string& key, MaterialType& material)
			{
				return GetPool<MaterialType>().Add(key, material);
			}
			template <typename MaterialType> void Delete(const std::string& key)
			{
				GetPool<MaterialType>().Delete(key);
			}
			template <typename MaterialType> auto Find(const std::string& key)
			{
				return GetPool<MaterialType>().Find(key);
			}
			template <typename MaterialType> bool Has(const std::string& key) const
			{
				return GetPool<MaterialType>().GetIndexer().Has(key);
			}

		public:
			template <typename MaterialType> auto& GetPool()
			{
				return GetMaterialDatas<MaterialType>().pool;
			}
			template <typename MaterialType> const auto& GetPool() const
			{
				return GetMaterialDatas<MaterialType>().pool;
			}

		private:
			template <typename MaterialType> auto& GetDirty()
			{
				return GetMaterialDatas<MaterialType>().dirty;
			}
			template <typename MaterialType> const auto& GetDirty() const
			{
				return GetMaterialDatas<MaterialType>().dirty;
			}

			template <typename MaterialType> auto& GetMaterialDatas()
			{
				return std::get<MaterialDatas<MaterialType>>(_materials);
			}
			template <typename MaterialType> const auto& GetMaterialDatas() const
			{
				return std::get<MaterialDatas<MaterialType>>(_materials);
			}

		private:
			template <class MaterialType>
			using MaterialPool = Core::VectorHashMap<std::string, MaterialType>;

			template <class MaterialType>
			struct MaterialDatas
			{
				MaterialPool<MaterialType> pool;
				std::vector<MaterialType*> dirty;
			};

			std::tuple<	MaterialDatas<Material::PhysicallyBasedMaterial>,
						MaterialDatas<Material::SkyBoxMaterial> >	_materials;
		};
	}
}