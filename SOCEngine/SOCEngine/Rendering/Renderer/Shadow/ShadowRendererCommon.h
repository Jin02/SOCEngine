#pragma once

#include "MeshRenderer.h"
#include "ObjectManager.h"

namespace Rendering
{
	namespace Renderer
	{
		namespace ShadowMap
		{
			struct RenderManagerParam
			{
				const MeshRenderer::RenderParam&		meshRenderParam;
				const Manager::MeshManager&				meshManager;		
				const MeshRenderer&						meshRenderer;
				const Core::ObjectManager&				objMgr;
				const Core::TransformPool&				transformPool;
				
				RenderManagerParam(	const MeshRenderer::RenderParam&		_meshRenderParam,
									const Manager::MeshManager&				_meshManager,
									const Core::ObjectManager&				_objMgr,
									const Core::TransformPool&				_transformPool,
									const MeshRenderer&						_meshRenderer,
									RenderQueue::OpaqueMeshRenderQueue&		_opaqueRenderQ,
									RenderQueue::AlphaTestMeshRenderQueue&	_alphaTestRenderQ )
					:	meshRenderParam(_meshRenderParam), meshManager(_meshManager), meshRenderer(_meshRenderer),
						objMgr(_objMgr), transformPool(_transformPool) { }
			};

			struct TempRenderQueue
			{
				RenderQueue::OpaqueMeshRenderQueue		opaqueRenderQ;
				RenderQueue::AlphaTestMeshRenderQueue	alphaTestRenderQ;
			};
		}
	}

}