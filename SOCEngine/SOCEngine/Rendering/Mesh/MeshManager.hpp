#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectId.hpp"
#include <tuple>
#include <assert.h>

namespace Rendering
{
	namespace Manager
	{
		template <typename Trait>
		class MeshPool : public Core::VectorHashMap<Core::ObjectId::LiteralType, Geometry::Mesh>
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
			template<typename MeshTraits> Geometry::Mesh& Acquire(Core::ObjectId objId)
			{
				auto mesh = Geometry::Mesh(objId);
				return GetPool<MeshTraits>().Add(objId.Literal(), mesh);
			}
			template<typename MeshTraits> void Delete(Core::ObjectId objId)
			{
				GetPool<MeshTraits>().Delete(objId.Literal());
			}
			template<typename MeshTraits> bool Has(Core::ObjectId objId) const
			{
				return	GetPool<MeshTraits>().GetIndexer().Has(objId.Literal());
			}
			template<typename MeshTraits> Geometry::Mesh* Find(Core::ObjectId id)
			{
				return GetPool<MeshTraits>().Find(id.Literal());
			}

			Geometry::Mesh& Acquire(Core::ObjectId objId);
			void Delete(Core::ObjectId objId);
			bool Has(Core::ObjectId objId) const;
			Geometry::Mesh* Find(Core::ObjectId id);

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