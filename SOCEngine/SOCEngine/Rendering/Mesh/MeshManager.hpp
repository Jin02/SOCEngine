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

		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			Geometry::Mesh& Acquire(Core::ObjectID objID);
			void Delete(Core::ObjectID objID);
			bool Has(Core::ObjectID objID) const;
			Geometry::Mesh* Find(Core::ObjectID id);

			void ComputeWorldSize(Math::Vector3& refWorldMin, Math::Vector3& refWorldMax, const Core::TransformPool& tfPool) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::TransformPool& tfPool);

			void CheckTraitWithDirty(const Core::TransformPool& tfPool);
			void ClearDirty();

			GET_CONST_ACCESSOR(HasDirtyMeshes, bool, _dirtyMeshes.empty() == false);

			template <typename Trait>
			void AddMeshIDToTraitArray(Geometry::Mesh& mesh)
			{
				auto& meshIdsByVBKey = GetSortedMeshIDsByVBKey<Trait>();

				uint vbKey = mesh.GetVertexBuffer().GetKey();
				using MeshIDs = std::vector<Core::ObjectID::LiteralType>;

				MeshIDs* objIDs = meshIdsByVBKey.Find(vbKey);
				if (objIDs)
				{
					uint meshLiteralID = mesh.GetObjectID().Literal();
					for (Core::ObjectID::LiteralType objID : (*objIDs))
					{
						if (objID == mesh.GetObjectID().Literal())
							return;
					}

					objIDs->push_back(meshLiteralID);
				}
				else
				{
					meshIdsByVBKey.Add(vbKey, std::vector<Core::ObjectID::LiteralType>{mesh.GetObjectID().Literal()});
				}
			}
			template <typename Trait>
			void DeleteMeshIDToTraitArray(Geometry::Mesh& mesh)
			{
				auto& meshIdsByVBKey = GetSortedMeshIDsByVBKey<Trait>();

				uint vbKey = mesh.GetVertexBuffer().GetKey();
				using MeshIDs = std::vector<Core::ObjectID::LiteralType>;

				MeshIDs* objIDs = meshIdsByVBKey.Find(vbKey);
				if (objIDs)
				{
					uint meshLiteralID = mesh.GetObjectID().Literal();

					uint size = objIDs->size();
					for (uint i=0; i<size; ++i)
					{
						if (objIDs->at(i) == mesh.GetObjectID().Literal())
						{
							objIDs->erase(objIDs->begin() + i);
							return;
						}
					}
				}
			}

			GET_ACCESSOR(Pool, auto&, _pool);
			GET_CONST_ACCESSOR(Pool, const auto&, _pool);

		private:
			using SortedMeshIDsByVBKey = Core::VectorHashMap<uint, std::vector<Core::ObjectID::LiteralType>>;

			template <typename Trait> auto& GetSortedMeshIDsByVBKey()
			{
				return std::get<SortedMeshIDsByVBKey<Trait>>(_tuple);
			}
			template <typename Trait> const auto& GetSortedMeshIDsByVBKey() const
			{
				return std::get<SortedMeshIDsByVBKey<Trait>>(_tuple);
			}

		private:
			MeshPool					_pool;
			SortedMeshIDsByVBKey		_opaqueMeshIDs;
			SortedMeshIDsByVBKey		_alphaBlendMeshIDs;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}