#pragma once

#include "MeshRenderer.h"
#include "ObjectManager.h"

namespace Rendering
{
	namespace Renderer
	{
		struct CullingParam
		{
			const Manager::MeshManager&		meshManager;		
			const Core::ObjectManager&		objMgr;
			const Core::TransformPool&		transformPool;
			
			CullingParam(const Manager::MeshManager& _meshManager, const Core::ObjectManager& _objMgr, const Core::TransformPool& _transformPool)
				:	meshManager(_meshManager), objMgr(_objMgr), transformPool(_transformPool) { }
		};

		struct ShadowSystem
		{
			const Manager::ShadowManager&				manager;
			const ShadowAtlasMapRenderer&				renderer;
		};

		struct TempRenderQueue
		{
			RenderQueue::OpaqueMeshRenderQueue		opaqueRenderQ;
			RenderQueue::AlphaTestMeshRenderQueue	alphaTestRenderQ;
		};
	}
}