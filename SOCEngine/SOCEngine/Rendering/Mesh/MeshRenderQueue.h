#pragma once

#include "OpaqueMeshRenderQueue.h"
#include "TemporaryPtr.hpp"

namespace Rendering
{
	namespace RenderQueue
	{
		using TransparentMeshRenderQueue	= std::vector<TemporaryPtr<const Geometry::Mesh>>;
		using AlphaBlendMeshRenderQueue		= OpaqueMeshRenderQueue;		
	}
}
