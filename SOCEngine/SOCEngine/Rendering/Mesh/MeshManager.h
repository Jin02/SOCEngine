#pragma once

#include "VectorIndexer.hpp"
#include "Mesh.h"
#include "ObjectID.hpp"
#include <tuple>
#include <assert.h>
#include "VBSortedMeshpool.h"
#include "OpaqueMeshRenderQueue.h"

namespace Core
{
	class Object;
}

namespace Rendering
{
	namespace Geometry
	{
		using TransparentMeshPool	= Core::VectorHashMap<Core::ObjectID::LiteralType, Mesh>;
		using OpaqueMeshPool		= VBSortedMeshPool;
		using AlphaTestMeshPool		= OpaqueMeshPool;

		struct MeshPoolRefs
		{
			AlphaTestMeshPool&		alphaTestMeshes;
			OpaqueMeshPool&			opaqueMeshes;
			TransparentMeshPool&	transparentMeshes;

			explicit MeshPoolRefs(AlphaTestMeshPool& _alphaTestMeshes, OpaqueMeshPool& _opaqueMeshes, TransparentMeshPool& _transparentMeshes)
					: alphaTestMeshes(_alphaTestMeshes), opaqueMeshes(_opaqueMeshes), transparentMeshes(_transparentMeshes) { }
		};
	};
		
	namespace Manager
	{
		class CameraManager;

		class MeshManager final
		{
		public:
			MeshManager() = default;
			DISALLOW_ASSIGN_COPY(MeshManager);

		public:
			Geometry::Mesh& Add(Geometry::Mesh& mesh, Geometry::TransparentMeshPool& meshPool)
			{
				assert(mesh.GetVBKey() != 0); //Error, mesh does not init yet.
				return meshPool.Add(mesh.GetObjectID().Literal(), mesh);
			}

			Geometry::Mesh& Add(Geometry::Mesh& mesh, Geometry::OpaqueMeshPool& meshPool) // or AlphaTestMeshPool
			{
				assert(mesh.GetVBKey() != 0); //Error, mesh does not init yet.				
				return meshPool.Add(mesh.GetObjectID(), mesh.GetVBKey(), mesh);
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
			template<class Pool> const Geometry::Mesh* Find(Core::ObjectID id, Pool& meshPool) const
			{
				return meshPool.Find(id.Literal());
			}			

			Geometry::Mesh& Add(Geometry::Mesh& mesh)
			{
				return Add(mesh, GetOpaqueMeshPool());
			}

			Geometry::Mesh& Acquire(Core::ObjectID id)
			{
				Geometry::Mesh mesh(id);
				return Add(mesh, GetOpaqueMeshPool());
			}

			void Delete(Core::ObjectID objID);
			bool Has(Core::ObjectID objID) const;
			Geometry::Mesh* Find(Core::ObjectID id);

			void CheckDirty(const Core::TransformPool& tfPool, const Core::ObjectManager& objMgr, const Manager::CameraManager& camMgr);
			void ComputeWorldSize(Math::Vector3& refWorldMin, Math::Vector3& refWorldMax, const Core::TransformPool& tfPool) const;
			void UpdateTransformCB(Device::DirectX& dx, const Core::TransformPool& tfPool);

			void ClearDirty() { _dirtyMeshes.clear(); _mustUpdateCBMeshes.clear(); }

			bool ChangeTrait(Core::ObjectID id,
							 Geometry::OpaqueMeshPool& fromPool, Geometry::TransparentMeshPool& toPool)
			{
				uint literlID = id.Literal();
				assert(fromPool.Has(literlID));
				assert(toPool.Has(literlID) == false);

				toPool.Add(literlID, *fromPool.Find(literlID));
				fromPool.Delete(literlID);
			}
			bool ChangeTrait(Core::ObjectID id,
							 Geometry::TransparentMeshPool& fromPool, Geometry::OpaqueMeshPool& toPool) // or AlphaTest
			{
				uint literlID = id.Literal();
				assert(fromPool.Has(literlID));
				assert(toPool.Has(literlID) == false);

				Geometry::Mesh* mesh = fromPool.Find(literlID);
				toPool.Add(id, mesh->GetVBKey(), *mesh);
				fromPool.Delete(literlID);
			}

			GET_ALL_ACCESSOR(TransparentMeshPool,	Geometry::TransparentMeshPool&,	_transparentMeshPool);
			GET_ALL_ACCESSOR(OpaqueMeshPool,		Geometry::OpaqueMeshPool&,		_opaqueMeshPool);
			GET_ALL_ACCESSOR(AlphaTestMeshPool,		Geometry::AlphaTestMeshPool&,	_alphaTestMeshPool);
			GET_ACCESSOR(AllMeshPoolRefs,			auto,							Geometry::MeshPoolRefs(_alphaTestMeshPool, _opaqueMeshPool, _transparentMeshPool));

			GET_CONST_ACCESSOR(HasDirtyMeshes,		bool,							_dirtyMeshes.empty() == false);

		private:
			Geometry::TransparentMeshPool				_transparentMeshPool;
			Geometry::OpaqueMeshPool					_opaqueMeshPool;
			Geometry::AlphaTestMeshPool					_alphaTestMeshPool;

			std::vector<Geometry::Mesh*>				_mustUpdateCBMeshes;
			std::vector<const Geometry::Mesh*>			_dirtyMeshes;
		};
	}
}