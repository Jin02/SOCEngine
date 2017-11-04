#pragma once

#include "Sphere.h"
#include "Frustum.h"

#include "MeshManager.hpp"
#include "ObjectManager.h"

#include "MeshRenderQueue.h"

namespace Rendering
{
	namespace Geometry
	{
		class MeshUtility final
		{			
		public:
			template <class CullFunc>					// CullFunc Form = "[](const Mesh&, const Transform&) -> bool { ... }"
			static void ClassifyTransparentMesh(
				RenderQueue::TransparentMeshRenderQueue& renderQueue,
				const Math::Vector3& viewDir,
				const TransparentMeshPool& pool,
				const Core::ObjectManager& objMgr,
				const Core::TransformPool& transformPool,
				CullFunc cullFunc)
			{
				renderQueue.clear();

				const auto& meshes = pool.GetVector();
				for (const auto& mesh : meshes)
				{
					ObjectID id							= mesh.GetObjectID();
					const Object* object				= objMgr.Find(id); assert(object);
					const Core::Transform& transform	= _FindTransform(mesh.GetObjectID(), transformPool);

					bool use =	object->GetUse() & cullFunc(mesh, transform);

					if (use)
						renderQueue.emplace_back(const_cast<Mesh*>(&mesh));
				}

				_SortTransparentMesh(renderQueue, viewDir, transformPool);
			}

			template <class CullFunc>					// CullFunc Form = "[](const Mesh&, const Transform&) -> bool { ... }"
			static void ClassifyOpaqueMesh(
				RenderQueue::OpaqueMeshRenderQueue& renderQueue,
				const OpaqueMeshPool& pool,
				const Core::ObjectManager& objMgr,
				const Core::TransformPool& transformPool,
				CullFunc cullFunc)
			{
				renderQueue.DeleteAllContent();
				
				pool.Iterate(
						[&objMgr, &transformPool,
						 &renderQueue, &cullFunc](const Mesh& mesh)
						{
							Core::ObjectID id					= mesh.GetObjectID();
							const Core::Object* object			= objMgr.Find(id); assert(object);
							const Core::Transform& transform	= _FindTransform(id, transformPool);
							
							bool use = object->GetUse() & cullFunc(mesh, transform);
							if (use)
								renderQueue.Add(const_cast<Mesh&>(mesh));
						}
					);				
			}
			
			
		private:
			static const Core::Transform&
				_FindTransform(Core::ObjectID id, const Core::TransformPool& transformPool);
			static void
				_SortTransparentMesh(RenderQueue::TransparentMeshRenderQueue& renderQueue,
									 const Math::Vector3& viewDir, const Core::TransformPool& transformPool);
		};
	}
}
