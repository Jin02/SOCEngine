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

			void UpdateTraits();
			void ClearDirty() { _dirtyMeshes.clear(); }

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
			std::tuple<	MeshPoolTrait<Geometry::OpaqueTrait>,
						MeshPoolTrait<Geometry::AlphaBlendTrait>,
						MeshPoolTrait<Geometry::TransparencyTrait>> _tuple;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}