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
		template <typename Trait>
		class MeshPool : public Core::VectorHashMap<Core::ObjectID::LiteralType, Geometry::Mesh>
		{
		public:
			MeshPool() = default;
			DISALLOW_ASSIGN(MeshPool);
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

			template <typename Trait> MeshPool<Trait>& GetPool()
			{
				return std::get<MeshPool<Trait>>(_tuple);
			}
			template <typename Trait> const MeshPool<Trait>& GetPool() const
			{
				return std::get<MeshPool<Trait>>(_tuple);
			}

			GET_CONST_ACCESSOR(HasDirtyMeshes, bool, _dirtyMeshes.empty() == false);

		private:
			std::tuple<	MeshPool<Geometry::OpaqueTrait>,
						MeshPool<Geometry::AlphaBlendTrait>,
						MeshPool<Geometry::TransparencyTrait>> _tuple;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}