#pragma once

#include "Sphere.h"
#include "Frustum.h"

#include "MeshManager.hpp"
#include "ObjectManager.h"

namespace Rendering
{
	namespace Geometry
	{
		class MeshUtility final
		{			
		public:
			template <class CullFunc>
			static void ClassifyTransparentMesh(
				std::vector<const Mesh*>& refMeshes, const Vector3& viewDir,
				const TransparentMeshPool& pool,
				const Core::ObjectManager& objMgr,
				const Core::TransformPool& transformPool,
				CullFunc cullFunc)
			{
				refMeshes.clear();

				const auto& meshes = pool.GetVector();
				for (const auto& mesh : meshes)
				{
					ObjectID id = mesh.GetObjectID();
					const Object* object = objMgr.Find(id); assert(object);
					const auto& transform = _FindTransform(mesh, transformPool);

					bool use =	object->GetUse() | cullFunc(mesh, transform);

					if (use)
						refMeshes.push_back(&mesh);
				}

				_SortTransparentMesh(refMeshes, viewDir, transformPool);
			}

		private:
			static const Core::Transform&
				_FindTransform(const Geometry::Mesh& mesh, 
							   const Core::TransformPool& transformPool);
			static void
				_SortTransparentMesh(std::vector<const Mesh*>& refMeshes,
									 const Vector3& viewDir,
									 const Core::TransformPool& transformPool);
		};
	}
}
