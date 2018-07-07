#pragma once

#include "PhysicallyBasedMaterial.h"
#include "SkyBoxMaterial.h"

#include "VectorIndexer.hpp"
#include <tuple>

#include "MaterialID.hpp"
#include "Bookmark.hpp"

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
			void Initialize(Device::DirectX& dx);
			void Destroy();
			void UpdateConstBuffer(Device::DirectX& dx);

		public:
			template <typename MaterialType> 
			std::pair<MaterialID, MaterialType&> Add(MaterialType& material)
			{
				MaterialID key = GetIDManager<MaterialType>().Acquire();
				GetIDBookmark<MaterialType>().Add(material.GetName(), key.Literal());

				return std::pair<MaterialID, MaterialType&>(
					key,
					GetPool<MaterialType>().Add(key.Literal(), material));
			}

			template <typename MaterialType> void Delete(const std::string& key)
			{
				uint literalID = GetIDBookmark<MaterialType>().Find(key);

				GetPool<MaterialType>().Delete(MaterialID(literalID));
				GetIDBookmark<MaterialType>().Delete(key);
				GetIDManager<MaterialType>().Delete(MaterialID(literalID));
			}
			template <typename MaterialType> void Delete(MaterialID id)
			{
				auto findMaterial = GetPool<MaterialType>().Find(id);
				if(findMaterial == nullptr) return;

				GetIDBookmark<MaterialType>().Delete(findMaterial->GetName());
				GetPool<MaterialType>().Delete(literalID);
				GetIDManager<MaterialType>().Delete(id);
			}

			template <typename MaterialType> auto Find(const std::string& key)
			{
				return GetPool<MaterialType>().Find(GetIDBookmark<MaterialType>().Find(key));
			}
			template <typename MaterialType> auto Find(MaterialID id)
			{
				return GetPool<MaterialType>().Find(id);
			}

			template <typename MaterialType> const auto Find(const std::string& key) const
			{
				return GetPool<MaterialType>().Find(GetIDBookmark<MaterialType>().Find(key));
			}
			template <typename MaterialType> const auto Find(MaterialID id) const
			{
				return GetPool<MaterialType>().Find(id);
			}

			template <typename MaterialType> bool Has(const std::string& key) const
			{
				return GetIDBookmark<MaterialType>().Has(key);
			}
			template <typename MaterialType> bool Has(MaterialID id) const
			{
				return GetIDBookmark<MaterialType>().Has(id);
			}

			template <typename MaterialType> MaterialID FindID(const std::string& key)
			{
				return MaterialID(GetIDBookmark<MaterialType>().Find(key));
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
			template <typename MaterialType> auto& GetIDBookmark()
			{
				return GetMaterialDatas<MaterialType>().idBookmark;
			}
			template <typename MaterialType> const auto& GetIDBookmark() const
			{
				return GetMaterialDatas<MaterialType>().idBookmark;
			}
			template <typename MaterialType> auto& GetIDManager()
			{
				return GetMaterialDatas<MaterialType>().idMgr;
			}
			template <typename MaterialType> const auto& GetIDManager() const
			{
				return GetMaterialDatas<MaterialType>().idMgr;
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
			using MaterialPool = Core::VectorHashMap<MaterialID::LiteralType, MaterialType>;

			template <class MaterialType>
			struct MaterialDatas
			{
				MaterialPool<MaterialType>			pool;
				std::vector<MaterialType*>			dirty;

				Core::BookHashMapmark<std::string>	idBookmark;
				MaterialIDManager					idMgr;
			};

			std::tuple<	MaterialDatas<Material::PhysicallyBasedMaterial>,
						MaterialDatas<Material::SkyBoxMaterial> >	_materials;

			MaterialID _pbmDefaultKey;
		};
	}
}