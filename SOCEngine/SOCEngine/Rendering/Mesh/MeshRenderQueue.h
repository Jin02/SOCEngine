#pragma once

#include "OpaqueMeshRenderQueue.h"

namespace Rendering
{
	namespace RenderQueue
	{
		using TransparentMeshRenderQueue	= std::vector<const Geometry::Mesh*>;
		using AlphaBlendMeshRenderQueue		= OpaqueMeshRenderQueue;		
	}
}
