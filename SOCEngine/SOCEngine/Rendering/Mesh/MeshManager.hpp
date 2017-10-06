#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectID.hpp"
#include <tuple>
#include <assert.h>
#include "VBSortedMeshPool.h"

namespace Rendering
{
	namespace Manager
	{
		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			template<class Pool> Geometry::Mesh& Acquire(Core::ObjectID objID, Pool& meshPool)
			{
				auto mesh = Geometry::Mesh(objID);
				return meshPool.Add(objID.Literal(), mesh);
			}
			template<class Pool> void Delete(Core::ObjectID objID, Pool& meshPool)
			{
				meshPool.Delete(objID.Literal());
			}
			template<class Pool> bool Has(Core::ObjectID objID, Pool& meshPool) const
			{
				return	meshPool.Has(objID.Literal());
			}
			template<class Pool> Geometry::Mesh* Find(Core::ObjectID id, Pool& meshPool)
			{
				return meshPool.Find(id.Literal());
			}

			Geometry::Mesh& Acquire(Core::ObjectID objID);
			void Delete(Core::ObjectID objID);
			bool Has(Core::ObjectID objID) const;
			Geometry::Mesh* Find(Core::ObjectID id);

			void CheckDirty(const Core::TransformPool& tfPool);
			void ComputeWorldSize(Math::Vector3& refWorldMin, Math::Vector3& refWorldMax, const Core::TransformPool& tfPool) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::TransformPool& tfPool);

			void ClearDirty() { _dirtyMeshes.clear(); }

			template <class FromPool, class ToPool>
			bool ChangeTrait(Core::ObjectID id, FromPool& fromPool, ToPool& toPool)
			{
				uint literlID = id.Literal();
				assert(fromPool.Has(literlID));
				assert(toPool.Has(literlID) == false);

				toPool.Add(literlID, *fromPool.Find(literlID));
				fromPool.Delete(literlID);
			}

			GET_ACCESSOR(TransparentMeshPool,	Geometry::TransparentMeshPool&,				_transparentMeshPool);
			GET_ACCESSOR(OpaqueMeshPool,		Geometry::OpaqueMeshPool&,					_opaqueMeshPool);
			GET_ACCESSOR(AlphaBlendMeshPool,	Geometry::OpaqueMeshPool&,					_alphaBlendMeshPool);

			GET_CONST_ACCESSOR(TransparentMeshPool,	const Geometry::TransparentMeshPool&,	_transparentMeshPool);
			GET_CONST_ACCESSOR(OpaqueMeshPool,		const Geometry::OpaqueMeshPool&,		_opaqueMeshPool);
			GET_CONST_ACCESSOR(AlphaBlendMeshPool,	const Geometry::OpaqueMeshPool&,		_alphaBlendMeshPool);

			GET_CONST_ACCESSOR(HasDirtyMeshes, bool, _dirtyMeshes.empty() == false);

		private:
			Geometry::TransparentMeshPool	_transparentMeshPool;
			Geometry::OpaqueMeshPool		_opaqueMeshPool;
			Geometry::OpaqueMeshPool		_alphaBlendMeshPool;

			std::vector<Geometry::Mesh*> _dirtyMeshes;
		};
	}
}