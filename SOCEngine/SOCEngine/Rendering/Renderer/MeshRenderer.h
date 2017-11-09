#pragma once

#include "DefaultShaders.h"
#include "MeshManager.h"
#include "MaterialManager.h"

#include "MeshRenderQueue.h"

namespace Rendering
{
	namespace Renderer
	{
		class MeshRenderer final
		{
		public:
			struct Param
			{
				const Manager::BufferManager&	bufferMgr;
				const Manager::DefaultShaders&	defaultShaders;

				Param(const Manager::BufferManager& _bufferMgr, const Manager::DefaultShaders& _defaultShaders) 
					: bufferMgr(_bufferMgr), defaultShaders(_defaultShaders) {}
			};

			static void RenderWithoutIASetVB(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Geometry::Mesh& mesh);

			template <class CallPerMeshFunc, class CallPostRender>
			static void RenderTransparentMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::TransparentMeshRenderQueue& meshes, CallPerMeshFunc func, CallPostRender postFunc)
			{
				for (const auto mesh : meshes)
				{
					const auto& vbPool			= param.bufferMgr.GetPool<Buffer::VertexBuffer>();
					const auto* vertexBuffer	= vbPool.Find(meshPtr->GetVBKey()); assert(vertexBuffer);

					vertexBuffer->IASetBuffer(dx);
					func(meshPtr);

					RenderWithoutIASetVB(dx, param, renderType, *meshPtr);
				}

				postFunc();
			}

			template <class CallPerMeshFunc, class CallPostRender>
			static void RenderOpaqueMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::OpaqueMeshRenderQueue& meshes, CallPerMeshFunc func, CallPostRender postCall)
			{
				meshes.Iterate(
					[&dx, &param, renderType, &func](const Mesh* mesh)
					{
						const auto& vbPool	= param.bufferMgr.GetPool<Buffer::VertexBuffer>();
						const auto* vb		= vbPool.Find(mesh->GetVBKey()); assert(vb);

						vb->IASetBuffer(dx);
						func(mesh);

						RenderWithoutIASetVB(dx, param, renderType, *mesh);
					}
				);

				postCall();
			}

			template <class CallPerMeshFunc, class CallPostRender>
			static void RenderAlphaTestMeshes(Device::DirectX& dx, Param param, DefaultRenderType renderType, const Rendering::RenderQueue::AlphaTestMeshRenderQueue& meshes, CallPerMeshFunc&& cullFunc, CallPostRender&& postCall)
			{
				RenderOpaqueMeshes(dx, param, renderType, meshes, cullFunc, postCall);
			}
		};
	}
}