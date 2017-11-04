#pragma once

#include "OpaqueMeshRenderQueue.h"

namespace Rendering
{
	namespace RenderQueue
	{
		using TransparentMeshRenderQueue	= std::vector<OpaqueMeshRenderQueue::MeshRenderQType*>;
		using AlphaBlendMeshRenderQueue		= OpaqueMeshRenderQueue;
	}
}
