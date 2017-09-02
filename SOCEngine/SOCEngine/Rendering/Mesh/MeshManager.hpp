#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectID.hpp"
#include <tuple>
#include <assert.h>

namespace Rendering
{
	namespace Manager
	{
		namespace RenderMethod
		{
			enum class Method
			{
				Opaque,
				AlphaBlend,
				Transparency
			};
			struct Opaque		{	constexpr static Method method = Method::Opaque;		};
			struct AlphaBlend	{	constexpr static Method method = Method::AlphaBlend;	};
			struct Transparency	{	constexpr static Method method = Method::Transparency;	};
		}

		using MeshPool = Core::VectorHashMap<Core::ObjectID::LiteralType, Geometry::Mesh>;

		template <typename Trait>
		class MeshPoolTrait : public MeshPool
		{
		public:
			MeshPoolTrait() = default;
			DISALLOW_ASSIGN(MeshPoolTrait);
		};

		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			template<typename MeshTraits> Geometry::Mesh& Acquire(Core::ObjectID objID)
			{
				auto mesh = Geometry::Mesh(objID);
				return GetPool<MeshTraits>().Add(objID.Literal(), mesh);
			}
			template<typename MeshTraits> void Delete(Core::ObjectID objID)
			{
				GetPool<MeshTraits>().Delete(objID.Literal());
			}
			template<typename MeshTraits> bool Has(Core::ObjectID objID) const
			{
				return	GetPool<MeshTraits>().Has(objID.Literal());
			}
			template<typename MeshTraits> Geometry::Mesh* Find(Core::ObjectID id)
			{
				return GetPool<MeshTraits>().Find(id.Literal());
			}

			Geometry::Mesh& Acquire(Core::ObjectID objID);
			void Delete(Core::ObjectID objID);
			bool Has(Core::ObjectID objID) const;
			Geometry::Mesh* Find(Core::ObjectID id);

			void CheckDirty(const Core::TransformPool& tfPool);
			void ComputeWorldSize(Math::Vector3& refWorldMin, Math::Vector3& refWorldMax, const Core::TransformPool& tfPool) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::TransformPool& tfPool);

			void ClearDirty() { _dirtyMeshes.clear(); }

			template <typename FromTrait, typename ToTrait>
			bool ChangeTrait(Core::ObjectID id)
			{
				auto& fromIndexer = GetPool<FromTrait>().GetIndexer();
				auto& toIndexer = GetPool<ToTrait>().GetIndexer();

				assert(fromIndexer.Has(id) == false);
				assert(toIndexer.Has(id) == false);

				uint idx = fromIndexer.Find(id);
				//GetPool<ToTrait>().Add(id, GetPool<FromTrait>().Get(idx));
				//GetPool<FromTrait>().Delete(id);
			}

			template <typename Trait> MeshPool& GetPool()
			{
				return std::get<MeshPoolTrait<Trait>>(_tuple);
			}
			template <typename Trait> const MeshPool& GetPool() const
			{
				return std::get<MeshPoolTrait<Trait>>(_tuple);
			}

			GET_CONST_ACCESSOR(HasDirtyMeshes, bool, _dirtyMeshes.empty() == false);

		private:
			std::tuple<	MeshPoolTrait<RenderMethod::Opaque>,
						MeshPoolTrait<RenderMethod::AlphaBlend>,
						MeshPoolTrait<RenderMethod::Transparency>> _tuple;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}